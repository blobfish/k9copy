//
// C++ Interface: k9fifo
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9FIFO_H
#define K9FIFO_H

#include "k9common.h"
#include <QMutex>
#include <QWaitCondition>
#include <QTemporaryFile>
#include <QQueue>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

#define INPUT_SIZE ( 0x2000*1024)
#define MAX_FILE_SIZE (0x100*1024*1024)
#define MAX_NUMBER_OF_FILES 50
class k9fifo{
public:
   virtual uint64_t count() = 0;
   virtual uint64_t freespace() =0;
   virtual void enqueue (uchar *_buffer, uint32_t _size) =0 ;
   virtual void dequeue(uchar *_buffer,uint32_t _size) =0;
   virtual void clear()=0;
   virtual ~k9fifo(){}
}; 


class k9FileFifo:public k9fifo {
private:
        QTemporaryFile *m_rfile,*m_wfile;
	QMutex m_mutex;
        quint64 m_end,m_start;
	qint64 m_fstart,m_fend;
        QQueue <QTemporaryFile*> m_queue;
	QString m_output;
        QWaitCondition m_wait;
public:
	k9FileFifo ();
        uint64_t count(); // { return  (queue-head)%INPUT_SIZE ;}
        uint64_t freespace() {return 5*MAX_FILE_SIZE;}
	void enqueue (uchar *_buffer, uint32_t _size) ;
	void dequeue(uchar *_buffer,uint32_t _size) ;
        void clear();
	virtual ~k9FileFifo();
};


class k9MemoryFifo:public k9fifo {
private:
	uint32_t  head,queue;
	uchar *array;
	uint32_t m_count;
	bool noData;
	QMutex mutex;
	QWaitCondition wDataRead,wDataReady;
public:
	k9MemoryFifo () { head=queue=m_count=0;noData=false;array= new uchar[INPUT_SIZE];}
        uint64_t count(); // { return  (queue-head)%INPUT_SIZE ;}
        uint64_t freespace() {return INPUT_SIZE-count();}
	void enqueue (uchar *_buffer, uint32_t _size) ;
	void dequeue(uchar *_buffer,uint32_t _size) ;
        void clear();
        void setNoData();
        void addData(uchar *data,uint size);
        int readData(uchar * data,uint size);


        virtual ~k9MemoryFifo() { delete[] array;}
};

#endif
