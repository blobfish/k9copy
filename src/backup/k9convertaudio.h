//
// C++ Interface: k9convertaudio
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9CONVERTAUDIO_H
#define K9CONVERTAUDIO_H

#include "k9common.h"
#include <QThread>
#include "k9fifo.h"
#include <QWaitCondition>
#include <QEventLoop>



class k9ConvertAudio: public QThread {
    Q_OBJECT
public:
    k9ConvertAudio(const QString &_output,const QStringList &_cmd) ;
    ~k9ConvertAudio();
    void addData(uchar *_buffer,int _size);
    void end(bool _wait=true);
    static void test();

    QString getFps() const;
    void setFps(const QString& theValue);
    QStringList getCmd() const;
    void setDebug(bool theValue);

protected:
    virtual void run();
public slots:
    void finished ( int exitCode, QProcess::ExitStatus exitStatus );
    bool Error() const;

    QStringList & getOutput() ;
	

private:
    k9Process *m_process;
    QString m_fileName;
    k9MemoryFifo *m_fifo;
    bool m_nodata;
    QStringList m_cmd;
    QStringList m_output;
    QWaitCondition m_dataReady;
    QEventLoop m_eventLoop;
    QMutex m_mutex;
    QString m_fps;
    bool m_debug;
    bool m_error;
};

class _k9ConvertOutput: public QObject {
    Q_OBJECT
public:
    _k9ConvertOutput(k9ConvertAudio *parent);
private:
    k9ConvertAudio *m_parent;
public slots:
    void getStdout(char *buffer, int buflen) ;
    void finished ( int exitCode, QProcess::ExitStatus exitStatus );

};
#endif
