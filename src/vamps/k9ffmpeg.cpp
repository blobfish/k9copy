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
#include "k9common.h"
#include "k9ffmpeg.h"
#ifdef HAVE_SWSCALE
#include "swscale.h"
#endif
#include <dlfcn.h>
#include <klocale.h>

k9FFMpeg _FFMpegInstance;

k9FFMpeg::k9FFMpeg(){
    CodecHandle=0;
    FormatHandle=0;
    SwscaleHandle=0;
        CodecHandle=dlopen("libavcodec.so",RTLD_LAZY ); //| RTLD_GLOBAL
        FormatHandle=dlopen("libavformat.so",RTLD_LAZY);
#ifdef HAVE_SWSCALE
	SwscaleHandle=dlopen("libswscale.so",RTLD_LAZY);
#endif
	UtilHandle=dlopen("libavutil.so",RTLD_LAZY);
    if (!CodecHandle) {
        m_error =i18n("Cannot open then library %1","libavcodec");
        return;
    }
    if (!FormatHandle) {
        m_error =i18n("Cannot open then library %1","libavformat");
        return;
    }
    if (!UtilHandle) {
	m_error =i18n("Cannot open the library %1","libavutil");
	return;
    }
#ifdef HAVE_SWSCALE
    if (!SwscaleHandle) {
        m_error =i18n("Cannot open then library %1","libswscale");
        return;
    }
#endif
    avcodec_init =(avcodec_init_t)dlsym(CodecHandle,"avcodec_init");
    av_register_all = (av_register_all_t)dlsym(FormatHandle,"av_register_all");
    avcodec_alloc_context=(avcodec_alloc_context_t)dlsym(CodecHandle,"avcodec_alloc_context");
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
    img_convert = (img_convert_t)dlsym(CodecHandle,"img_convert");
    av_free = (av_free_t)dlsym(CodecHandle,"av_free");
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
    url_fopen=(url_fopen_t)dlsym(FormatHandle,"url_fopen");

    audio_resample_init=(audio_resample_init_t)dlsym(CodecHandle,"audio_resample_init");
    avcodec_decode_audio2=(avcodec_decode_audio2_t)dlsym(CodecHandle,"avcodec_decode_audio2");
    avcodec_decode_audio=(avcodec_decode_audio_t)dlsym(CodecHandle,"avcodec_decode_audio");

    avcodec_encode_audio=(avcodec_encode_audio_t)dlsym(CodecHandle,"avcodec_encode_audio");
    guess_format=(guess_format_t)dlsym(FormatHandle,"guess_format");
    audio_resample=(audio_resample_t) dlsym(CodecHandle,"audio_resample");
    av_write_header=(av_write_header_t)dlsym(FormatHandle,"av_write_header");
    av_alloc_format_context=(av_alloc_format_context_t)dlsym(FormatHandle,"av_alloc_format_context");
    avcodec_find_encoder=(avcodec_find_encoder_t)dlsym(CodecHandle,"avcodec_find_encoder");
    av_init_packet=(av_init_packet_t)dlsym(FormatHandle,"av_init_packet");
    av_write_frame=(av_write_frame_t)dlsym(FormatHandle,"av_write_frame");
    av_write_trailer=(av_write_trailer_t)dlsym(FormatHandle,"av_write_trailer");
    av_set_parameters=(av_set_parameters_t)dlsym(FormatHandle,"av_set_parameters");
    dump_format=(dump_format_t)dlsym(FormatHandle,"dump_format");
    av_new_stream=(av_new_stream_t)dlsym(FormatHandle,"av_new_stream");
}


k9FFMpeg::~k9FFMpeg() {
	if (FormatHandle)
        	dlclose(FormatHandle);
	if (CodecHandle)
        	dlclose(CodecHandle);
#ifdef HAVE_SWSCALE
	if (SwscaleHandle)
        	dlclose(CodecHandle);
#endif

}

k9FFMpeg *k9FFMpeg::instance() {
   return &_FFMpegInstance;

}