//
// C++ Implementation: k9subtitle2pgm
//
// Description: Extractiion of subtitles from an mp2 stream. Subtitles are extracted to vobsub files.
//              This code is based on extract_ac3 (transcode) and vobsub (mplayer)
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9subtitle2pgm.h"
#include "k9common.h"
#include <QApplication>
#include "k9ifo2.h"


k9Subtitle2Pgm::k9Subtitle2Pgm ( QObject *parent):QThread(parent) {
    m_fifo = new k9MemoryFifo();
    start_rpts=end_rpts=-1.;
    m_totalLength=0.;
}

k9Subtitle2Pgm::~k9Subtitle2Pgm() {
    delete m_fifo;
}



void k9Subtitle2Pgm::addData(uchar *data,uint size) {
    if (!isRunning()) start();
    m_fifo->addData(data,size);
}




uint k9Subtitle2Pgm::stream_read_int16(uchar *s) {
    uint a, b, result;

    a = s[0];
    b = s[1];

    result = (a << 8) | b;
    return result;
}

int k9Subtitle2Pgm::get_pts_dts(char *buffer, ulong *pts, ulong *dts) {
    uint pes_header_bytes = 0;
    uint pts_dts_flags;
    int pes_header_data_length;

    int has_pts_dts=0;

    uint ptr=0;

    /* drop first 8 bits */
    ++ptr;
    pts_dts_flags = (buffer[ptr++] >> 6) & 0x3;
    pes_header_data_length = buffer[ptr++];

    switch (pts_dts_flags)

    {

    case 2:

        *pts = (buffer[ptr++] >> 1) & 7;  //low 4 bits (7==1111)
        *pts <<= 15;
        *pts |= (stream_read_int16((uchar*)&buffer[ptr]) >> 1);
        ptr+=2;
        *pts <<= 15;
        *pts |= (stream_read_int16((uchar*)&buffer[ptr]) >> 1);
        ptr+=2;

        pes_header_bytes += 5;

        has_pts_dts=1;

        break;

    case 3:

        *pts = (buffer[ptr++] >> 1) & 7;  //low 4 bits (7==1111)
        *pts <<= 15;
        *pts |= (stream_read_int16((uchar*)&buffer[ptr]) >> 1);
        ptr+=2;
        *pts <<= 15;
        *pts |= (stream_read_int16((uchar*)&buffer[ptr]) >> 1);
        ptr+=2;

        *dts = (buffer[ptr++] >> 1) & 7;
        *dts <<= 15;
        *dts |= (stream_read_int16((uchar*)&buffer[ptr]) >> 1);
        ptr+=2;
        *dts <<= 15;
        *dts |= (stream_read_int16((uchar*)&buffer[ptr]) >> 1);
        ptr+=2;

        pes_header_bytes += 10;

        has_pts_dts=1;

        break;

    default:

        has_pts_dts=0;
        *dts=*pts=0;
        break;
    }

    return(has_pts_dts);
}


uint k9Subtitle2Pgm::stream_read_int32(uchar *s) {
    uint a, b, c, d, result;

    a = s[0];
    b = s[1];
    c = s[2];
    d = s[3];

    result = (a << 24) | (b << 16) | (c << 8) | d;
    return result;
}

uint k9Subtitle2Pgm::read_tc_time_stamp(char *s) {

    ulong i, j;
    ulong clock_ref=0, clock_ref_ext=0;

    if (s[0] & 0x40) {

        i = stream_read_int32((uchar*)s);
        j = stream_read_int16((uchar*)s+4);

        if (i & 0x40000000 || (i >> 28) == 2) {
            clock_ref  = ((i & 0x31000000) << 3);
            clock_ref |= ((i & 0x03fff800) << 4);
            clock_ref |= ((i & 0x000003ff) << 5);
            clock_ref |= ((j & 0xf800) >> 11);
            clock_ref_ext = (j >> 1) & 0x1ff;
        }
    }

    return ((uint) (clock_ref * 300 + clock_ref_ext));
}





#define BUFFER_SIZE 40960

void k9Subtitle2Pgm::pes_ac3_loop (void) {
    static int mpeg1_skip_table[16] = {
        1, 0xffff,      5,     10, 0xffff, 0xffff, 0xffff, 0xffff,
        0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
    };
    last_pts=0;
    last_pts_set = 0;

    uint8_t * buf;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t * end;
    uint8_t * tmp1=NULL;
    uint8_t * tmp2=NULL;

    char pack_buf[16];

    uint pack_lpts=0;
    double pack_sub_rpts=0.0f, abs_sub_rpts=0.0f;

    ulong i_pts, i_dts;

    buf = buffer;
    int track_code=m_trackCod;


    uint index=0;
    void *vobsub_h=vobsub_out_open(m_baseName.toUtf8(), m_palette,720,576,m_langCod.toAscii(), index);



    do {
        end = buf + m_fifo->readData(buffer,BUFFER_SIZE);
        buf = buffer;

        //scan buffer
        while (buf + 4 <= end) {


            // check for valid start code
            if (buf[0] || buf[1] || (buf[2] != 0x01)) {
                buf++;
                continue;
            }// check for valid start code


            switch (buf[3]) {

            case 0xb9:	/* program end code */
                return;

                //check for PTS


            case 0xba:	/* pack header */

                memcpy(pack_buf, &buf[4], 6);
                pack_lpts = read_tc_time_stamp(pack_buf);

                /* skip */
                if ((buf[4] & 0xc0) == 0x40)	        /* mpeg2 */
                    tmp1 = buf + 14 + (buf[13] & 7);
                else if ((buf[4] & 0xf0) == 0x20)	/* mpeg1 */
                    tmp1 = buf + 12;
                else if (buf + 5 > end)
                    goto copy;
                else {
                    fprintf (stderr, "(%s) weird pack header\n", __FILE__);
                    return;
                }

                if (tmp1 > end)
                    goto copy;
                buf = tmp1;
                break;


            case 0xbd:	/* private stream 1 */
                tmp2 = buf + 6 + (buf[4] << 8) + buf[5];
                if (tmp2 > end)
                    goto copy;
                if ((buf[6] & 0xc0) == 0x80)	/* mpeg2 */
                    tmp1 = buf + 9 + buf[8];
                else {	/* mpeg1 */
                    for (tmp1 = buf + 6; *tmp1 == 0xff; tmp1++)
                        if (tmp1 == buf + 6 + 16) {
                            fprintf (stderr, "(%s) too much stuffing\n", __FILE__);
                            buf = tmp2;
                            break;
                        }
                    if ((*tmp1 & 0xc0) == 0x40)
                        tmp1 += 2;
                    tmp1 += mpeg1_skip_table [*tmp1 >> 4];
                }


                //subtitle

                if ((*tmp1 == track_code && track_code < 0x40 )) {

                    if (tmp1 < tmp2) {

                        // get pts time stamp:
                        memcpy(pack_buf, &buf[6], 16);

                        if (get_pts_dts(pack_buf, &i_pts, &i_dts)) {
                            pack_sub_rpts = (double) i_pts/90000.;

                            abs_sub_rpts=m_totalLength+pack_sub_rpts - start_rpts;
                        }
                        vobsub_out_output(vobsub_h, tmp1+1,tmp2-tmp1-1, abs_sub_rpts);


                    }

                    //ac3 package

                    if (*tmp1 == track_code && track_code >= 0x80) {
                        tmp1 += 4;

                        //test
                        if (0) {
                            memcpy(pack_buf, &buf[6], 16);
                            get_pts_dts(pack_buf, &i_pts, &i_dts);
                            fprintf(stderr, "AC3 PTS=%f\n", (double) i_pts/90000.);
                        }

                    }
                }

                buf = tmp2;
                break;
            case 0xbb: {
		pci_t	 pci_pack;
		k9Ifo2::navRead_PCI(&pci_pack,buf-14 + 0x2d);
//                k9DVDChapter *ch=m_title->getChapterFromSector(dsi_pack.dsi_gi.nv_pck_lbn);
//                if (ch!=m_chapter) {
		    //adds the length of previous vobu
		    if (start_rpts !=-1.) {
			m_totalLength+=end_rpts-start_rpts;
		    }
                    start_rpts=(double) pci_pack.pci_gi.vobu_s_ptm/90000.;
		    end_rpts=(double) pci_pack.pci_gi.vobu_e_ptm/90000.;
		    
//                    m_chapter=ch;
                }
            default:
                if (buf[3] < 0xb9) fprintf (stderr, "(%s) broken stream - skipping data\n", __FILE__);

                /* skip */
                tmp1 = buf + 6 + (buf[4] << 8) + buf[5];
                if (tmp1 > end)
                    goto copy;
                buf = tmp1;
                break;

            } //start code selection
        } //scan buffer

        if (buf < end) {
copy:
            /* we only pass here for mpeg1 ps streams */
            memmove (buffer, buf, end - buf);
        }
        buf = buffer + (end - buf);

    } while (end == buffer + BUFFER_SIZE);
    vobsub_out_close(vobsub_h);
}





void k9Subtitle2Pgm::run() {
    pes_ac3_loop();
}



/**********************************************************************
 * Vobsub output
 **********************************************************************/


#define MIN(a,b) a>b?b:a
#define MAX(a,b) a<b?b:a

void k9Subtitle2Pgm::create_idx(vobsub_out_t *me, const uint *palette, uint orig_width, uint orig_height) {
    int i;
    fprintf(me->fidx,
            "# VobSub index file, v7 (do not modify this line!)\n"
            "#\n"
            "# Generated by MPlayer " VERSION "\n"
            "# See <URL:http://www.mplayerhq.hu/> for more information about MPlayer\n"
            "# See <URL:http://vobsub.edensrising.com/> for more information about Vobsub\n"
            "#\n"
            "size: %ux%u\n",
            orig_width, orig_height);
    if (palette) {
        fputs("palette:", me->fidx);
        for (i = 0; i < 16; ++i) {
            const double y = palette[i] >> 16 & 0xff,
                             u = (palette[i] >> 8 & 0xff) - 128.0,
                                 v = (palette[i] & 0xff) - 128.0;
            if (i)
                putc(',', me->fidx);
            fprintf(me->fidx, " %02x%02x%02x",
                    MIN(MAX((int)(y + 1.4022 * u), 0), 0xff),
                    MIN(MAX((int)(y - 0.3456 * u - 0.7145 * v), 0), 0xff),
                    MIN(MAX((int)(y + 1.7710 * v), 0), 0xff));
        }
        putc('\n', me->fidx);
    }

    fprintf(me->fidx,"# ON: displays only forced subtitles, OFF: shows everything\n"
            "forced subs: OFF\n");
}

void *k9Subtitle2Pgm::vobsub_out_open(const char *basename, const uint *palette,
                                      uint orig_width, uint orig_height,
                                      const char *id, uint index) {
    vobsub_out_t *result = NULL;
    char *filename;
    filename = (char*)malloc(strlen(basename) + 5);
    if (filename) {
        result = (vobsub_out_t*)malloc(sizeof(vobsub_out_t));
        if (result) {
            result->aid = index;
            strcpy(filename, basename);
            strcat(filename, ".sub");
            result->fsub = fopen(filename, "ab");
            if (result->fsub == NULL)
                perror("Error: vobsub_out_open subtitle file open failed");
            strcpy(filename, basename);
            strcat(filename, ".idx");
            result->fidx = fopen(filename, "ab");
            if (result->fidx) {
                if (ftell(result->fidx) == 0) {
                    create_idx(result, palette, orig_width, orig_height);
                    /* Make the selected language the default language */
                    fprintf(result->fidx, "\n# Language index in use\nlangidx: %u\n", index);
                }
                fprintf(result->fidx, "\nid: %s, index: %u\n", id ? id : "xx", index);
                /* So that we can check the file now */
                fflush(result->fidx);
            } else
                perror("Error: vobsub_out_open index file open failed");
            free(filename);
        }
    }
    return result;
}

void k9Subtitle2Pgm::vobsub_out_close(void *me) {
    vobsub_out_t *vob = (vobsub_out_t*)me;
    if (vob->fidx)
        fclose(vob->fidx);
    if (vob->fsub)
        fclose(vob->fsub);
    free(vob);
}

void k9Subtitle2Pgm::vobsub_out_output(void *me, const uchar *packet, int len, double pts) {

    vobsub_out_t *vob = (vobsub_out_t*)me;
    if (vob->fsub) {
        /*  Windows' Vobsub require that every packet is exactly 2kB long */
        uchar buffer[2048];
        uchar *p;
        int remain = 2048;
        /* Do not output twice a line with the same timestamp, this
           breaks Windows' Vobsub */
        if (vob->fidx && (!last_pts_set || last_pts != pts)) {
            static uint last_h = 9999, last_m = 9999, last_s = 9999, last_ms = 9999;
            uint h, m, ms;
            double s;
            s = pts;
            h = s / 3600;
            s -= h * 3600;
            m = s / 60;
            s -= m * 60;
            ms = (s - (uint) s) * 1000;
            if (ms >= 1000)     /* prevent overfolws or bad float stuff */
                ms = 0;
            if (h != last_h || m != last_m || (uint) s != last_s || ms != last_ms) {
                QTime time(h,m,s,ms);
//                fprintf(vob->fidx, "timestamp: %02u:%02u:%02u:%03u, filepos: %09lx\n",
//                        h, m, (uint) s, ms, ftell(vob->fsub));
                fprintf(vob->fidx, "timestamp: %02u:%02u:%02u:%03u, filepos: %09lx\n",
                        time.hour(), time.minute(), (uint) time.second(), time.msec(), ftell(vob->fsub));
                last_h = h;
                last_m = m;
                last_s = (uint) s;
                last_ms = ms;
            }
        }
        last_pts = pts;
        last_pts_set = 1;

        /* Packet start code: Windows' Vobsub needs this */
        p = buffer;
        *p++ = 0;               /* 0x00 */
        *p++ = 0;
        *p++ = 1;
        *p++ = 0xba;
        *p++ = 0x40;
        memset(p, 0, 9);
        p += 9;
        {   /* Packet */
            static uchar last_pts[5] = { 0, 0, 0, 0, 0};
            uchar now_pts[5];
            int pts_len, pad_len, datalen = len;
            pts *= 90000;
            now_pts[0] = 0x21 | (((ulong)pts >> 29) & 0x0e);
            now_pts[1] = ((ulong)pts >> 22) & 0xff;
            now_pts[2] = 0x01 | (((ulong)pts >> 14) & 0xfe);
            now_pts[3] = ((ulong)pts >> 7) & 0xff;
            now_pts[4] = 0x01 | (((ulong)pts << 1) & 0xfe);
            pts_len = memcmp(last_pts, now_pts, sizeof(now_pts)) ? sizeof(now_pts) : 0;
            memcpy(last_pts, now_pts, sizeof(now_pts));

            datalen += 3;       /* Version, PTS_flags, pts_len */
            datalen += pts_len;
            datalen += 1;       /* AID */
            pad_len = 2048 - (p - buffer) - 4 /* MPEG ID */ - 2 /* payload len */ - datalen;
            /* XXX - Go figure what should go here!  In any case the
               packet has to be completly filled.  If I can fill it
               with padding (0x000001be) latter I'll do that.  But if
               there is only room for 6 bytes then I can not write a
               padding packet.  So I add some padding in the PTS
               field.  This looks like a dirty kludge.  Oh well... */
            if (pad_len < 0) {
                /* Packet is too big.  Let's try ommiting the PTS field */
                datalen -= pts_len;
                pts_len = 0;
                pad_len = 0;
            } else if (pad_len > 6)
                pad_len = 0;
            datalen += pad_len;

            *p++ = 0;           /* 0x0e */
            *p++ = 0;
            *p++ = 1;
            *p++ = 0xbd;

            *p++ = (datalen >> 8) & 0xff; /* length of payload */
            *p++ = datalen & 0xff;
            *p++ = 0x80;                /* System-2 (.VOB) stream */
            *p++ = pts_len ? 0x80 : 0x00; /* pts_flags */
            *p++ = pts_len + pad_len;
            memcpy(p, now_pts, pts_len);
            p += pts_len;
            memset(p, 0, pad_len);
            p += pad_len;
        }
        *p++ = 0x20 |  vob->aid; /* aid */
        if (fwrite(buffer, p - buffer, 1, vob->fsub) != 1
                || fwrite(packet, len, 1, vob->fsub) != 1)
            perror("ERROR: vobsub write failed");
        else
            remain -= p - buffer + len;

        /* Padding */
        if (remain >= 6) {
            p = buffer;
            *p++ = 0x00;
            *p++ = 0x00;
            *p++ = 0x01;
            *p++ = 0xbe;
            *p++ = (remain - 6) >> 8;
            *p++ = (remain - 6) & 0xff;
            /* for better compression, blank this */
            memset(buffer + 6, 0, remain - (p - buffer));
            if (fwrite(buffer, remain, 1, vob->fsub) != 1)
                perror("ERROR: vobsub padding write failed");
        } else if (remain > 0) {
            /* I don't know what to output.  But anyway the block
               needs to be 2KB big */
            memset(buffer, 0, remain);
            if (fwrite(buffer, remain, 1, vob->fsub) != 1)
                perror("ERROR: vobsub blank padding write failed");
        } else if (remain < 0)
            fprintf(stderr,
                    "\nERROR: wrong thing happenned...\n"
                    "  I wrote a %i data bytes spu packet and that's too long\n", len);
    }

}


void k9Subtitle2Pgm::setBaseName(const QString& theValue) {
    m_baseName = theValue;
}


void k9Subtitle2Pgm::end() {
    m_fifo->setNoData();
    while (isRunning()) {
        wait(100);
        k9Tools::processEvents();
    }
}


QTime k9Subtitle2Pgm::pts2Time(double _pts) {

    uint h, m, ms;
    double s;
    s = _pts;
    h = s / 3600;
    s -= h * 3600;
    m = s / 60;
    s -= m * 60;
    ms = (s - (uint) s) * 1000;
    if (ms >= 1000)     /* prevent overfolws or bad float stuff */
        ms = 0;
    QTime t(h,m,s,ms);
    qDebug() << " h:" << h << " m:" << m << " s:" << s << " ms:" << ms << "   time:" << t.toString("hh:mm:ss.zzz");
    return t;


}

void k9Subtitle2Pgm::setPalette(uint _index,uint value) {
    m_palette[_index]=value;
}


void k9Subtitle2Pgm::setLangCod(const QString& theValue) {
    m_langCod = theValue;
}


void k9Subtitle2Pgm::setTrackCod(int theValue) {
    m_trackCod = theValue;
}
