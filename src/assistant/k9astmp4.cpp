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

#include "k9astmp4.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include <QDebug>
#include "k9mp4title.h"


k9astMp4::k9astMp4(k9Assistant* parent,k9DVD *_dvd)
        : QWidget(parent) {
    Ui_astMp4.setupUi(this);
    m_parent=parent;
    m_dvd=_dvd;
    m_mp4Title =new k9MP4Title(Ui_astMp4.options);
    connect (this,SIGNAL(titleChanged(k9DVDTitle*)),m_mp4Title,SLOT(titleChanged(k9DVDTitle*)));
    cbTitle=Ui_astMp4.cbTitle;
    QGridLayout *grid = new QGridLayout(Ui_astMp4.options);
    grid->addWidget(m_mp4Title,0,0);

}

k9astMp4::~k9astMp4() {
}

void k9astMp4::setMatroskaCodecs() {
   QStringList codecs;
   codecs << "ffmpeg";
   m_mp4Title->loadCodecs(codecs,codecs);
}

void k9astMp4::setAviCodecs() {
  m_mp4Title->loadCodecs(QStringList(),QStringList());

}

void k9astMp4::setAudio() {
   m_mp4Title->setOptions(k9MP4Title::optAudio);
   QStringList codecs;
   codecs <<"ffmpeg";
   m_mp4Title->loadCodecs(codecs,codecs);
}

void k9astMp4::addTitle(k9DVDTitle *_title) {
   m_titles.append(_title);
   if (cbTitle->count()==1) {
 	if (cbTitle->itemData(0,Qt::UserRole).toInt()==0) {
	    cbTitle->setItemText(0,_title->getname());
	    cbTitle->setItemData(0,1,Qt::UserRole); 
	    emit titleChanged(_title);
	    return;
	}
   }
   cbTitle->addItem(_title->getname(),1);
}

void k9astMp4::removeTitle(k9DVDTitle *_title) {
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

void k9astMp4::cbTitleChanged(int _index) {
  	emit titleChanged(m_titles.at(_index));


}

/*$SPECIALIZATION$*/

