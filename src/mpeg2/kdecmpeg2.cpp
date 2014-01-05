/**************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   k9copy@free.fr                                                            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "kdecmpeg2.h"
#include <qpixmap.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qapplication.h>
#include "k9saveimage.h"
#include "k9decodethread.h"
extern "C" {
#include <mpeg2dec/mpeg2.h>
#include <mpeg2dec/mpeg2convert.h>
}



void kDecMPEG2::init() {
  demux_pid=0;
  m_thread=NULL;
  demux_track=0xe0;
  decoder = mpeg2_init ();
  m_opened=true;
  if (decoder == NULL) {
	  fprintf (stderr, "Could not allocate a decoder object.\n");
	  exit (1);
  }  
}

kDecMPEG2::kDecMPEG2(k9DecodeThread *_thread) {
   init();
   m_thread=_thread;
}


kDecMPEG2::kDecMPEG2(){
   init();
}


#define DEMUX_PAYLOAD_START 1
int kDecMPEG2::demux (uint8_t * buf, uint8_t * end, int flags)
{
    static int mpeg1_skip_table[16] = {
	0, 0, 4, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    /*
     * the demuxer keeps some state between calls:
     * if "state" = DEMUX_HEADER, then "head_buf" contains the first
     *     "bytes" bytes from some header.
     * if "state" == DEMUX_DATA, then we need to copy "bytes" bytes
     *     of ES data before the next header.
     * if "state" == DEMUX_SKIP, then we need to skip "bytes" bytes
     *     of data before the next header.
     *
     * NEEDBYTES makes sure we have the requested number of bytes for a
     * header. If we dont, it copies what we have into head_buf and returns,
     * so that when we come back with more data we finish decoding this header.
     *
     * DONEBYTES updates "buf" to point after the header we just parsed.
     */

#define DEMUX_HEADER 0
#define DEMUX_DATA 1
#define DEMUX_SKIP 2
    static int state = DEMUX_SKIP;
    static int state_bytes = 0;
    static uint8_t head_buf[264];

    uint8_t * header;
    int bytes;
    int len;

#define NEEDBYTES(x)						\
    do {							\
	int missing;						\
								\
	missing = (x) - bytes;					\
	if (missing > 0) {					\
	    if (header == head_buf) {				\
		if (missing <= end - buf) {			\
		    memcpy (header + bytes, buf, missing);	\
		    buf += missing;				\
		    bytes = (x);				\
		} else {					\
		    memcpy (header + bytes, buf, end - buf);	\
		    state_bytes = bytes + end - buf;		\
		    return 0;					\
		}						\
	    } else {						\
		memcpy (head_buf, header, bytes);		\
		state = DEMUX_HEADER;				\
		state_bytes = bytes;				\
		return 0;					\
	    }							\
	}							\
    } while (0)

#define DONEBYTES(x)		\
    do {			\
	if (header != head_buf)	\
	    buf = header + (x);	\
    } while (0)

    if (flags & DEMUX_PAYLOAD_START)
    	goto payload_start;
    switch (state) {
        case DEMUX_HEADER:
        	if (state_bytes > 0) {
        	    header = head_buf;
        	    bytes = state_bytes;
        	    goto continue_header;
    	    }
  	      break;
        case DEMUX_DATA:
          	if (demux_pid || (state_bytes > end - buf)) {
          	    decode_mpeg2 (buf, end);
          	    state_bytes -= end - buf;
          	    return 0;
          	}
          	decode_mpeg2 (buf, buf + state_bytes);
          	buf += state_bytes;
          	break;
        case DEMUX_SKIP:
          	if (demux_pid || (state_bytes > end - buf)) {
          	    state_bytes -= end - buf;
          	    return 0;
          	}
          	buf += state_bytes;
          	break;
    }
    m_pause=false;
    while (!m_pause) {
      	if (demux_pid) {
      	    state = DEMUX_SKIP;
      	    return 0;
      	}
        payload_start:
      	header = buf;
      	bytes = end - buf;
continue_header:
      	NEEDBYTES (4);
      	if (header[0] || header[1] || (header[2] != 1)) {
	        if (demux_pid) {
        		state = DEMUX_SKIP;
	        	return 0;
	        } else
          if (header != head_buf) {
		        buf++;
		        goto payload_start;
	        } else {
         		header[0] = header[1];
         		header[1] = header[2];
         		header[2] = header[3];
         		bytes = 3;
         		goto continue_header;
	        }
	      }
      	if (demux_pid) {
      	    if ((header[3] >= 0xe0) && (header[3] <= 0xef))
          		goto pes;
      	    fprintf (stderr, "bad stream id %x\n", header[3]);
      	    exit (1);
      	}
      	switch (header[3]) {
      	case 0xb9:	/* program end code */
      	    /* DONEBYTES (4); */
      	    /* break;         */
      	    return 1;
      	case 0xba:	/* pack header */
      	    NEEDBYTES (5);
      	    if ((header[4] & 0xc0) == 0x40) {	/* mpeg2 */
           		NEEDBYTES (14);
          	  len = 14 + (header[13] & 7);
      		    NEEDBYTES (len);
      		    DONEBYTES (len);
      		/* header points to the mpeg2 pack header */
      	    } else if ((header[4] & 0xf0) == 0x20) {	/* mpeg1 */
      		    NEEDBYTES (12);
      		    DONEBYTES (12);
      		/* header points to the mpeg1 pack header */
      	    } else {
      		    fprintf (stderr, "weird pack header\n");
      		    DONEBYTES (5);
      	    }
      	    break;
      	default:
	        if (header[3] == demux_track) {
pes:
		        NEEDBYTES (7);
		        if ((header[6] & 0xc0) == 0x80) {	/* mpeg2 */
		        NEEDBYTES (9);
		        len = 9 + header[8];
		        NEEDBYTES (len);
		        /* header points to the mpeg2 pes header */
		        if (header[7] & 0x80) {
			        uint32_t pts, dts;

			        pts = (((header[9] >> 1) << 30) |
			              (header[10] << 22) | ((header[11] >> 1) << 15) |
			              (header[12] << 7) | (header[13] >> 1));
			        dts = (!(header[7] & 0x40) ? pts :
			              (((header[14] >> 1) << 30) |
				            (header[15] << 22) |
				            ((header[16] >> 1) << 15) |
				            (header[17] << 7) | (header[18] >> 1)));
			        mpeg2_tag_picture (decoder, pts, dts);
		        }
		      } else {	/* mpeg1 */
		        int len_skip;
		        uint8_t * ptsbuf;

		        len = 7;
		        while (header[len - 1] == 0xff) {
			        len++;
			        NEEDBYTES (len);
			        if (len > 23) {
			          fprintf (stderr, "too much stuffing\n");
			          break;
			        }
		        }
		        if ((header[len - 1] & 0xc0) == 0x40) {
		      	  len += 2;
			        NEEDBYTES (len);
		        }
		        len_skip = len;
		        len += mpeg1_skip_table[header[len - 1] >> 4];
		        NEEDBYTES (len);
		        /* header points to the mpeg1 pes header */
		        ptsbuf = header + len_skip;
		        if ((ptsbuf[-1] & 0xe0) == 0x20) {
			        uint32_t pts, dts;

			        pts = (((ptsbuf[-1] >> 1) << 30) |
			              (ptsbuf[0] << 22) | ((ptsbuf[1] >> 1) << 15) |
			              (ptsbuf[2] << 7) | (ptsbuf[3] >> 1));
			        dts = (((ptsbuf[-1] & 0xf0) != 0x30) ? pts :
			              (((ptsbuf[4] >> 1) << 30) |
				            (ptsbuf[5] << 22) | ((ptsbuf[6] >> 1) << 15) |
			            	(ptsbuf[7] << 7) | (ptsbuf[18] >> 1)));
	        		mpeg2_tag_picture (decoder, pts, dts);
		        }
		      }
		      DONEBYTES (len);
		      bytes = 6 + (header[4] << 8) + header[5] - len;
		      if (demux_pid || (bytes > end - buf)) {
		         decode_mpeg2 (buf, end);
		         state = DEMUX_DATA;
		         state_bytes = bytes - (end - buf);
		         return 0;
		      } else if (bytes > 0) {
		         decode_mpeg2 (buf, buf + bytes);
		         buf += bytes;
		      }
	      } else if (header[3] < 0xb9) {
	      	fprintf (stderr,"looks like a video stream, not system stream\n");
      		DONEBYTES (4);
	      } else {
	      	NEEDBYTES (6);
		      DONEBYTES (6);
		      bytes = (header[4] << 8) + header[5];
		      if (bytes > end - buf) {
		        state = DEMUX_SKIP;
		        state_bytes = bytes - (end - buf);
		        return 0;
		      }
		      buf += bytes;
	      }
    	}
    }
    return 0;
}

void kDecMPEG2::sync() {
   int t=40- m_timer.elapsed();
   if (t>0 && m_thread!=NULL) {
	m_thread->sleepms(t);
	
    }
    m_timer.restart();
}

void kDecMPEG2::save_ppm (int width, int height, uint8_t * buf, int )
{
    m_thread->pgm_save((uchar*) buf,0,width,height);
}

void kDecMPEG2::decode_mpeg2(uint8_t * current, uint8_t * end)
{
    const mpeg2_info_t * info;
    mpeg2_state_t state;
    int framenum = 0;
    mpeg2_buffer (decoder, current, end);

    info = mpeg2_info (decoder);

    while (1) {
	    state = mpeg2_parse (decoder);
	    switch (state) {
  	    case STATE_BUFFER:
  	      return;
  	    case STATE_SEQUENCE: 
  	      		mpeg2_convert (decoder, mpeg2convert_rgb (MPEG2CONVERT_RGB,24), NULL);
  	      break;
	    case STATE_PICTURE:
		break;
  	    case STATE_SLICE:
  	    case STATE_END:
  	    case STATE_INVALID_END:
  		    if (info->display_fbuf  ) save_ppm (info->sequence->width, info->sequence->height,info->display_fbuf->buf[0], framenum++);
  	      break;
  	    default:
  	      break;
  	  }
   }
}

int kDecMPEG2::decode (uint8_t * buf, uint8_t * end, int )
{
  mutex.lock();
  if (buf && end)
        demux (buf,end,0);
  mutex.unlock();
  return 0;
}
kDecMPEG2::~kDecMPEG2(){
   if (m_opened)
   	mpeg2_close (decoder);
}

void kDecMPEG2::restart() { 
  mutex.lock();
  if (m_opened)
  	mpeg2_close (decoder);
  decoder = mpeg2_init ();
  m_opened=true;
  if (decoder == NULL) {
	  fprintf (stderr, "Could not allocate a decoder object.\n");
  }  
  mutex.unlock();  
}


void kDecMPEG2::start() {
  m_timer.start();
  decoder = mpeg2_init ();
  m_opened=true;
  if (decoder == NULL) {
	  fprintf (stderr, "Could not allocate a decoder object.\n");
	  exit (1);
  }  
}

void kDecMPEG2::stop() {
   if (m_opened)
	mpeg2_close(decoder);
   m_opened=false;
}

void kDecMPEG2::pause() {
    m_pause=true;
}

