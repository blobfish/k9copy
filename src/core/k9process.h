//
// C++ Interface: k9process
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9PROCESS_H
#define K9PROCESS_H
#include <QDateTime>
#include <QEventLoop>
#include <KProcess>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Process : public KProcess
{
Q_OBJECT
public:
    k9Process(QObject *parent = 0, const char *name = 0);
    void sync();
    const QString & debug();
    ~k9Process();
    int getElapsed() const;
    virtual bool start ();
    void setShellCommand (const QString &cmd);
    void clearProgram();
    void wait();
    bool isRunning();
    bool normalExit();
    void setDebugOutput(bool theValue);
	
private:
    bool m_waitSync;
    bool m_shellCmd;
    QString m_debug;
    bool m_debugOutput;
    QTime m_elapsed;
    QEventLoop *m_eventLoop;
    void syncStart();
protected:
   virtual void getStdout(QByteArray data);
   virtual void getStderr(QByteArray data);
private slots:
   void slotReadyReadStandardError ();
   void slotReadyReadStandardOutput ();
   void slotStarted();
   void slotError(QProcess::ProcessError );
   void slotFinished ( int exitCode, QProcess::ExitStatus exitStatus );
signals:
    void receivedStderr(char *, int);
    void receivedStdout(char *, int );
    void receivedStderr(k9Process*,char *, int);
    void receivedStdout(k9Process*,char *, int );
    void processExited(k9Process *);
};

#endif
