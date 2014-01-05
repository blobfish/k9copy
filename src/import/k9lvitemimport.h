//
// C++ Interface: k9lvitemimport
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9LVITEMIMPORT_H
#define K9LVITEMIMPORT_H

#include "k9common.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "k9avifile.h"
#include "k9title.h"
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9LvItemImport: public QTreeWidgetItem {
public:
    enum  eObjectType {ROOT=1000,TITLE=1001,CHAPTER=1002};

    k9LvItemImport( QTreeWidgetItem *parent,eObjectType _objectType);
    ~k9LvItemImport();
    eObjectType getObjectType() { return m_objectType;};

    void setAviFile ( k9AviFile* _value ){m_aviFile = _value;}
    k9AviFile* getAviFile() const { return m_aviFile;}

    void setTitle(k9Title* _value);
    k9Title* getTitle() const { return m_title;}
//    virtual int compare (QListViewItem *i,int col,bool ascending) const;
    bool operator< ( const QTreeWidgetItem & other ) const  ;

private:
    eObjectType m_objectType;
    k9AviFile *m_aviFile;
    k9Title *m_title;
};

#endif
