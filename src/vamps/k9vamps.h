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
#ifndef K9VAMPS_H
#define K9VAMPS_H

#include "k9common.h"
#include <qfile.h>
#include <qthread.h>
#include <qobject.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/stat.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include "k9dvdbackupinterface.h"
#include "k9requant.h"
#include "k9fifo.h"
#include "k9saveimage.h"
#include <QMap>
// DVD sector size
#define SECT_SIZE 2048

// read buffer size (4MB)
#define RBUF_SIZE (0x1000*1024)

// write buffer size (4MB)
#define WBUF_SIZE (0x1000*1024)

// initial video buffer size (1MB)
#define VBUF_SIZE (1024*1024)




class k9bgUpdate : public QThread
{
private:
	uchar * m_buffer;
	k9DVDBackupInterface *m_backup;
	uint32_t m_size;
	QMutex mutex;
public:
	k9bgUpdate(k9DVDBackupInterface * _backup);
	void update(uchar *_buffer, uint32_t size);
protected:
	void run();

};




class k9vamps:public QThread
{
private:
	uchar       *rbuf;//  [RBUF_SIZE];		// the PS read buffer
	uchar       wbuf  [WBUF_SIZE];		// the PS write buffer
	uchar      *vibuf;			// the video ES requant input buffer
	uchar      *vobuf;			// the video ES requant output buffer
	uchar      *rptr ;			// pointer to current char in read buf
	uchar      *rhwp ;			// read buffer high water pointer
	uchar      *wptr ;			// pointer to first unused char in wbuf
	uint64_t      bytes_read;			// total PS bytes read
	uint64_t      bytes_written;		// total PS bytes written
	uint64_t      padding_bytes;		// total padding bytes written
	uint64_t      vin_bytes;			// total unshrinked video ES bytes
	uint64_t      vout_bytes;			// total shrinked video ES bytes
	uint64_t      ps_size;			// total PS size in bytes
	uint32_t      vbuf_size;			// the video ES requant buffers' size
	uint32_t	    rbuf_size;
	uint32_t      vilen;				// current GOP's unshrinked vidES bytes
	uint32_t      volen;				// current GOP's shrinked vidES bytes
	int         total_packs;		// total no. PS packs
	int         video_packs;		// no. video packs in PS
	int         skipped_video_packs;	// skipped thereof
	int         aux_packs;			// no. audio and subpicture packs in PS
        int         skipped_aux_packs;		// skipped thereof
        int         sequence_headers;		// no. sequence headers (== #GOPs)
	int         nav_packs;			// no. nav packs
	int         eof;			// end of file flag
	int         spu_track_map [32];		// subpicture track# translation map
	int         audio_track_map [8];	// audio track# translation map
	int         verbose;			// level of verbosity
	int         calc_ps_vap;		// calc vaporization based on PS size
	bool        m_preserve;			// preserve audio/spu track numbers
	float       vap_fact;			// vaporization factor from cmd line
	bool		noData;
	QMutex		mutex;
	k9bgUpdate *m_bgUpdate;
	//k9MemoryFifo m_fifo;
        k9fifo *m_fifo;
	QString m_errMsg;
	bool m_error;

	double avgdiff;
	double m_totfact,m_nbfact,m_avgfact;
	QFile *m_output;
private:
	// prototypes
	void vaporize (void);
	void fatal (QString _msg);
	int lock(int size);
	void copy(int size);
	void skip (int size);
	void flush();
	uint64_t wtell (uchar *ptr);

	uint64_t rtell (uchar *ptr);
	bool check_pack (uchar *ptr);
	int check_video_packet (uchar *ptr);
	int requant (uchar *dst, uchar *src, int n, float fact);
	int new_private_1_type (uchar *ptr);
	void copy_private_1 (uchar *ptr);
	int new_mpeg_audio_id (int id);
	void copy_mpeg_audio (uchar *ptr);
	void vap_leader ();
	void vap_trailer (int length);
	int vap_phase1 (void);
	int gen_video_packet (uchar *ptr, uchar *voptr, int avail);
	void vap_phase2 (int seq_length);
  	pthread_t            thread;
	int getStreamIndex(uchar *ptr);
	int readData(uchar *data,uint size);

	QWaitCondition wDataRead;
	QWaitCondition wDataReady;
	k9DVDBackupInterface *m_dvdbackup;
	k9requant *m_requant;
        k9SaveImage *m_saveImage;
protected:
	void run();
public:
	k9vamps(k9DVDBackupInterface *dvdbackup,bool _useCache=false);
	void addData(uchar* data,uint size);
	void setNoData();
	void addSubpicture(uint id);
	void addAudio(uint id);
        void addAudio(uint id,uint newId);
	void reset();
	void setInputSize(uint64_t size);
	void setVapFactor(float factor);
        void setSaveImage(k9SaveImage*);
	void setOutput(QFile *_output);
	uint64_t getOutputBytes();
        uint64_t getSkippedBytes();
	QString & geterrMsg();
	bool geterror();
	void abort();
        void setPreserve(bool _value);
	~k9vamps();
};


#endif
