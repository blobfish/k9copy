//
// C++ Implementation: k9saveimage
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9saveimage.h"
#include "k9decodethread.h"
#include <QDir>


#include <QImage>

void k9SaveImage::drawImage(QImage *_image) {
    m_cpt++;
    if (m_cpt ==20) {
     m_mutex.lock();
     m_image=*_image;
     m_mutex.unlock();
     //m_decoder->pause();
    }
}


k9SaveImage::k9SaveImage()
 : QThread()
{
    m_tempFile= new QTemporaryFile(k9Tools::getTempPath()+"k9pXXXXXX.png");
    m_tempFile->open();
    m_tempFile->setAutoRemove(true);
    m_decoder=new k9DecodeThread();
    m_decoder->setSaveImage(this);
    //connect(m_decoder, SIGNAL(pixmapReady(QImage*)), this, SLOT(drawImage(QImage*)));
}


k9SaveImage::~k9SaveImage()
{
    wait();
    delete m_decoder;
    delete m_tempFile;
}

void k9SaveImage::play() {
    m_cpt=0;
    m_stop=false;
    m_timer.start();
    m_decoder->start(LowestPriority);
}
void k9SaveImage::stop() {
    m_stop=true;
    wait();
    //m_decoder->stop();
}

void k9SaveImage::addData(uchar *_buffer, uint32_t _size) {
    if (m_timer.elapsed() >=7000 ) {
        if (m_mutex.tryLock()) {
            m_size=_size;

            m_buffer=new uchar[m_size];
            memcpy(m_buffer,_buffer,m_size);
            m_mutex.unlock();
            start(LowestPriority);
        }
        m_timer.restart();
    }
}


void k9SaveImage::run() {
    m_mutex.lock();
    m_cpt=0;
    m_decoder->addData(m_buffer ,m_size);
    delete m_buffer;
    QString sFileName=m_tempFile->fileName();

    m_image.save(sFileName,"PNG");
    m_fileName=sFileName;
    sFileName="\rINFOIMAGE:"+sFileName;
    fprintf(stderr, "%s", (const char*) sFileName.toUtf8());

    m_mutex.unlock();
}



QString k9SaveImage::getFileName()  {
    m_mutex.lock();
    QString ret( m_fileName.isNull() ? QString(""):m_fileName);
    m_mutex.unlock();
    return ret;
}
