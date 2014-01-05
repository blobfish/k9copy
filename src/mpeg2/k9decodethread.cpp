//
// C++ Implementation: k9decodethread
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9decodethread.h"
#include "k9internalplayer.h"
#include "k9saveimage.h"

#include <QFile>


k9DecodeThread::k9DecodeThread() {
    noData=FALSE;
    m_viewMpeg2=NULL;
    m_backupDlg=NULL;
    m_saveImage=NULL;
    m_decmpeg2=new kDecMPEG2(this);
}


k9DecodeThread::~k9DecodeThread() {
   setNoData();
   wait();
   delete m_decmpeg2;
}

void k9DecodeThread::clear() {
    m_fifo.clear();
    wDataRead.wakeAll();
    wDataReady.wakeAll();

}

void k9DecodeThread::addData(uchar *data,uint size) {
    if (! isRunning()) start();
    m_fifo.addData(data,size);
}


uint32_t k9DecodeThread::freespace() {
   return m_fifo.freespace();
}


uint32_t k9DecodeThread::count() {
   return m_fifo.count();
}

int k9DecodeThread::readData(uchar * data,uint size) {
    return m_fifo.readData(data,size);
}

void k9DecodeThread::setNoData() {
    noData=true;
    m_fifo.setNoData();
    wDataRead.wakeAll();
    wDataReady.wakeAll();
}

void k9DecodeThread::sleepms(int _ms) {
    msleep(_ms);
}

#define INBUF_SIZE   2048

void k9DecodeThread::test() {


}


void k9DecodeThread::run() {
    noData=FALSE;
      m_decmpeg2->start();
      while (1) {
       int count=2048;
       uchar buffer[count];
       uint32_t size=readData(buffer,count);
       if (size==0 && noData)
       break;
       m_decmpeg2->decode(buffer ,buffer+size,0);
      }
      m_decmpeg2->stop();
    
  }



void k9DecodeThread::pgm_save(uchar * buf,int wrap,int width,int height) {
    if (noData)
        return;

    int len;
    len =(int) (3*width*height);
    char c[255];
    sprintf(c,"P6\n%d %d\n255\n", width, height);
    char *s;
    s= new char[len+strlen(c)];
    memcpy(s,c,strlen(c));
    memcpy(s+strlen(c),buf, len);
    QImage pix;
    pix.loadFromData((uchar*)s,strlen(c)+len);
    delete s;
    if (m_viewMpeg2)
        m_viewMpeg2->drawImage(&pix);
    else if (m_saveImage)
        m_saveImage->drawImage(&pix);
    else if (m_backupDlg)
        m_backupDlg->drawImage(&pix);


}



void k9DecodeThread::setSaveImage(k9SaveImage* theValue) {
    m_saveImage = theValue;
}


void k9DecodeThread::setViewMpeg2(k9InternalPlayer* theValue) {
    m_viewMpeg2 = theValue;
}


void k9DecodeThread::setBackupDlg(k9InternalPlayer* theValue) {
    m_backupDlg = theValue;
}


void k9DecodeThread::Stop() {
    setNoData();
}
