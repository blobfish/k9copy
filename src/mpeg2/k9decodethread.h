//
// C++ Interface: k9decodethread
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9DECODETHREAD_H
#define K9DECODETHREAD_H
#include "k9common.h"
#include "k9vamps.h"

#include <qthread.h>
#include <qobject.h>
#include <qwaitcondition.h>
#include "kdecmpeg2.h"
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9SaveImage;
class k9InternalPlayer;
class k9BackupDlg;
class k9DecodeThread : public QThread {
private:
    k9MemoryFifo m_fifo;
    QWaitCondition wDataRead;
    QWaitCondition wDataReady;
    bool noData;
    kDecMPEG2 *m_decmpeg2;
    k9SaveImage *m_saveImage;
    k9InternalPlayer *m_viewMpeg2;
    k9InternalPlayer *m_backupDlg;
public:
    k9DecodeThread();
    ~k9DecodeThread();
    void addData(uchar *data,uint size);
    void setNoData();
    void clear();
    void sleepms(int _ms);

    void setSaveImage(k9SaveImage* theValue);

    void setViewMpeg2(k9InternalPlayer* theValue);

    void setBackupDlg(k9InternalPlayer* theValue);

    void Stop();

     void test();
    void pgm_save(uchar * buf,int wrap,int width,int height);
    uint32_t freespace();
    uint32_t count();
protected:
    int readData(uchar * data,uint size);
    void run();



};

#endif
