//
// C++ Implementation: k9avidecode
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9avidecode.h"
#include <avcodec.h>
#include <qimage.h>
#include <dlfcn.h>
#include "k9common.h"

#ifdef HAVE_SWSCALE

#include "swscale.h"

static int sws_flags = SWS_BICUBIC;

#endif

void av_free_packet_internal(AVPacket *pkt)
{
    if (pkt) {
        if (pkt->destruct) pkt->destruct(pkt);
        pkt->data = NULL; pkt->size = 0;
    }
}



k9AviDecode::k9AviDecode(QObject *parent, const char *)
        : QObject(parent) {
    CodecHandle=0;
    FormatHandle=0;
    SwscaleHandle=0;
    glibref=0;
    if (glibref==0) {
        CodecHandle=dlopen("libavcodec.so",RTLD_LAZY ); //| RTLD_GLOBAL
        FormatHandle=dlopen("libavformat.so",RTLD_LAZY);
#ifdef HAVE_SWSCALE
        SwscaleHandle=dlopen("libswscale.so",RTLD_LAZY);
        if (SwscaleHandle==0)
            SwscaleHandle=dlopen("libswscale.so.2",RTLD_LAZY);
#endif
    }
    m_error="";
    QStringList errs;
    if (!CodecHandle) {
        errs << i18n("Cannot open the library %1").arg("libavcodec");
    }
    if (!FormatHandle ) {
        errs << i18n("Cannot open the library %1").arg("libavformat");
    }
#ifdef HAVE_SWSCALE
    if (!SwscaleHandle) {
        errs << i18n("Cannot open the library %1").arg("libswscale");
    }
#endif

    av_register_all = (av_register_all_t)dlsym(FormatHandle,"av_register_all");
    av_open_input_file = (av_open_input_file_t)dlsym(FormatHandle,"av_open_input_file");
    av_find_stream_info = (av_find_stream_info_t)dlsym(FormatHandle,"av_find_stream_info");
    avcodec_find_decoder =(avcodec_find_decoder_t) dlsym(CodecHandle,"avcodec_find_decoder");
    avcodec_open = (avcodec_open_t)dlsym(CodecHandle,"avcodec_open");
    avcodec_alloc_frame = (avcodec_alloc_frame_t)dlsym(CodecHandle,"avcodec_alloc_frame");
    avpicture_get_size = (avpicture_get_size_t)dlsym(CodecHandle,"avpicture_get_size");
    av_malloc = (av_malloc_t)dlsym(CodecHandle,"av_malloc");
    avpicture_fill = (avpicture_fill_t)dlsym(CodecHandle,"avpicture_fill");
    av_read_frame = (av_read_frame_t)dlsym(FormatHandle,"av_read_frame");
    avcodec_decode_video = (avcodec_decode_video_t)dlsym(CodecHandle,"avcodec_decode_video");
    if (avcodec_decode_video==0) avcodec_decode_video=(avcodec_decode_video_t)dlsym(CodecHandle,"avcodec_decode_video2");
#ifndef HAVE_SWSCALE
    img_convert = (img_convert_t)dlsym(CodecHandle,"img_convert");
//if img_convert is null (deprecated in ffmpeg), we need libswscale
    if (!img_convert)
      errs << i18n("Cannot open the library %1").arg("libswscale");
#endif
    av_free = (av_free_t)dlsym(CodecHandle,"av_free");
    av_free_packet = (av_free_packet_t)dlsym(CodecHandle,"av_free_packet");
    if (av_free_packet==0)
         av_free_packet=av_free_packet_internal;
    avcodec_close = (avcodec_close_t)dlsym(FormatHandle,"avcodec_close");
    av_close_input_file = (av_close_input_file_t)dlsym(FormatHandle,"av_close_input_file");
    av_seek_frame=(av_seek_frame_t)dlsym(FormatHandle,"av_seek_frame");
    av_rescale_q=(av_rescale_q_t)dlsym(FormatHandle,"av_rescale_q");
    avcodec_flush_buffers=(avcodec_flush_buffers_t)dlsym(CodecHandle,"avcodec_flush_buffers");
#ifdef HAVE_SWSCALE
    sws_freeContext= (sws_freeContext_t)dlsym(SwscaleHandle,"sws_freeContext");
    sws_getContext=(sws_getContext_t)dlsym(SwscaleHandle,"sws_getContext");
    sws_scale= (sws_scale_t)dlsym(SwscaleHandle,"sws_scale");
#endif
    av_gettime=(av_gettime_t)dlsym(FormatHandle,"av_gettime");
    
    if (errs.count() >0) {
        m_error=errs.join("\n");
        k9Dialogs::error(m_error,"k9copy",QStringList());
        return;
    }
    
    av_register_all();
    m_opened=false;

    glibref++;
}


k9AviDecode::~k9AviDecode() {
    if (m_opened)
        close();
    glibref--;
    if (glibref==0) {
	if (FormatHandle)
        	dlclose(FormatHandle);
	if (CodecHandle)
        	dlclose(CodecHandle);
#ifdef HAVE_SWSCALE
	if (SwscaleHandle)
        	dlclose(CodecHandle);
#endif
	FormatHandle=0;
	CodecHandle=0;
    }

}

bool k9AviDecode::open(const QString & _fileName) {
    m_error="";
    if (m_opened)
        close();

    // Open video file
    if (av_open_input_file(&m_FormatCtx, _fileName.toUtf8(), NULL, 0, NULL)!=0) {
        m_error=i18n("Couldn't open the file %1").arg(_fileName);
        return false; // Couldn't open file}
    }
// Retrieve stream information
    if (av_find_stream_info(m_FormatCtx)<0) {
        m_error =i18n("Couldn't find stream information");
        return false; // Couldn't find stream information
    }
    uint i;

// Find the first video stream
    m_videoStream=-1;
    for (i=0; i<m_FormatCtx->nb_streams; i++)
        if (m_FormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
            m_videoStream=i;
            break;
        }
    if (m_videoStream==-1) {
        m_error=i18n("The file doesn't contain any video stream");
        return false; // Didn't find a video stream
    }

    // Get a pointer to the codec context for the video stream
    m_CodecCtx=m_FormatCtx->streams[m_videoStream]->codec;


// Find the decoder for the video stream
    m_Codec=avcodec_find_decoder(m_CodecCtx->codec_id);
    if (m_Codec==NULL) {
        m_error=i18n("Unsupported codec");
        return false; // Codec not found
    }
// Open codec
    if (avcodec_open(m_CodecCtx, m_Codec)<0) {
        m_error =i18n("Could'nt open the codec");
        return false; // Could not open codec
    }


// Allocate video frame
    m_Frame=avcodec_alloc_frame();

// Allocate an AVFrame structure
    m_FrameRGB=avcodec_alloc_frame();
    if (m_FrameRGB==NULL) {
        m_error =i18n ("Unable to allocate memory for frames");
        return false;
    }


    int numBytes;
// Determine required buffer size and allocate buffer
    numBytes=avpicture_get_size(PIX_FMT_RGB24, m_CodecCtx->width,
                                m_CodecCtx->height);
    m_buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

// Assign appropriate parts of buffer to image planes in pFrameRGB
// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
// of AVPicture
    avpicture_fill((AVPicture *)m_FrameRGB, m_buffer, PIX_FMT_RGB24,
                   m_CodecCtx->width, m_CodecCtx->height);

 

    m_duration=(double)(calcDuration() + m_FormatCtx->start_time) / AV_TIME_BASE;
    m_opened=true;
    m_fileName=_fileName;
    return true;
}


int64_t k9AviDecode::calcDuration() {
    if (m_FormatCtx->duration >0)
	return m_FormatCtx->duration;
    else {
	int64_t duration=0;
	AVPacket packet;
	av_seek_frame(m_FormatCtx, m_videoStream, 0, AVSEEK_FLAG_BACKWARD );
	while (av_read_frame(m_FormatCtx, &packet)>=0) {
	        duration += packet.duration;
		av_free_packet(&packet);
	}
	return duration;
    }
}

void k9AviDecode::seek(double _seconds) {
    AVRational time_base = m_FormatCtx->streams[m_videoStream]->time_base;
    int64_t fspos = (int64_t)(_seconds * AV_TIME_BASE);
    fspos=av_rescale_q(fspos, AV_TIME_BASE_Q,  time_base);
    int i=av_seek_frame(m_FormatCtx, m_videoStream, fspos, AVSEEK_FLAG_BACKWARD );
//    double pos=av_gettime() / 1000000;
}

void k9AviDecode::readFrame(double _seconds) {
    AVRational time_base = m_FormatCtx->streams[m_videoStream]->time_base;
    int64_t fspos = (int64_t)(_seconds * AV_TIME_BASE);
    fspos=av_rescale_q(fspos, AV_TIME_BASE_Q,  time_base);
    int res=av_seek_frame(m_FormatCtx, m_videoStream, fspos, AVSEEK_FLAG_BACKWARD );
    avcodec_flush_buffers(m_CodecCtx);
    int frameFinished=0;
    AVPacket packet;
#ifdef HAVE_SWSCALE
    struct SwsContext *toRGB_convert_ctx;
#endif
    bool bFound=false;
    while (av_read_frame(m_FormatCtx, &packet)>=0 &&  !bFound) {
        // Is this a packet from the video stream?
        if (packet.stream_index==m_videoStream) {
            // Decode video frame
            avcodec_decode_video(m_CodecCtx, m_Frame, &frameFinished,
                                 packet.data, packet.size);

            // Did we get a video frame?
            // example: http://blog.tomaka17.com/2012/03/libavcodeclibavformat-tutorial/
            if (frameFinished) {
//            if (m_Frame->pts >=fspos)
		int64_t cur_dts=fspos;
                if (m_videoStream >= 0 && m_videoStream < static_cast<int>(m_FormatCtx->nb_streams)) //cast for compile warning.
                  cur_dts = m_FormatCtx->streams[m_videoStream]->cur_dts;
                if (cur_dts >=fspos) {
                    bFound=true;
#ifndef HAVE_SWSCALE
                  // Convert the image from its native format to RGB
                    img_convert((AVPicture *)m_FrameRGB, AV_PIX_FMT_RGB24,
                                (AVPicture*)m_Frame, m_CodecCtx->pix_fmt,
                                m_CodecCtx->width, m_CodecCtx->height);
                    SaveFrame(m_FrameRGB, m_CodecCtx->width,
                              m_CodecCtx->height);
#else
		    toRGB_convert_ctx=sws_getContext(m_CodecCtx->width, m_CodecCtx->height, m_CodecCtx->pix_fmt, m_CodecCtx->width, m_CodecCtx->height, PIX_FMT_RGB24, sws_flags,NULL,NULL,NULL);
        		   sws_scale(toRGB_convert_ctx, m_Frame->data, m_Frame->linesize, 0, m_CodecCtx->height, m_FrameRGB->data,m_FrameRGB->linesize);
                    // convert frame to QImage
                    SaveFrame(m_FrameRGB, m_CodecCtx->width,
                              m_CodecCtx->height);
		    sws_freeContext(toRGB_convert_ctx);
#endif
                }
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }

}

void k9AviDecode::SaveFrame(AVFrame *pFrame, int width, int height) {
    QImage pix;
    int len =(int) (3*width*height);
    char c[255];
    // Write header
    sprintf(c,"P6\n%d %d\n255\n", width, height);
    char *s= new char[len+strlen(c)];
    memcpy(s,c,strlen(c));
    memcpy(s+strlen(c),pFrame->data[0], len);
    pix.loadFromData((uchar*)s,strlen(c)+len);
    delete[] s;
    emit drawFrame( pix);

}

void k9AviDecode::close() {
    if (m_opened) {
        // Free the RGB image
        av_free(m_buffer);
        av_free(m_FrameRGB);

        // Free the YUV frame
        av_free(m_Frame);

        // Close the codec
        avcodec_close(m_CodecCtx);

        // Close the video file
        av_close_input_file(m_FormatCtx);
        m_opened=false;
    }
}


double k9AviDecode::getDuration() const {
    return m_duration;
}


bool k9AviDecode::opened() const {
    return m_opened;
}


QString k9AviDecode::getFileName() const {
    return m_fileName;
}


QString k9AviDecode::getError() const {
    return m_error;
}
