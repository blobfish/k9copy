//
// C++ Implementation: k9lvitemimport
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9lvitemimport.h"

//1001 =TITLE
//1002 = CHAPTER
//1000 = autre
k9LvItemImport::k9LvItemImport( QTreeWidgetItem *parent,eObjectType _objectType)
            : QTreeWidgetItem( parent,_objectType) {
        m_objectType=_objectType;
        m_title=NULL;
        m_aviFile=NULL;
}



bool k9LvItemImport::operator< ( const QTreeWidgetItem & other ) const   {
// int col=treeWidget()->sortColumn();

 if (other.type()== TITLE) {
     k9LvItemImport *item2=(k9LvItemImport*)&other;
     return   this->getTitle()->getNum() < item2->getTitle()->getNum();
 }
 if (other.type()== CHAPTER) {
     k9LvItemImport *item2=(k9LvItemImport*)&other;
     return  this->getAviFile()->getNum() < item2->getAviFile()->getNum()  ;
 }
 return false;
}


void k9LvItemImport::setTitle(k9Title* _value) {
    m_title = _value;
    QPixmap pix=QPixmap::fromImage(m_title->getButton()->getImage());
    setIcon(0,QIcon(pix));
}

k9LvItemImport::~k9LvItemImport() {
}
