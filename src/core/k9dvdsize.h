//
// C++ Interface: k9dvdsize
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9DVDSIZE_H
#define K9DVDSIZE_H

#include <QObject>
/**
@author Jean-Michel PETIT
*/
class k9DVDSize:public QObject {
Q_OBJECT
private:
    static int m_size;
public:
    k9DVDSize(QObject  *parent=0,const char *name=0) ;
    static int getMaxSize();
   static void setMaxSize(int _size);
    virtual int getMaxSizeDyn();
    virtual void setMaxSizeDyn(int _size);
    ~k9DVDSize();

};

#endif
