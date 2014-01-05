//
// C++ Interface: k9vamps
//
// Description: A transcription from Vamps  in C++
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9vamps.h"
#include <qapplication.h>
#include <QtDebug>

void k9vamps::setNoData() {
    noData=true;
    wDataRead.wakeAll();
    wDataReady.wakeAll();
}

void k9vamps::addData(uchar *data,uint size) {
    while (1) {
        if (m_fifo->freespace()>=size) {
            m_fifo->enqueue(data,size);
            wDataReady.wakeAll();
            break;
        } else {
	    QMutex m;
	    m.lock();
            wDataRead.wait(&m);
	    m.unlock();
	}
    }
}


int k9vamps::readData(uchar * data,uint size) {
    uint size2=size;
    uint32_t readSize=0,s=0;
    
    while (1) {
	// is there data in the buffer?
	if (m_fifo->count() >0) {
		// s= size of data that we will read (maximum = size)
		s=(m_fifo->count()) <size2 ? (m_fifo->count()) : size2;
		// increments the number of readen bytes
		readSize+=s;
		// decrements the number of max bytes to read 
		size2-=s;
		//moves bytes from buffer to output
		m_fifo->dequeue(data,s);
		//moves the position of output buffer to receive next bytes
		data+=s;
		//there's now free space in input buffer, we can wake the injection thread
		wDataRead.wakeAll();
	}
	// break the loop if injection thread terminated or we got what we want (size bytes)
	// otherwise, we're waiting for datas
        if(noData || (m_fifo->count() >=size2)) {
            break;
        } else {
	    QMutex m;
	    m.lock();
            wDataReady.wait(&m);
	    m.unlock();
	}
    }
    // if there's datas in input buffer and we did not get all what we wanted, we take them.
    s= (m_fifo->count()) <size2 ? (m_fifo->count()) : size2;
    readSize+=s;
    if (s>0 ) 
        m_fifo->dequeue(data,s);
   
    wDataRead.wakeAll();
    return readSize;
}

void k9vamps::addSubpicture(uint id) {
    int cpt=1;
    for (uint i=0;i<32;i++)
	if (spu_track_map[i]!=0) cpt++;
    spu_track_map[id-1]=cpt;
}


void k9vamps::addAudio(uint id) {
    int cpt=1;
    for (uint i=0;i <8;i++)
        if (audio_track_map[i] !=0) cpt++;

    audio_track_map[id-1]=cpt;
}

void k9vamps::addAudio(uint id,uint newId) {
    if (newId==0) 
        addAudio(id);
    else 
        audio_track_map[id-1]=newId;
}


void k9vamps::setInputSize(uint64_t size) {
    ps_size=size;
}

void k9vamps::setVapFactor(float factor) {
    vap_fact=factor;
}

void k9vamps::setSaveImage(k9SaveImage *m_save) {
    m_saveImage=m_save;
}

void k9vamps::reset() {
    m_preserve=true;
    m_error=false;
    bytes_read =0;
    bytes_written=0;
    padding_bytes=0;
    total_packs=0;
    video_packs=0;
    skipped_video_packs=0;
    aux_packs=0;
    skipped_aux_packs=0;
    sequence_headers=0;
    nav_packs=0;

    rptr = rbuf;
    rhwp = rbuf;
    wptr =  wbuf;
    vbuf_size = VBUF_SIZE;
    vap_fact= 1.0f;

    //	inbuffw=inbuff;
    for (uint i=0; i<8;i++) {
        audio_track_map[i]=0;
    }
    for (uint i=0; i<32;i++) {
        spu_track_map[i]=0;
    }

    calc_ps_vap = 1;
    vap_fact=1.0;
    ps_size=0;
    noData=false;

    avgdiff=1;
    m_totfact=m_nbfact=m_avgfact=0;

    vin_bytes=0;
    vout_bytes=0;

}

k9vamps::k9vamps(k9DVDBackupInterface *dvdbackup,bool _useCache):QThread(dvdbackup) {
if (!_useCache)
    m_fifo=new k9MemoryFifo();
else
    m_fifo=new k9FileFifo();

    m_saveImage=NULL;
    m_dvdbackup=dvdbackup;
    reset();
    m_requant=NULL;
    if (dvdbackup !=NULL) {
    	m_bgUpdate = new k9bgUpdate(dvdbackup);
        dvdbackup->setVamps(this);
    } else
	m_bgUpdate=NULL;
    rbuf_size= RBUF_SIZE;
    rbuf = new uchar[rbuf_size];
    m_output=NULL;
}


void k9vamps::setPreserve(bool _value) {
    m_preserve = _value;
}
void k9vamps::setOutput(QFile *_output) {
    m_output=_output;
} 

k9vamps::~k9vamps() {
    if (m_bgUpdate !=NULL)
	delete m_bgUpdate;
    delete[] rbuf;
    delete m_fifo;
}


void k9vamps::run () {
    m_error=false;
   m_errMsg="";
    m_requant=new k9requant();
    eof=0;

    // allocate video buffers
    vibuf = new uchar[vbuf_size];
    vobuf = new uchar[vbuf_size];

    if (vibuf == NULL || vobuf == NULL)
        fatal (QString("Allocation of video buffers failed: %1").arg(strerror (errno)));


    // actually do vaporization
    vaporize ();


    flush();

    if (m_requant !=NULL) {	
        m_requant->rqt_stop=true;
	while(m_requant->isRunning()) {
	    m_requant->condr.wakeAll();
	    m_requant->condw.wakeAll();
	    m_requant->wait(10);
	}
//        m_requant->mutr.unlock();
//        m_requant->mutw.unlock();
    }
    delete m_requant;
    m_requant=NULL;
    delete[] vibuf;
    delete[] vobuf;
    if (m_bgUpdate!=NULL)
	m_bgUpdate->wait();
    //mutex.unlock();
}



// lock `size' bytes in read buffer
// i.e. ensure the next `size' input bytes are available in buffer
// returns nonzero on EOF
int k9vamps::lock (int size) {
    int avail, n;

    avail = rhwp - rptr;

    if (avail >= size)
        return 0;

    if (avail) {
        memcpy (rbuf, rptr, avail);
        rptr = rbuf;
        rhwp = rptr + avail;
    }

    if (rbuf_size -avail <=0) {
	uchar *buffer =new uchar[rbuf_size+20480];
	memcpy (buffer,rbuf,rbuf_size);
	rptr = buffer +(rptr-rbuf);
	rhwp=buffer+(rhwp-rbuf);
	rbuf_size+=20480;
	delete[] rbuf;
	rbuf=buffer;
   }

    n = readData(rhwp,rbuf_size - avail);

    if (n % SECT_SIZE)
        fatal ("Premature EOF");

    rhwp       += n;
    bytes_read += n;

    return !n;
}


// copy `size' bytes from rbuf to wbuf
void k9vamps::copy (int size) {
    if (!size)
        return;

    if ((wptr - wbuf) + size > WBUF_SIZE)
        fatal ("Write buffer overflow");

    memcpy (wptr, rptr, size);
    rptr += size;
    wptr += size;
}


// skip `size' bytes in rbuf
void k9vamps::skip (int size) {
    rptr += size;
}


// flush wbuf
void k9vamps::flush (void) {
    int size;
    mutex.lock();
    size = wptr - wbuf;

    if (!size) {
        mutex.unlock();
        return;
    }
    //m_dvdbackup->getOutput(wbuf,size);
    // wait for a preceding update to finish
    if (m_bgUpdate!=NULL) {
	m_bgUpdate->wait();
	m_bgUpdate->update( wbuf,size);
    } 
    if (m_output != NULL)
	m_output->write((const char*) wbuf,size);
    if (m_saveImage !=NULL) 
        m_saveImage->addData(wbuf,size);
    wptr           = wbuf;
    bytes_written += size;
    mutex.unlock();
}


// returns no. bytes read up to where `ptr' points
uint64_t k9vamps::rtell (uchar *ptr) {
    return bytes_read - (rhwp - ptr);
}


// returns no. bytes written up to where `ptr' points
// (including those in buffer which are not actually written yet)
uint64_t k9vamps::wtell (uchar *ptr) {
    return bytes_written + (ptr - wbuf);
}


// some pack header consistency checking
bool k9vamps::check_pack (uchar *ptr) {
    uint32_t pack_start_code;
    int    pack_stuffing_length;

    pack_start_code  = (uint32_t) (ptr [0]) << 24;
    pack_start_code |= (uint32_t) (ptr [1]) << 16;
    pack_start_code |= (uint32_t) (ptr [2]) <<  8;
    pack_start_code |= (uint32_t) (ptr [3]);

    if (pack_start_code != 0x000001ba) {
//        fatal ("Bad pack start code at %llu: %08lx", rtell (ptr), pack_start_code);
	return false;
    }

    if ((ptr [4] & 0xc0) != 0x40) {
     //   fatal ("Not an MPEG2 program stream pack at %llu", rtell (ptr));
	return false;
    }

    // we rely on a fixed pack header size of 14
    // so better to ensure this is true
    pack_stuffing_length = ptr [13] & 7;

    if (pack_stuffing_length) {
        //fatal ("Non-zero pack stuffing length at %llu: %d\n",   rtell (ptr), pack_stuffing_length);
       return false;
    }

    return true;
}


// video packet consistency checking
int k9vamps::check_video_packet (uchar *ptr) {
    int    vid_packet_length, pad_packet_length, rc = 0;
    uint32_t vid_packet_start_code, pad_packet_start_code, sequence_header_code;

    vid_packet_start_code  = (uint32_t) (ptr [0]) << 24;
    vid_packet_start_code |= (uint32_t) (ptr [1]) << 16;
    vid_packet_start_code |= (uint32_t) (ptr [2]) <<  8;
    vid_packet_start_code |= (uint32_t) (ptr [3]);

    if (vid_packet_start_code != 0x000001e0)
        fatal(QString ("Bad video packet start code at %1: %2").arg(rtell(ptr)).arg(vid_packet_start_code,0,16));

    vid_packet_length  = ptr [4] << 8;
    vid_packet_length |= ptr [5];
    vid_packet_length += 6;

    if ((ptr [6] & 0xc0) != 0x80)
        fatal (QString("Not an MPEG2 video packet at %1").arg(rtell (ptr)));

    if (ptr [7]) {
        if ((ptr [7] & 0xc0) != 0xc0)
           qDebug() << QString("First video packet in sequence starting at %1 misses PTS or DTS, flags=%2").arg(rtell (ptr)).arg(ptr [7]);
        else {
            sequence_header_code  = (uint32_t) (ptr [6 + 3 + ptr [8] + 0]) << 24;
            sequence_header_code |= (uint32_t) (ptr [6 + 3 + ptr [8] + 1]) << 16;
            sequence_header_code |= (uint32_t) (ptr [6 + 3 + ptr [8] + 2]) <<  8;
            sequence_header_code |= (uint32_t) (ptr [6 + 3 + ptr [8] + 3]);
    
            if (sequence_header_code == 0x000001b3) {
                rc = 1;
            } else {
                //fprintf (stderr, "Start of GOP at %llu not on sector boundary\n",
                //         rtell (ptr + 6 + 3 + ptr [8]));
                sequence_headers++;
            }
        }
        
    }

    pad_packet_length = 0;

    if (14 + vid_packet_length < SECT_SIZE - 6) {
        // video packet does not fill whole sector
        // check for padding packet
        ptr += vid_packet_length;

        pad_packet_start_code  = (uint32_t) (ptr [0]) << 24;
        pad_packet_start_code |= (uint32_t) (ptr [1]) << 16;
        pad_packet_start_code |= (uint32_t) (ptr [2]) <<  8;
        pad_packet_start_code |= (uint32_t) (ptr [3]);

        if (pad_packet_start_code != 0x000001be)
            qDebug () <<QString("Bad padding packet start code at %1: %2").arg(rtell (ptr + vid_packet_length)).arg(pad_packet_start_code);
        else {
            pad_packet_length  = ptr [4] << 8;
            pad_packet_length |= ptr [5];
            pad_packet_length += 6;
        }
    }

    // length of video packet plus padding packet must always match sector size
    if (14 + vid_packet_length + pad_packet_length != SECT_SIZE)
        qDebug () << QString("Bad video packet length at %1: %2").arg(rtell (ptr)).arg(vid_packet_length);

    return rc;
}


// here we go
// this is where we switch to the requantization thread
// note that this and the requant thread never run concurrently (apart
// from a very short time) so a dual CPU box does not give an advantage
// returns size of evaporated GOP
int k9vamps::requant (uchar *dst, uchar *src, int n, float fact) {
    if (n==0) return 0;
    int rv;
    if (! m_requant->isRunning()) {
        m_requant->initvar();
    }
    m_requant->rqt_stop=false;
    // this ensures for the requant thread to stop at this GOP's end
    memcpy (src + n, "\0\0\1", 3);

    m_requant->mutr.lock();

    m_requant->rqt_rptr     = src;
    m_requant->rqt_wptr     = dst;
    m_requant->rqt_rcnt     = n;
    m_requant->rqt_wcnt     = 0;
    m_requant->rqt_fact     =  fact  ;
    m_requant->rqt_inbytes  = vin_bytes;
    m_requant->rqt_outbytes = vout_bytes;
    m_requant->rqt_visize   = (uint64_t) ((float) ps_size * (float) vin_bytes / ((float) total_packs * (float) SECT_SIZE));

    // create requantization thread
    if (! m_requant->isRunning()) {
        m_requant->start();
        m_requant->rqt_run=true;
    }

    m_requant->condr.wakeAll();
    m_requant->mutr.unlock();

    // now the requant thread should be running

    m_requant->mutw.lock();

    // wait for requant thread to finish
    while (!m_requant->rqt_wcnt)
        m_requant->condw.wait( &m_requant->mutw);

    rv = m_requant->rqt_wcnt;

    m_requant->mutw.unlock();
/*    if ((m_requant->rbuf-m_requant->cbuf -3) >0 )  {
        memcpy(dst+m_requant->rqt_wcnt,m_requant->cbuf,m_requant->rbuf-m_requant->cbuf -3);
        rv +=m_requant->rbuf-m_requant->cbuf -3;
    }
/*/
   if ((m_requant->rbuf-m_requant->cbuf -2) >0 )  {
        memcpy(dst+m_requant->rqt_wcnt,m_requant->cbuf,m_requant->rbuf-m_requant->cbuf -2);
        rv +=m_requant->rbuf-m_requant->cbuf -2;
    }



//    if (rv>n)
//        qDebug("requant error");

    double realrqtfact=(double)(vin_bytes) / (double)(vout_bytes+rv);
    avgdiff = ((m_avgfact) /realrqtfact);

    //qDebug ("factor : " +QString::number(m_avgfact) +"  --> " +QString::number((float)n/(float)rv) +" avgdiff : " + QString::number(avgdiff) +" rqt_visize :" +QString::number(m_requant->rqt_visize) +" ps_size :" +QString::number(ps_size) + " vin_bytes :" + QString::number(vin_bytes)) ;

    return rv;

}


int k9vamps::getStreamIndex(uchar *ptr) {
    int type, abase;

    type = ptr [6 + 3 + ptr [8]];
    //fprintf (stderr, "type=%02x\n", type);

    if (type >= 0x20 && type <= 0x3f) {
        // subpicture
        return type- 0x20 +1;
    }

    if (type >= 0x80 && type <= 0x87) {
        // AC3 audio
        abase = 0x80;
    } else if (type >= 0x88 && type <= 0x8f) {
        // DTS audio
        abase = 0x88;
    } else if (type >= 0xa0 && type <= 0xa7) {
        // LPCM audio
        abase = 0xa0;
    } else {
//        fatal ("Unknown private stream 1 type at %llu: %02x", rtell (ptr), type);
        abase = 0;
    }
	
    return type - abase + 1;

}

// translate type of private stream 1 packet
// according to the track translation maps
// returns new track type (e.g. 0x80 for first AC3 audio
// track in cmd line) or zero if track is not to be copied
int k9vamps::new_private_1_type (uchar *ptr) {
    int type, track, abase;

    type = ptr [6 + 3 + ptr [8]];
    //fprintf (stderr, "type=%02x\n", type);

    if (type >= 0x20 && type <= 0x3f) {
        // subpicture
        
        track = spu_track_map [type - 0x20];

        return track ? track - 1 + 0x20 : 0;
    }

    if (type >= 0x80 && type <= 0x87) {
        // AC3 audio
        abase = 0x80;
    } else if (type >= 0x88 && type <= 0x8f) {
        // DTS audio
        abase = 0x88;
    } else if (type >= 0xa0 && type <= 0xa7) {
        // LPCM audio
        abase = 0xa0;
    } else {
//        fatal ("Unknown private stream 1 type at %llu: %02x", rtell (ptr), type);
        abase = 0;
    }
	
    track = audio_track_map [type - abase];

    return track ? track - 1 + abase : 0;
}


// selectivly copy private stream 1 packs
// patches track type to reflect new track
// mapping unless user opted to preserve them
void k9vamps::copy_private_1 (uchar *ptr) {
    int type;

    type = new_private_1_type (ptr);

    if (type) {
        if (m_dvdbackup) {
   	    int streamIndex=getStreamIndex(ptr);
	    if (streamIndex) {
		m_dvdbackup->getOutput( (type >= 0x20 && type <= 0x3f) ? k9DVDBackupInterface::SUBPICTURE:k9DVDBackupInterface::AUDIO,streamIndex,ptr,SECT_SIZE);
	    }
	}
 	
        if (!m_preserve)
            ptr [6 + 3 + ptr [8]] = type;

        copy (SECT_SIZE);

        return;
    }

    skip (SECT_SIZE);
}


// translate ID of MPEG audio packet
// according to the audio track translation map
// returns new ID (e.g. 0xc0 for first MPEG audio
// track in cmd line) or zero if track is not to be copied
int k9vamps::new_mpeg_audio_id (int id) {
    int track;

    track = audio_track_map [id - 0xc0];

    return track ? track - 1 + 0xc0 : 0;
}


// selectivly copy MPEG audio packs
// patches ID to reflect new track mapping unless user opted to preserve them
void k9vamps::copy_mpeg_audio (uchar *ptr) {
    int id;

    id = new_mpeg_audio_id (ptr [3]);

    if (id) {
        if (!m_preserve)
            ptr [3] = id;

        copy (SECT_SIZE);

        return;
    }

    skip (SECT_SIZE);
}


// process beginning of program stream up to
// - but not including - first sequence header
// this PS leader is NOT shrunk since the PS may not
// necessarily begin at a GOP boundary (although it should?)
// nevertheless the unwanted private stream 1 and MPEG audio
// packs are skipped since some players could get confused otherwise
void k9vamps::vap_leader () {
    uchar *ptr;
    int    id, data_length;

    while (!lock (SECT_SIZE)) {
        ptr = rptr;
        if (check_pack (ptr)) {
	        ptr += 14;
        	id   = ptr [3];
	} else {
		ptr +=14;
	        id   = 0;
	}

        switch (id) {
        case 0xe0:
            // video
            if (check_video_packet (ptr))
                // sequence header
                return;

            copy (SECT_SIZE);
	    if (m_dvdbackup)
	       m_dvdbackup->getOutput(k9DVDBackupInterface::VIDEO,0,ptr,SECT_SIZE);

            break;

        case 0xbd:
            // private 1: audio/subpicture
            copy_private_1 (ptr);
            break;

        case 0xc0:
        case 0xc1:
        case 0xc2:
        case 0xc3:
        case 0xc4:
        case 0xc5:
        case 0xc6:
        case 0xc7:
            // MPEG audio
            copy_mpeg_audio (ptr);
            break;

        case 0xbb:
            // system header/private 2: PCI/DSI
            copy (SECT_SIZE);
	    if (m_dvdbackup)
                m_dvdbackup->getOutput(k9DVDBackupInterface::NAV,0,ptr,SECT_SIZE);
            break;

        case 0xbe:
            // padding
            data_length  = ptr [4] << 8;
            data_length |= ptr [5];

            if (14 + data_length != SECT_SIZE - 6)
                fatal (QString("Bad padding packet length at %1: %2").arg(rtell (ptr)).arg(data_length));
  	    //JMP:à vérifier 
  	    skip (SECT_SIZE);

            break;
	   
        default:
	 //   fatal("Encountered stream ID %02x at %llu, "
         //          "probably bad MPEG2 program stream", id, rtell (ptr));
		copy (SECT_SIZE);
        }

        if (wptr == wbuf + WBUF_SIZE)
            flush ();
    }

    eof = 1;
    flush ();

    return;
}


// process end of program stream
// the same counts here as for the PS' beginning
void k9vamps::vap_trailer (int length) {
    uchar *ptr;
    int    i, id, data_length;

    for (i = 0; i < length; i += SECT_SIZE) {
        ptr = rptr + 14;
        id  = ptr [3];

        if (id == 0xbd) {
            // private 1: audio/subpicture
            copy_private_1 (ptr);
        } else if (id >= 0xc0 && id <= 0xc7) {
            // MPEG audio
            copy_mpeg_audio (ptr);
        } else if (id == 0xbe) {
            // padding
            data_length  = ptr [4] << 8;
            data_length |= ptr [5];

            if (14 + data_length != SECT_SIZE - 6)
                fatal (QString("Bad padding packet length at %1: %2").arg(rtell (ptr)).arg(data_length));
	    skip (SECT_SIZE);
        } else {
            copy (SECT_SIZE);
        }

        if (wptr == wbuf + WBUF_SIZE)
            flush ();
    }

    flush ();
}


// vaporization is split in two phases - this is phase 1
// PS packs are read into rbuf until a sequence header is found.
// All video packs are unpacketized and the contained video ES
// GOP copied to vibuf. In the same course the private stream 1
// and MPEG audio packs are inspected and the number of packs
// not to be copied are counted. This is to forecast the video
// vaporization factor in case the user specified a PS shrink factor.
// returns GOP length in bytes
int k9vamps::vap_phase1 (void) {
    uchar *ptr, *viptr = vibuf;
    int    seq_length, id, data_length, opt_length, seqhdr;

    for (seq_length = 0;
            !lock (seq_length + SECT_SIZE); seq_length += SECT_SIZE) {
        ptr = rptr + seq_length;
        if (check_pack (ptr)) {
	        ptr += 14;
        	id   = ptr [3];
	} else {
		ptr += 14;
		id   = 0;
	}
	

        // avoid duplicate counts for sequence headers
        if (seq_length)
            total_packs++;

        switch (id) {
        case 0xe0:
            // video
            seqhdr = check_video_packet (ptr);

            if (seq_length) {
                video_packs++;

                if (seqhdr) {
                    sequence_headers++;
                    vilen = viptr - vibuf;

                    return seq_length;
                }
            }

            // copy contained video ES fragment to vibuf
            data_length  = ptr [4] << 8;
            data_length |= ptr [5];
            opt_length   = 3 + ptr [8];
            data_length -= opt_length;

            if ((viptr - vibuf) + data_length > vbuf_size - 3) {
                // reallocate video buffers
                int i = viptr - vibuf;

                // grow by another VBUF_SIZE bytes
                vbuf_size += VBUF_SIZE;
		uchar *tmp;
		tmp =new uchar[vbuf_size];
		memcpy(tmp,vibuf,vbuf_size-VBUF_SIZE);
		delete[] vibuf;
		vibuf = tmp;

//                vibuf      = (uchar*)realloc (vibuf, vbuf_size);
		tmp=new uchar[vbuf_size];
		memcpy(tmp,vobuf,vbuf_size-VBUF_SIZE);
		delete[] vobuf;
		vobuf=tmp;

//                vobuf      = (uchar*)realloc (vobuf, vbuf_size);

                if (vibuf == NULL || vobuf == NULL)
                    fatal ("Reallocation of video buffers failed");

                viptr = vibuf + i;
            }

            //fprintf (stderr, "data_length=%d\n", data_length);
            memcpy (viptr, ptr + 6 + opt_length, data_length);
            viptr += data_length;
            break;

        case 0xbd:
            // private 1: audio/subpicture
            aux_packs++;

            if (!new_private_1_type (ptr))
                skipped_aux_packs++;

            break;

        case 0xc0:
        case 0xc1:
        case 0xc2:
        case 0xc3:
        case 0xc4:
        case 0xc5:
        case 0xc6:
        case 0xc7:
            // MPEG audio
            aux_packs++;

            if (!new_mpeg_audio_id (id))
                skipped_aux_packs++;

            break;

        case 0xbb:
            // system header/private 2: PCI/DSI
            nav_packs++;
            break;

        case 0xbe:
            // padding
            skipped_aux_packs++;
            data_length  = ptr [4] << 8;
            data_length |= ptr [5];

            if (14 + data_length != SECT_SIZE - 6)
                fatal (QString("Bad padding packet length at %1: %2").arg(rtell (ptr)).arg(data_length));

            break;

        default:
//	    fatal("Encountered stream ID %02x at %llu, "
//                   "probably bad MPEG2 program stream", id, rtell (ptr));	
		break;
        }

    }

    eof = 1;

    return seq_length;
}


// re-packetize the video ES
// `ptr' points to original PES packet where to put the video data
// `voptr' points to first unpacketized byte in vobuf
// `avail' specifies number of bytes remaining in vobuf
// returns number of ES bytes in generated PES packet
int k9vamps::gen_video_packet (uchar *ptr, uchar *voptr, int avail) {
    int i, header_data_length, data_length, padding_length;

    // if original PES holds optional data (e.g. DTS/PTS) we must keep it
    header_data_length = (ptr [7] & 0xc0) == 0xc0 ? ptr [8] : 0;
    data_length        = SECT_SIZE - (14 + 6 + 3 + header_data_length);

    if (avail >= data_length) {
        // write out a full video packet (usually 2025 byte)
        memcpy (ptr + 6 + 3 + header_data_length, voptr, data_length);
        ptr [4] = (SECT_SIZE - (14 + 6)) >> 8;
        ptr [5] = (SECT_SIZE - (14 + 6)) & 0xff;
        ptr [8] = header_data_length;

        return data_length;
    }

    if (avail < data_length - 6) {
        // write a short video packet and a padding packet
        memcpy (ptr + 6 + 3 + header_data_length, voptr, avail);
        ptr [4] = (3 + header_data_length + avail) >> 8;
        ptr [5] =  3 + header_data_length + avail;
        ptr [8] = header_data_length;

        // generate padding packet
        ptr           += 6 + 3 + header_data_length + avail;
        padding_length = data_length - (avail + 6);
        padding_bytes += padding_length + 6;
        ptr [0]        = 0;
        ptr [1]        = 0;
        ptr [2]        = 1;
        ptr [3]        = 0xbe;
        ptr [4]        = padding_length >> 8;
        ptr [5]        = padding_length;

        for (i = 0; i < padding_length; i++)
            ptr [6+i] = 0xff;

        return avail;
    }

    // write a padded video packet (1 to 6 padding bytes)
    padding_length = data_length - avail;
    padding_bytes += padding_length;
    memset (ptr + 6 + 3 + header_data_length, 0xff, padding_length);
    header_data_length += padding_length;
    memcpy (ptr + 6 + 3 + header_data_length, voptr, avail);
    ptr [4] = (SECT_SIZE - (14 + 6)) >> 8;
    ptr [5] = (SECT_SIZE - (14 + 6)) & 0xff;
    ptr [8] = header_data_length;

    return avail;
}


// this is phase 2 of vaporization
// the shrunk video ES is re-packetized by using the source PES packets
// unused PS packs are skipped
// only wanted private stream 1 and MPEG audio packs are copied
// all nav packs are copied
void k9vamps::vap_phase2 (int seq_length) {
    int    i, id, avail, data_length;
    uchar *ptr, *voptr = vobuf, *vohwp = vobuf + volen;

    for (i = 0; i < seq_length; i += SECT_SIZE) {
        ptr = rptr + 14;
        id  = ptr [3];

        switch (id) {
        case 0xe0:
            // video
            avail = vohwp - voptr;

            if (avail) {
                // still some video output data left
                voptr += gen_video_packet (ptr, voptr, avail);
                copy (SECT_SIZE);
		if (m_dvdbackup)
		    m_dvdbackup->getOutput(k9DVDBackupInterface::VIDEO,0,ptr,SECT_SIZE);

            } else {
                // no video output data left - skip input sector
                skip (SECT_SIZE);
                skipped_video_packs++;
            }

            break;

        case 0xbd:
            // private 1: audio/subpicture
            copy_private_1 (ptr);
            break;

        case 0xc0:
        case 0xc1:
        case 0xc2:
        case 0xc3:
        case 0xc4:
        case 0xc5:
        case 0xc6:
        case 0xc7:
            // MPEG audio
            copy_mpeg_audio (ptr);
            break;

        case 0xbb:
            // system header/private 2: PCI/DSI
            copy (SECT_SIZE);   	    
	    if (m_dvdbackup)
   	       m_dvdbackup->getOutput(k9DVDBackupInterface::NAV,0,ptr,SECT_SIZE);

            break;


        case 0xbe:
            // padding
            data_length  = ptr [4] << 8;
            data_length |= ptr [5];

            if (14 + data_length != SECT_SIZE - 6)
                fatal (QString("Bad padding packet length at %1: %2").arg(rtell (ptr)).arg(data_length));
	    //JMP: à vérifier
  	    skip (SECT_SIZE);
            break;

        default:
            copy (SECT_SIZE);
//	    fatal("Encountered stream ID %02x at %llu, "
//                   "probably bad MPEG2 program stream", id, rtell (ptr));
        }

        if (wptr == wbuf + WBUF_SIZE)
            // end of write buffer reached --> flush it to disk
            flush ();
    }
}

QString & k9vamps::geterrMsg() {
    return m_errMsg;
}

bool k9vamps::geterror() {
    return m_error;
}

// entry point from main()
// the requant thread already has been started
void k9vamps::vaporize (void) {
    int   seq_length;
    float fact = vap_fact;

    // process PS up to but not including first sequence header
    vap_leader ();

    // just in case - maybe should spit out a warning/error here
    if (eof)
        return;

    total_packs++;
    nav_packs++;
    total_packs++;
    video_packs++;

    // main loop
    while (1) {
        // do phase 1 of vaporization
        seq_length = vap_phase1 ();

        if (eof) {
            // EOF on source PS
            // process packs after and including last sequence header
            vap_trailer (seq_length);

            // only exit point from main loop
            return;
        }

        //fprintf (stderr, "seq_length=%d\n", seq_length);

        if (calc_ps_vap && vap_fact > 1.0f) {
            // forecast video ES vaporization factor
            // the basic formulars look like:
            // vap_fact  = total_packs/(restpacks+vop)
            // restpacks = total_packs-(video_packs+skipped_aux_packs)
            // fact      = (video_packs*net-(gops*net/2+10))/(vop*net-(gops*net/2+10))
            // net       = SECT_SIZE-(14+9)
            // 14: pack header size
            // 9:  PES header sizerequant
            // 10: PTS+DTS size in PES header of sequence header
            // You are welcome to double check everything here!
            float vop, net;
            net  = (float) (SECT_SIZE - (14+9));
            vop  = video_packs + skipped_aux_packs -
                   (float) total_packs * (1.0f-1.0f/vap_fact);
            fact = ((float) video_packs * net -
                    ((float) sequence_headers * net/2.0f + 10.0f)) /
                   (vop * net - ((float) sequence_headers * net/2.0f + 10.0f));

            //JMP
            m_totfact+=fact ;
            m_nbfact++;
            m_avgfact=m_totfact/m_nbfact;

            // requant seems to get stuck on factors < 1
            if (fact < 1.0f)
                fact = 1.0f;

            if (verbose >= 2)
                fprintf (stderr, "Info: Target video ES vaporization factor: %.3f\n",
                         fact);
        }

        vin_bytes += vilen;

        if (fact > 1.0f) {
            // do requantization
            volen = requant (vobuf, vibuf, vilen, fact);
        } else {
            // don't do requantization
            memcpy (vobuf, vibuf, vilen);
            volen = vilen;
        }

        vout_bytes += volen;

        // do phase 2 of vaporization
        vap_phase2 (seq_length);

        //fprintf (stderr,
        //	       "tot=%d, vid=%d, ps1=%d, nav=%d, sv=%d, sp1=%d, fact=%.3f\n",
        //	       total_packs, video_packs, aux_packs, nav_packs,
        //	       skipped_video_packs, skipped_aux_packs, fact);
    }
}

uint64_t k9vamps::getOutputBytes() {
    return bytes_written;
}

uint64_t k9vamps::getSkippedBytes() {
    return skipped_aux_packs+skipped_video_packs;
}


void k9vamps::abort() {
    //fatal("vamps stopped");
    setNoData();
    if (m_requant !=NULL)
        m_requant->wait();
    if (m_bgUpdate!=NULL)
        m_bgUpdate->wait();
}

// this is a *very* sophisticated kind of error handling :-)
void
k9vamps::fatal (QString msg) {
    m_errMsg=msg;
    m_error=true;
    if (m_requant !=NULL)
        m_requant->terminate();
    if (m_bgUpdate !=NULL)
        m_bgUpdate->terminate();
    terminate();
}

/****************************  BACKGROUND UPDATE **********************/

k9bgUpdate::k9bgUpdate(k9DVDBackupInterface * _backup) {
    m_backup = _backup;

}

void k9bgUpdate::update(uchar *_buffer,uint32_t _size) {
   //à vérifier: ajout d'un wait pour s'assurer que le thread n'est pas en cours d'exécution
    wait();

    mutex.lock();
    m_buffer=new uchar[_size];
    memcpy(m_buffer,_buffer,_size);
    m_size=_size;
    start();
    mutex.unlock();
}
void k9bgUpdate::run() {
    m_backup->getOutput(m_buffer,m_size);
    delete[] m_buffer;
}
