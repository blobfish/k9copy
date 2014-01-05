//
// C++ Implementation:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9astmpeg.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include <QDebug>
#include "k9titlefactor.h"


k9astMpeg::k9astMpeg(k9Assistant* parent,k9DVD *_dvd)
        : QWidget(parent) {
    Ui_astMpeg.setupUi(this);
    m_parent=parent;
    m_dvd=_dvd;
    k9TitleFactor *titleFactor=new  k9TitleFactor(Ui_astMpeg.options);
    m_titleFactor=titleFactor;
    titleFactor->SelectionChanged(m_dvd,true);
    m_titleFactor=titleFactor;
    connect (this,SIGNAL(titleChanged(k9DVDTitle*)),titleFactor,SLOT(changedTitle(k9DVDTitle*)));
    cbTitle=Ui_astMpeg.cbTitle;
    QGridLayout *grid = new QGridLayout(Ui_astMpeg.options);
    grid->addWidget(titleFactor,0,0);

}

k9astMpeg::~k9astMpeg() {
}

void k9astMpeg::updateFactor() {
   m_titleFactor->SelectionChanged(m_dvd,m_parent->copyMenus());
}
void k9astMpeg::addTitle(k9DVDTitle *_title) {
   m_titles.append(_title);
   if (cbTitle->count()==1) {
 	if (cbTitle->itemData(0,Qt::UserRole).toInt()==0) {
	    cbTitle->setItemText(0,_title->getname());
	    cbTitle->setItemData(0,1,Qt::UserRole);
	    emit titleChanged(_title);
	    return;
	}
   }
   cbTitle->addItem(_title->getname(),1);}

void k9astMpeg::removeTitle(k9DVDTitle *_title) {
  int i=m_titles.indexOf(_title);

  if (i!=-1) {
	if (cbTitle->count() ==1) {
	     cbTitle->setItemData(0,0,Qt::UserRole);
 	     cbTitle->setItemText(0,"--");
	} else
 	     cbTitle->removeItem(i);
	m_titles.removeOne(_title);
  }
}

void k9astMpeg::cbTitleChanged(int _index) {
  emit titleChanged(m_titles.at(_index));


}

/*$SPECIALIZATION$*/

