//
// C++ Interface: k9saveimage
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9SAVEIMAGE_H
#define K9SAVEIMAGE_H

#include "k9common.h"
#include <qthread.h>
#include <qobject.h>
#include <qdatetime.h>
#include <QTemporaryFile>
#include "k9fifo.h"
#include <qimage.h>


/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
typedef enum  { FrameType_B=0x18,FrameType_I=0x08,FrameType_P=0x10,FrameType_D=0x20} eFrameType;

class k9DecodeThread;

class k9SaveImage : public QThread {
    Q_OBJECT
public:
    k9SaveImage();
    ~k9SaveImage();

    void play();
    void stop();
    void addData(uchar *_buffer, uint32_t _size);
protected:
    void run();
public slots:
    void drawImage(QImage*);

    QString getFileName() ;


private:
    k9DecodeThread *m_decoder;
    k9MemoryFifo m_fifo;
    bool m_stop;
    QTime m_timer;
    uint m_cpt;
    QTemporaryFile *m_tempFile;
    uchar *m_buffer;
    uint32_t m_size;
    QMutex m_mutex;
    QImage m_image;
    QString m_fileName;
};

#endif
