//
// C++ Interface: k9avidecode
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9FFMPEG_H
#define K9FFMPEG_H

#include <QString>
#include <avcodec.h>
#include <avformat.h>
#include <avio.h>
#ifdef HAVE_SWSCALE
#include <swscale.h>
#endif
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/


//typedef dvd_file_t * (*DVDOpenFile_t) ( dvd_reader_t *, int, dvd_read_domain_t );
typedef void (*av_register_all_t) (void);
typedef void (*avcodec_init_t)(void);
typedef int64_t (*av_gettime_t) (void);
typedef int (*av_open_input_file_t)(AVFormatContext **, const char *,AVInputFormat *,int, AVFormatParameters *);
typedef int (*av_find_stream_info_t)(AVFormatContext *);
typedef AVCodec* (*avcodec_find_decoder_t)(enum CodecID);
typedef int (*avcodec_open_t)(AVCodecContext *, AVCodec *);
typedef AVFrame * (*avcodec_alloc_frame_t)(void);
typedef AVCodecContext * (*avcodec_alloc_context_t)(void);

typedef int (*avpicture_get_size_t)(int , int , int );
typedef void * (*av_malloc_t)(unsigned int );
typedef int (*avpicture_fill_t)(AVPicture *, uint8_t *,int , int , int);
typedef int (*av_read_frame_t)(AVFormatContext *, AVPacket *);

typedef int (*avcodec_decode_video_t)(AVCodecContext *, AVFrame *,int *, uint8_t *, int );
typedef int (*avcodec_encode_audio_t)(AVCodecContext *avctx, uint8_t *buf, int buf_size, const short *samples);

typedef int (*img_convert_t)(AVPicture *, int , const AVPicture *, int ,int, int);
typedef void (*av_free_t)(void *);
typedef int (*avcodec_close_t)(AVCodecContext *);
typedef void (*av_close_input_file_t)(AVFormatContext *);
typedef int (*av_seek_frame_t)(AVFormatContext *,int,int64_t timestamp,int flags); 		
typedef int64_t (*av_rescale_q_t)(int64_t , AVRational , AVRational )	;
typedef void (*avcodec_flush_buffers_t)(AVCodecContext *);

#ifdef HAVE_SWSCALE
typedef void (*sws_freeContext_t)(struct SwsContext *swsContext);
typedef struct SwsContext* (*sws_getContext_t)(int srcW, int srcH, int srcFormat, int dstW, int dstH, int dstFormat, int flags,
                                  SwsFilter *srcFilter, SwsFilter *dstFilter, double *param);
typedef int (*sws_scale_t)(struct SwsContext *context, uint8_t* src[], int srcStride[], int srcSliceY,int srcSliceH, uint8_t* dst[], int dstStride[]);
#endif


typedef int (*url_fopen_t)(ByteIOContext **s, const char *filename, int flags);

typedef ReSampleContext * (*audio_resample_init_t)(int output_channels, int input_channels,int output_rate,int input_rate);
typedef int (*av_write_header_t)(AVFormatContext *);
typedef int (*avcodec_decode_audio2_t)(AVCodecContext *avctx, int16_t *samples,int *frame_size_ptr,const uint8_t *buf, int buf_size);
typedef int (*avcodec_decode_audio_t)(AVCodecContext *avctx, int16_t *samples,int *frame_size_ptr, const uint8_t *buf, int buf_size);

typedef AVOutputFormat *(*guess_format_t)(const char *short_name, const char *filename, const char *mime_type);
typedef int (*audio_resample_t)(ReSampleContext *s, short *output, short *input, int nb_samples);
typedef AVFormatContext *(*av_alloc_format_context_t)(void);
typedef AVCodec *(*avcodec_find_encoder_t)(enum CodecID id);
typedef void (*av_init_packet_t)(AVPacket *pkt);
typedef int (*av_write_frame_t)(AVFormatContext *s, AVPacket *pkt);
typedef int (*av_write_trailer_t)(AVFormatContext *s);
typedef int (*av_set_parameters_t)(AVFormatContext *s, AVFormatParameters *ap);
typedef void (*dump_format_t)(AVFormatContext *ic, int index, const char *url, int is_output);
typedef AVStream *(*av_new_stream_t)(AVFormatContext *s, int id);


class k9FFMpeg 
{
public:
    k9FFMpeg();

    ~k9FFMpeg();
    static k9FFMpeg *instance();
public:
    avcodec_init_t avcodec_init;
    avcodec_alloc_context_t avcodec_alloc_context;
    av_register_all_t av_register_all;
    av_open_input_file_t av_open_input_file;
    av_find_stream_info_t av_find_stream_info;
    avcodec_find_decoder_t avcodec_find_decoder;
    avcodec_open_t avcodec_open;
    avcodec_alloc_frame_t avcodec_alloc_frame;
    av_seek_frame_t av_seek_frame;
    avpicture_get_size_t avpicture_get_size;
    av_malloc_t av_malloc;
    avpicture_fill_t avpicture_fill;
    av_read_frame_t av_read_frame;
    avcodec_decode_video_t avcodec_decode_video;
    img_convert_t img_convert;
    av_free_t av_free;
    avcodec_close_t avcodec_close;
    av_close_input_file_t av_close_input_file;
    av_rescale_q_t av_rescale_q;
    av_gettime_t av_gettime;
    avcodec_flush_buffers_t avcodec_flush_buffers;
#ifdef HAVE_SWSCALE
    sws_freeContext_t sws_freeContext;
    sws_getContext_t sws_getContext;
    sws_scale_t sws_scale;
#endif
    url_fopen_t url_fopen;
    audio_resample_init_t audio_resample_init;
    av_write_header_t av_write_header;
    avcodec_decode_audio2_t avcodec_decode_audio2;
    avcodec_decode_audio_t avcodec_decode_audio;
    avcodec_encode_audio_t avcodec_encode_audio;
    guess_format_t guess_format;
    audio_resample_t audio_resample;
    av_alloc_format_context_t av_alloc_format_context;
    avcodec_find_encoder_t avcodec_find_encoder;
    av_init_packet_t av_init_packet;
    av_write_frame_t av_write_frame;
    av_write_trailer_t av_write_trailer;
    av_set_parameters_t av_set_parameters;
    dump_format_t dump_format;
    av_new_stream_t av_new_stream;
private:
    void *CodecHandle;
    void *FormatHandle;
    void *SwscaleHandle;
    void *UtilHandle;
    QString m_error;
};

#endif
