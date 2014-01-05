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

#include "k9astdvdoption.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include <QDebug>
#include "k9titlefactor.h"

class _k9TitleListItem : public QListWidgetItem {
public:
   _k9TitleListItem ( QListWidget *parent):QListWidgetItem(parent) {};
   k9DVDTitle *title;

};


k9astDVDOption::k9astDVDOption(k9Assistant* parent,k9DVD *_dvd)
        : QWidget(parent) {
    Ui_astDVDOption.setupUi(this);
    m_parent=parent;
    m_dvd=_dvd;
    m_titles=Ui_astDVDOption.titles;
    k9Config config;
    Ui_astDVDOption.rbMenu->setChecked(config.getKeepMenus());
    Ui_astDVDOption.rbDvdauthor->setChecked(!config.getKeepMenus());
}

k9astDVDOption::~k9astDVDOption() {
}

void k9astDVDOption::clear()  {
    m_titles->clear();
}
void k9astDVDOption::addTitle(k9DVDTitle *_title) {
   _k9TitleListItem *item=new _k9TitleListItem(m_titles);
   item->title=_title;
   item->setText(_title->getname());
}

void k9astDVDOption::removeTitle(k9DVDTitle *_title) {
   for (int i=0;i <m_titles->count();i++) {
	_k9TitleListItem *item=(_k9TitleListItem*)m_titles->item(i);
	if (item->title==_title) {
	   m_titles->takeItem(i);
	   break;
	}
   }


}

void k9astDVDOption::updateOptions() {
   k9DVDTitle *title=NULL;
   m_parent->setCopyMenus(Ui_astDVDOption.rbMenu->isChecked());
   for (int i=0;i <m_titles->count();i++) {
	_k9TitleListItem *item=(_k9TitleListItem*)m_titles->item(i);
	qDebug () << "title :" <<item->title->getname();
	if (i==0) {
	   m_dvd->setstart(item->title);
	} else {
	   title->setnextTitle(item->title);
        }
	title=item->title;
        item->title->setnextTitle(NULL);
   }

}


void k9astDVDOption::rbMenuToggled(bool _state) {
  Ui_astDVDOption.gbTitles->setEnabled(!_state);
}
/*$SPECIALIZATION$*/

