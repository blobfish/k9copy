//
// C++ Implementation: k9updatefactor
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9updatefactor.h"

k9UpdateFactor::k9UpdateFactor(QObject *parent, const char *)
 : QThread(parent)
{
}


k9UpdateFactor::~k9UpdateFactor()
{
   wait();
}

void k9UpdateFactor::run(){
    do  {
    	m_restart=false;
    	emit updateFactor_internal();
    } while (m_restart);
}

void k9UpdateFactor::updateFactor() {
   m_mutex.lock();
   if (isRunning()) {
   	m_restart=true;
   }
   else
   	start();
   m_mutex.unlock();
}

