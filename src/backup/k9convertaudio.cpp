//
// C++ Implementation: k9convertaudio
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9convertaudio.h"
#include <QFile>
#include <QDebug>
#include <QApplication>
#include "k9log.h"


k9ConvertAudio::~k9ConvertAudio() {
    delete m_fifo;
}


k9ConvertAudio::k9ConvertAudio(const QString &_output,const QStringList &_cmd):QThread(NULL) {
    if (_output !="") {
        m_fileName=_output;
        QFile::remove(_output);
    }
    m_fifo=new k9MemoryFifo();
    m_cmd=_cmd;
    m_debug=false;
    m_error=false;
}

void k9ConvertAudio::finished ( int exitCode, QProcess::ExitStatus exitStatus ) {
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    if (!m_nodata)
	m_error=true;
}


void _k9ConvertOutput::finished ( int exitCode, QProcess::ExitStatus exitStatus ) {
    m_parent->finished(exitCode,exitStatus);
}

_k9ConvertOutput::_k9ConvertOutput(k9ConvertAudio *parent):QObject(){
   m_parent=parent;
}

void _k9ConvertOutput::getStdout( char *buffer, int buflen) {
    QString tmp( QByteArray(buffer,buflen));
    if (! tmp.isEmpty()) {
	m_parent->getOutput() << tmp;
	if (m_parent->getOutput().count() > 10)
	   m_parent->getOutput().removeAt(0);
    }
    if (m_parent->getCmd().at(0).contains("mencoder")) {
        

        int pos=tmp.indexOf("Pos:");
        if (pos!=-1) {
            QString tmp2=tmp.mid(pos);
            float t;
            int frame;
            int fps;
            sscanf(tmp2.toUtf8(),"Pos: %f%*s%d",&t,&frame);
            tmp2=tmp2.mid(tmp2.indexOf("(")+1);
            tmp2=tmp2.mid(tmp2.indexOf(")")+1);
            sscanf(tmp2.toUtf8(),"%d",&fps);

            m_parent->setFps(QString::number(fps));
        } //else
          //  k9log::add(tmp,k9logLevel::INFO);
    }
    if (m_parent->getCmd().at(0).contains("ffmpeg")) {
	int pos=tmp.indexOf("fps=");
	if (pos!=-1) {
	    QString tmp2=tmp.mid(pos+4);
	    pos=tmp2.indexOf("q");
	    if (pos!=-1)
	    	m_parent->setFps(tmp2.mid(0,pos));
 	}

    }// else
     //       k9log::add(tmp,k9logLevel::INFO);

}


void k9ConvertAudio::run() {
    _k9ConvertOutput *output=new _k9ConvertOutput(this);
    m_nodata=false;
    m_error=false;
    m_process=new k9Process(0,0);
    connect(m_process, SIGNAL(finished ( int , QProcess::ExitStatus )),output,SLOT(finished(int, QProcess::ExitStatus)));
    connect(m_process, SIGNAL(receivedStdout( char *, int)),output, SLOT(getStdout(char *, int) ));
    connect(m_process, SIGNAL(receivedStderr(char *, int)),output, SLOT(getStdout( char *, int) ));
    m_process->setDebugOutput(m_debug);
    if (m_fileName!="") {
        *m_process << "ffmpeg" << "-i" << "/dev/stdin" << m_fileName;
    } else {
	//mencoder must be run in a shell cmd to enable filters !!
        //ffmpeg too, in order to make 2 passes encoding
        //setting the working directory as the output directory of k9copy
        k9Config cfg;
        m_process->setWorkingDirectory(cfg.getPrefOutput());
        m_process->setShellCommand(m_cmd.join(" "));
    }
    qDebug() << m_process->debug();
    m_process->start();


//    while (m_process->isRunning() && !m_nodata) {
      while (m_process->isRunning()) {
        int size=m_fifo->count();

        size=qMin(size,4096);

        if (size >0 ) {
            m_mutex.lock();
            while (m_process->isRunning()) {
                if (m_process->bytesToWrite() >size)
                    m_process->waitForBytesWritten(-1);
                else
                    break;
            }
            m_mutex.unlock();
            uchar  buffer[size];
            //m_fifo->dequeue(buffer,size);
            m_fifo->readData(buffer,size);
	    if (m_process->isRunning())
               m_process->write((char*)buffer,size);
            //m_dataReady.wakeAll();

        } else if (m_nodata) {
            while (m_process->bytesToWrite() >0)
                m_process->waitForBytesWritten();
            m_process->closeWriteChannel();
            m_process->wait();
        } else {
          //  m_process->waitForFinished(10);
        }
    }
    delete output;
}

void k9ConvertAudio::addData(uchar *_buffer,int _size) {
    if (m_error)	
	return;

    if (!isRunning())
        start();
    m_fifo->addData(_buffer,_size);
}
void k9ConvertAudio::end(bool _wait) {
    m_nodata=true;
    m_fifo->setNoData();
    if (_wait)
        while (isRunning()) {
            qApp->processEvents();
            wait(100);
        }
}

void k9ConvertAudio::test() {
   
}


QString k9ConvertAudio::getFps() const {
    return m_fps;
}


void k9ConvertAudio::setFps(const QString& theValue) {
    m_fps = theValue;
}


QStringList k9ConvertAudio::getCmd() const {
    return m_cmd;
}


void k9ConvertAudio::setDebug(bool theValue) {
    m_debug = theValue;
}


bool k9ConvertAudio::Error() const {
    return m_error;
}


QStringList &k9ConvertAudio::getOutput()  {
    return m_output;
}
