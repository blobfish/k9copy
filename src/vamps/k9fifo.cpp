//
// C++ Implementation: k9fifo
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9fifo.h"
#include <QDir>

uint64_t k9MemoryFifo::count() {
	return m_count;
}

void k9MemoryFifo::enqueue (uchar *_buffer, uint32_t _size) {
   mutex.lock();
    if (_size+queue > INPUT_SIZE) {
        uint32_t s1,s2;
        s1=INPUT_SIZE-queue;
        memcpy(array+queue,_buffer,s1);
        s2=_size-s1;
        memcpy(array,_buffer+s1,s2);
    } else
        memcpy(array+queue,_buffer,_size);
    queue=(queue+_size) %INPUT_SIZE;
    m_count+=_size;
    mutex.unlock();
}

void k9MemoryFifo::dequeue(uchar *_buffer,uint32_t _size) {
    mutex.lock();
    if ( _size+head >INPUT_SIZE) {
        uint32_t s1,s2;
        s1=INPUT_SIZE - head;
        memcpy(_buffer,array+head,s1);
        s2=_size-s1;
        memcpy(_buffer+s1,array,s2);
    } else
        memcpy(_buffer,array+head,_size);
    head =(head+_size)%INPUT_SIZE;
    m_count -=_size;
    mutex.unlock();
}

void k9MemoryFifo::clear() {
    mutex.lock();
    head=queue;
    m_count=0;
    mutex.unlock();
}


void k9MemoryFifo::setNoData() {
    noData=true;
    wDataRead.wakeAll();
    wDataReady.wakeAll();
}

void k9MemoryFifo::addData(uchar *data,uint size) {
    while (1) {
        if (freespace()>=size) {
            enqueue(data,size);
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


int k9MemoryFifo::readData(uchar * data,uint size) {
    uint size2=size;
    uint32_t readSize=0,s=0;

    while (1) {
        // is there data in the buffer?
        if (count() >0) {
            // s= size of data that we will read (maximum = size)
            s=(count()) <size2 ? (count()) : size2;
            // increments the number of readen bytes
            readSize+=s;
            // decrements the number of max bytes to read
            size2-=s;
            //moves bytes from buffer to output
            dequeue(data,s);
            //moves the position of output buffer to receive next bytes
            data+=s;
            //there's now free space in input buffer, we can wake the injection thread
            wDataRead.wakeAll();
        }
        // break the loop if injection thread terminated or we got what we want (size bytes)
        // otherwise, we're waiting for datas
        if (noData || (count() >=size2)) {
            break;
        } else {
            QMutex m;
            m.lock();
            wDataReady.wait(&m);
            m.unlock();
        }
    }
    // if there's datas in input buffer and we did not get all what we wanted, we take them.
    s= (count()) <size2 ? (count()) : size2;
    readSize+=s;
    if (s>0 )
        dequeue(data,s);

    wDataRead.wakeAll();
    return readSize;
}





//********************************************************************
k9FileFifo::k9FileFifo() {
    m_rfile=m_wfile=NULL;
    m_output=k9Tools::getTempPath();
    clear();
    m_wfile=m_rfile;
}
void k9FileFifo::enqueue (uchar *_buffer, uint32_t _size) {
   m_mutex.lock();
   while (m_queue.count() >MAX_NUMBER_OF_FILES) {
     m_wait.wait(&m_mutex);
   }
   m_wfile->seek(m_fstart);
   m_wfile->write((const char*)_buffer,_size);
   m_fstart=m_wfile->pos();
   if (m_fstart > MAX_FILE_SIZE) {
        m_wfile=new QTemporaryFile(QDir::cleanPath(m_output +"/k9b"));
        m_wfile->setAutoRemove(true);
	m_wfile->open();
	m_queue.enqueue(m_wfile);
  	m_fstart=0;
   }
   m_end+=_size;

   m_mutex.unlock();
}
uint64_t k9FileFifo::count() {
   return m_end-m_start;
}


k9FileFifo::~k9FileFifo(){
   if (m_rfile) {
      m_rfile->close();
      delete m_rfile;
   }
   while (m_queue.count() >0) {
      QTemporaryFile *file=m_queue.dequeue();
      file->close();
      delete file;
   }
}

void k9FileFifo::dequeue(uchar *_buffer,uint32_t _size) {
    uint32_t size2=0;
    m_mutex.lock();
    m_rfile->seek(m_fend);
    uint32_t size=m_rfile->read((char*)_buffer,_size);
    m_start+=size;
    size2+=size;

    while (size2 <_size && m_queue.count()>0) {
	m_rfile->close();
	delete m_rfile;
	m_rfile=m_queue.dequeue();
	m_rfile->seek(0);
	size=m_rfile->read((char*)_buffer+size2,_size-size2);
	m_start+=size;
	size2+=size;
        m_wait.wakeAll();
    }
    m_fend=m_rfile->pos();
    m_mutex.unlock();
}

void k9FileFifo::clear() {
	if (m_rfile) {
          m_rfile->close();
          delete m_rfile;
        }
	qDeleteAll(m_queue);

	m_fstart=m_fend=0;
        m_rfile=new QTemporaryFile(QDir::cleanPath(m_output +"/k9b"));
        m_rfile->setAutoRemove(true);
	m_rfile->open();
	m_start=m_end=0;
}
