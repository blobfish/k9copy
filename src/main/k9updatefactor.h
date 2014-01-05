//
// C++ Interface: k9updatefactor
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9UPDATEFACTOR_H
#define K9UPDATEFACTOR_H

#include "k9common.h"
#include <qobject.h>
#include <qthread.h>
#include <qmutex.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9Main;
class k9UpdateFactor : public QThread
{
Q_OBJECT
private:
    QMutex m_mutex;
    bool m_restart;
protected:
    void run();
public:
    k9UpdateFactor(QObject *parent = 0, const char *name = 0);

    ~k9UpdateFactor();
    void updateFactor();
signals:
    void updateFactor_internal();

};

#endif
