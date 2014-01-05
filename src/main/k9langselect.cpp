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


#include "k9langselect.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include "k9main.h"
k9LangSelect::k9LangSelect(k9Main *mainWindow,QWidget* parent)
: QWidget(parent)
{
    Ui_langSelect.setupUi(this);
    setMain( mainWindow);
    m_main->setLangSelect( this);

}

k9LangSelect::~k9LangSelect()
{
}

/*$SPECIALIZATION$*/

void ckLvLangItem::stateChange(bool state) {
    if (mainDlg->getupdating())
        return;
    mainDlg->checkLang(language,streamType,state);
    mainDlg->updateSelection();
//    k9DVD *d = mainDlg->dvd;
    mainDlg->setDVDSize();
    mainDlg->updateFactor();
    //(mainDlg->factor)->setValue((int) (d->getfactor(mainDlg->withMenus(),true )*100)-100 );
}

void k9LangSelect::listViewItemChanged(QTreeWidgetItem *_item,int _col) {
    if (_item->type()==1001 && _col==0) {
	ckLvLangItem *it=(ckLvLangItem*)_item;
	it->stateChange(it->isOn());
    }
}



void k9LangSelect::setMain(k9Main* _value) {
    m_main = _value;
    m_dvd=m_main->dvd;
}

void k9LangSelect::fillLvLanguages() {
    Ui_langSelect.lvLanguages->clear();
    langAudItems.clear();
    langSubItems.clear();
    QStringList slAudLang,slSubLang;
    if (m_dvd->getopened()) {
        k9DVDSubtitle *l_sub;
        k9DVDAudioStream *l_auds;
	QList <k9DVDListItem*> *items=m_main->getItems();
        for (int i=0;i<items->count();i++) {
            k9DVDListItem *litem=(k9DVDListItem*)items->at(i);
            switch (litem->streamType) {
            case SUB: {
                    l_sub=litem->subtitle;
                    if (slSubLang.contains(l_sub->getlanguage())==0)
                        slSubLang.append(l_sub->getlanguage());
                    break;
                }
            case AUD: {
                    l_auds=litem->audioStream;
                    if (slAudLang.contains(l_auds->getlanguage())==0)
                        slAudLang.append(l_auds->getlanguage());
                    break;
                }
	    default:
		break;
            }
        }
    }
    QTreeWidgetItem *audioRoot = new QTreeWidgetItem(Ui_langSelect.lvLanguages);
    audioRoot->setExpanded( TRUE );
    audioRoot->setText(0, i18n("Audio"));
    foreach ( QString audLang,slAudLang) {
        ckLvLangItem *lvitem=new ckLvLangItem(audioRoot,m_main);
        lvitem->setText(0,audLang);
        lvitem->streamType=AUD;
        lvitem->language=audLang;
        langAudItems.append(lvitem);
    }

    QTreeWidgetItem *subRoot = new QTreeWidgetItem(Ui_langSelect.lvLanguages);
    subRoot->setExpanded( TRUE );
    subRoot->setText(0, i18n("Subtitles"));
    foreach ( QString subLang, slSubLang) {
        ckLvLangItem *lvitem=new ckLvLangItem(subRoot,m_main);
        lvitem->setText(0,subLang);
        lvitem->streamType=SUB;
        lvitem->language=subLang;
        langSubItems.append(lvitem);
    }
  connect(Ui_langSelect.lvLanguages,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(listViewItemChanged(QTreeWidgetItem*,int)));

}



void k9LangSelect::update() {
    for (int i=0;i<langAudItems.count();i++)
        updateLvLang(AUD,langAudItems.at(i)->language);
    for (int i=0;i<langSubItems.count();i++)
        updateLvLang(SUB,langSubItems.at(i)->language);
}


void k9LangSelect::updateLvLang(const eStreamType streamType,const QString & lang ) {
    uint Total=0,Selected=0;
    QString lg;
    QList<k9DVDListItem*> *items = m_main->getItems();
    for (int i=0;i<items->count();i++) {
        k9DVDListItem *litem=(k9DVDListItem*)items->at(i);
        if (litem->streamType==streamType ) {
            switch (streamType) {
            case SUB :
                lg= litem->subtitle->getlanguage();
                if (lg==lang) {
                    Total++;
                    if (litem->listItem->isOn())
                        Selected ++;
                }
                break;
            case AUD :
                lg=litem->audioStream->getlanguage();
                if (lg==lang) {
                    Total++;
                    if (litem->listItem->isOn())
                        Selected++;
                }
                break;
	    default:
		break;
            }
        }
    }
    if (streamType==AUD) {
        foreach (ckLvLangItem *langItem,langAudItems) {
            if (langItem->language ==lang) {
                if (Selected==Total) {
                    langItem->setOn(true);
                } else
                    langItem->setOn(false);
            }
        }
    }
    if (streamType==SUB) {
        foreach (ckLvLangItem *langItem, langSubItems){
            if (langItem->language ==lang) {
                if (Selected==Total) {
                    langItem->setOn(true);
                } else
                    langItem->setOn(false);
            }
        }
    }

}

void k9LangSelect::selectAll( bool _state) {
    foreach  (ckLvLangItem *langItem,langAudItems){
        langItem->setOn(_state);
    }
    foreach (ckLvLangItem *langItem,langSubItems){
        langItem->setOn(_state);
    }
}

void k9LangSelect::clear() {
    langAudItems.clear();
    langSubItems.clear(); 
    Ui_langSelect.lvLanguages->clear();
}
