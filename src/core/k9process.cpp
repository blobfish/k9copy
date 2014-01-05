//
// C++ Implementation: k9process
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9process.h"
#include "k9common.h"
#include <QObject>
#include <QEventLoop>
#include "k9tools.h"
#include <QDebug>
#include "k9dialogs.h"

k9Process::k9Process(QObject *parent, const char *)
 : KProcess(parent),m_waitSync(false)
{
   setOutputChannelMode (KProcess::SeparateChannels);
   //connect(this,SIGNAL(processExited( K3Process* )),this,SLOT(slotProcessExited( K3Process* )));
   connect(this,SIGNAL(readyReadStandardError ()),this,SLOT(slotReadyReadStandardError ()));
   connect(this,SIGNAL(readyReadStandardOutput ()),this,SLOT(slotReadyReadStandardOutput ()));
   connect(this,SIGNAL(finished ( int , QProcess::ExitStatus )),this,SLOT(slotFinished(int, QProcess::ExitStatus)));
   connect(this,SIGNAL(started()),this,SLOT(slotStarted()));
   connect(this,SIGNAL(error(QProcess::ProcessError)),this,SLOT(slotError(QProcess::ProcessError)));
   m_shellCmd=false;
   m_debugOutput=false;
}


k9Process::~k9Process()
{
}

void k9Process::syncStart() {
  m_waitSync=true;
  QEventLoop eventLoop;
  m_eventLoop=&eventLoop;
  eventLoop.exec();
}



void k9Process::sync() {
  m_waitSync=true;
  if (state() == QProcess::NotRunning)
	return;
  QEventLoop eventLoop;
  m_eventLoop=&eventLoop;
  //QApplication::eventLoop()->enterLoop();
  eventLoop.exec();
}

void k9Process::slotReadyReadStandardError() {
   QByteArray ba=readAllStandardError();
   getStderr(ba);
   emit receivedStderr(ba.data(), ba.length());
    emit receivedStderr(this,ba.data(), ba.length());
}

void k9Process::slotReadyReadStandardOutput() {
   QByteArray ba=readAllStandardOutput();
   getStdout(ba);
   emit receivedStdout( ba.data(), ba.length());
   emit receivedStdout(this, ba.data(), ba.length());
}


void k9Process::getStdout(QByteArray ba) {
   if (m_debugOutput) {
       qDebug() << ba;
   }

}

void k9Process::getStderr(QByteArray ba) {
   if (m_debugOutput) {
       qDebug() << ba;
   }

}

void k9Process::wait() {
    waitForFinished(-1);
}

void k9Process::slotFinished( int,QProcess::ExitStatus) {
   if (m_waitSync) {
   	//QApplication::eventLoop()->exitLoop();
	m_eventLoop->exit();
   	m_waitSync=false;	
   }
   emit processExited(this);

}

void k9Process::slotStarted() {
   if (m_waitSync) {
	m_eventLoop->exit();
   	m_waitSync=false;	
   }
}

void k9Process::slotError(QProcess::ProcessError ) {
   if (m_waitSync) {
	m_eventLoop->exit();
   	m_waitSync=false;	
   }
}


const QString & k9Process::debug() {
    m_debug="";
    for (int i=0;i<program().count();i++ ){
        m_debug +=" "+ (program().at(i));
    }
    return m_debug;
}

void k9Process::setShellCommand (const QString &cmd) {
    KProcess::setShellCommand(cmd);
    m_shellCmd=true;
}

void k9Process::clearProgram() {
    KProcess::clearProgram();
    m_shellCmd=false;
}

bool  k9Process::start () {
 QString progName;
 progName=program().at(0);


 if (!k9Tools::checkProgram(progName)) {
     k9Dialogs::error(i18n("Error starting program %1",progName),i18n("Running program"));
     return false;
  }
	
  m_elapsed.start();
  KProcess::start();
  syncStart();
  return (state() != QProcess::NotRunning);
}

bool k9Process::isRunning() {
   return state()==QProcess::Running;
}

bool k9Process::normalExit() {
   return exitStatus() == QProcess::NormalExit;
}

int k9Process::getElapsed() const {
    return m_elapsed.elapsed();
}


void k9Process::setDebugOutput(bool theValue) {
    m_debugOutput = theValue;
}
