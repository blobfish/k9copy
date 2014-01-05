//
// C++ Implementation: k9plaympeg2
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9plaympeg2.h"
#include "kdecmpeg2.h"
#include "dvdnav.h"

#include <qapplication.h>

k9PlayMPEG2::k9PlayMPEG2()
{
   m_title=NULL;
   m_stopped=true;
   connect (&m_timer,SIGNAL(timeout()),this,SLOT(timeout()));
}


k9PlayMPEG2::~k9PlayMPEG2()
{
    stop();
}

void k9PlayMPEG2::updatePos( uint32_t _position) {
    m_idxLect=_position;
    m_decoder.clear();
}

void k9PlayMPEG2::timeout() {
    emit setPosition(m_position);
}

#define DVD_LANGUAGE "en"
#define DVD_READ_CACHE 1

void k9PlayMPEG2::playTitle() {
    dvdnav_t *dvdnav;
    uint8_t mem[DVD_VIDEO_LB_LEN];
    int finished = 0;
    int32_t tt = 0,ptt=0;
    uint32_t pos, lgr;
    int title=m_title->getnumTitle();


    /* open dvdnav handle */
    if (dvdnav_open(&dvdnav, m_device.toUtf8(),m_dvd) != DVDNAV_STATUS_OK) {
        setError("ERR:Error on dvdnav_open\n");
        return ;
    }

    /* set read ahead cache usage */
    if (dvdnav_set_readahead_flag(dvdnav, DVD_READ_CACHE) != DVDNAV_STATUS_OK) {
        setError(QString("ERR:Error on dvdnav_set_readahead_flag: %1\n").arg(dvdnav_err_to_string(dvdnav)));
        return;
    }

    /* set the language */
    if (dvdnav_menu_language_select(dvdnav, DVD_LANGUAGE) != DVDNAV_STATUS_OK ||
            dvdnav_audio_language_select(dvdnav, DVD_LANGUAGE) != DVDNAV_STATUS_OK ||
            dvdnav_spu_language_select(dvdnav, DVD_LANGUAGE) != DVDNAV_STATUS_OK) {
        setError(QString("ERR:Error on setting languages: %1\n").arg(dvdnav_err_to_string(dvdnav)));
        return ;
    }

    /* set the PGC positioning flag to have position information relatively to the
     * whole feature instead of just relatively to the current chapter */
    if (dvdnav_set_PGC_positioning_flag(dvdnav, 1) != DVDNAV_STATUS_OK) {
        setError(QString("ERR:Error on dvdnav_set_PGC_positioning_flag: %1\n").arg(dvdnav_err_to_string(dvdnav)));
        return ;
    }

    int32_t parts;
    dvdnav_get_number_of_parts(dvdnav , title, &parts);
     
    if (m_chapter==0)
    	dvdnav_title_play(dvdnav , title);
    else
        dvdnav_part_play(dvdnav , title,m_chapter);
    /* the read loop which regularly calls dvdnav_get_next_block
     * and handles the returned events */

    while (!finished && !m_stopped && qApp!=NULL) {
        int result, event, len;
        uint8_t *buf = mem;

        if (m_idxLect !=0xFFFFFFFF) {
  	    dvdnav_sector_search(dvdnav, m_idxLect,SEEK_SET);
	    m_idxLect=0xFFFFFFFF;
	}


        /* the main reading function */
#ifdef DVD_READ_CACHE

        result = dvdnav_get_next_cache_block(dvdnav, &buf, &event, &len);
#else

        result = dvdnav_get_next_block(dvdnav, buf, &event, &len);
#endif


        if (result == DVDNAV_STATUS_ERR) {
            setError(QString("ERR:Error getting next block: %1\n").arg(dvdnav_err_to_string(dvdnav)));
            return;
        }

        switch (event) {
        case DVDNAV_NAV_PACKET:
            {
		dvdnav_current_title_info(dvdnav, &tt, &ptt);
		dvdnav_get_position(dvdnav, &pos, &lgr);

		if (tt != title)
			finished=1;
	
		if (finished==0 && buf[17]==0xE0 && !m_stopped) {
			m_decoder.addData( buf,len);
		}
	//	if (qApp->tryLock()) {
		   m_position= pos;
	//	   qApp->unlock();
	//	}


            }
	    break;
	//removed break --> save
        case DVDNAV_BLOCK_OK:
            /* We have received a regular block of the currently playing MPEG stream.*/
		if (!m_stopped)
		   m_decoder.addData( buf,len);
            break;
        case DVDNAV_NOP:
            /* Nothing to do here. */
            break;
        case DVDNAV_STILL_FRAME:
            /* We have reached a still frame. A real player application would wait
             * the amount of time specified by the still's length while still handling
             * user input to make menus and other interactive stills work.
             * A length of 0xff means an indefinite still which has to be skipped
             * indirectly by some user interaction. */
            {
//                dvdnav_still_event_t *still_event = (dvdnav_still_event_t *)buf;
                dvdnav_still_skip(dvdnav);
            }
            break;
        case DVDNAV_WAIT:
            /* We have reached a point in DVD playback, where timing is critical.
             * Player application with internal fifos can introduce state
             * inconsistencies, because libdvdnav is always the fifo's length
             * ahead in the stream compared to what the application sees.
             * Such applications should wait until their fifos are empty
             * when they receive this type of event. */
            dvdnav_wait_skip(dvdnav);
            break;
        case DVDNAV_SPU_CLUT_CHANGE:
            /* Player applications should pass the new colour lookup table to their
             * SPU decoder */
            break;
        case DVDNAV_SPU_STREAM_CHANGE:
            /* Player applications should inform their SPU decoder to switch channels */
            break;
        case DVDNAV_AUDIO_STREAM_CHANGE:
            /* Player applications should inform their audio decoder to switch channels */
            break;
        case DVDNAV_HIGHLIGHT:
            /* Player applications should inform their overlay engine to highlight the
             * given button */
            {
//                dvdnav_highlight_event_t *highlight_event = (dvdnav_highlight_event_t *)buf;
            }
            break;
        case DVDNAV_VTS_CHANGE:
            /* Some status information like video aspect and video scale permissions do
             * not change inside a VTS. Therefore this event can be used to query such
             * information only when necessary and update the decoding/displaying
             * accordingly. */
            break;
        case DVDNAV_CELL_CHANGE:
//		dvdnav_get_position(dvdnav, &pos, &lgr);
            break;
        case DVDNAV_HOP_CHANNEL:
            /* This event is issued whenever a non-seamless operation has been executed.
             * Applications with fifos should drop the fifos content to speed up responsiveness. */
            break;
        case DVDNAV_STOP:
            /* Playback should end here. */
            {
                finished = 1;
            }
            break;
        default:
            finished = 1;
            break;
        }

#ifdef DVD_READ_CACHE
        dvdnav_free_cache_block(dvdnav, buf);
#endif

    }
    m_decoder.setNoData();
    /* destroy dvdnav handle */
    dvdnav_close(dvdnav);

}


void k9PlayMPEG2::run() {
    m_stopped=false;

    m_idxLect=m_startSector;
    m_decoder.start(QThread::LowPriority);

    playTitle();
    m_decoder.Stop();
    m_decoder.wait();
    m_timer.stop();
}

void k9PlayMPEG2::stop() {
   m_stopped=true;

   m_decoder.setNoData();
   m_decoder.clear();
/* 
  m_decoder.wait();
*/

   wait();

}

void k9PlayMPEG2::play() {
  if (m_stopped && m_title!=NULL)  {
	m_stopped=false;
    m_startSector=m_title->getChapter( 0)->getstartSector();
    m_lastSector= m_startSector + m_title->getsectors(); //m_title->getChapter(m_title->getchapterCount()-1)->getendSector();
    m_position=m_startSector;
    emit setPosition( m_startSector);
    emit setMax( m_lastSector);
    emit setMin( m_startSector);
    start();
    m_timer.start(100);


	//open(m_dvd,m_device,m_title,m_chapter);
  }
}

k9DecodeThread *k9PlayMPEG2::getDecoder() {
  return &m_decoder;
}

void k9PlayMPEG2::open (dvd_reader_t *dvd,const QString & device,k9DVDTitle * title,int chapter=0) {
    m_dvd=dvd;
    m_chapter=chapter;
    int  ret = 0;
    struct stat dvd_stat;
    QString c;
    m_idxLect=0xFFFFFFFF;

    stop();

    m_title=title;
    m_device=device;

    m_startSector=0;
    m_lastSector=0;
    ret = stat(device.toUtf8(), &dvd_stat);
  /*  if ( ret < 0 ) {
        c=i18n("Can't find device %1\n").arg(device.latin1());
        setError(c);
        return;
    }
*/
    m_title=title;

    m_startSector=m_title->getChapter( 0)->getstartSector();
    m_lastSector= m_startSector + m_title->getsectors(); //m_title->getChapter(m_title->getchapterCount()-1)->getendSector();

    emit setPosition( m_startSector);
    emit setMax( m_lastSector);
    emit setMin( m_startSector);

    
    //   start();
}

