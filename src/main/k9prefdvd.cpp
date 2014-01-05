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


#include "k9prefdvd.h"
#include <kurlrequester.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include "k9tools.h"
#include "k9dvd.h"
#include <QListWidgetItem>
k9prefDVD::k9prefDVD(QWidget* parent)
: QWidget(parent)
{
    m_loading=true;
    Ui_prefDVD.setupUi(this);
    k9Config config;
#ifdef BACKLITE
    Ui_prefDVD.urOutput->setMode(KUrlRequester::DIRECTORY);
    Ui_prefDVD.gbPerformances->hide();
#else
    Ui_prefDVD.urOutput->setMode(KFile::Directory);
#endif
    Ui_prefDVD.urOutput->setUrl(KUrl(config.getPrefOutput()));
    Ui_prefDVD.ckK3b->setChecked(config.getPrefK3b());

    Ui_prefDVD.ckAutoBurn->setChecked(config.getPrefAutoBurn());
    Ui_prefDVD.ckQuickScan->setChecked(config.getQuickScan());
    Ui_prefDVD.ckDvdAuthor->setChecked(config.getUseDvdAuthor());
    Ui_prefDVD.ckDelTmpFiles->setChecked(config.getPrefDelTmpFiles());
    Ui_prefDVD.ckMpegChapter->setChecked(config.getMpegChapters());
    Ui_prefDVD.ckUserOps->setChecked(config.getPrefUserOps());
    Ui_prefDVD.sbSize->setValue(config.getPrefSize());
    Ui_prefDVD.sbSize->setSuffix(" "+ i18n("MB"));

    loadLanguages(Ui_prefDVD.lwAudioLang,config.getPrefAudioLang());
    loadLanguages(Ui_prefDVD.lwSubtitlesLang,config.getPrefSubtitleLang());

    switch(config.getPrefReadAhead()) {
	case 0  :
	 	Ui_prefDVD.cbReadAhead->setCurrentIndex(1);
		break;
	case 8  :
	 	Ui_prefDVD.cbReadAhead->setCurrentIndex(2);
		break;
	default:
	 	Ui_prefDVD.cbReadAhead->setCurrentIndex(0);
		break;
    }
    m_loading=false;
}

void k9prefDVD::loadLanguages(QListWidget *_list,const QStringList &_prefered) {
    m_loading=true;
    lng arrLng[200];
    k9DVD::LangList(arrLng);
    int index=0;
    _list->clear();
    //selected languages on top of the list
    foreach (QString lang, _prefered) {
        lang=lang.mid(0,2);
        QListWidgetItem *item=new QListWidgetItem(_list);
        item->setData(Qt::UserRole,QString(lang));
        item->setText(QString("%1 - %2").arg(lang).arg(k9DVD::lang_name(arrLng,lang,"")));
        item->setCheckState(Qt::Checked);
        _list->addItem(item);
    }
    do {
        if (!_prefered.contains(QString(arrLng[index].code)) && (QString(arrLng[index].code).trimmed()!="")) {
            QListWidgetItem *item=new QListWidgetItem(_list);
            item->setData(Qt::UserRole,QString(arrLng[index].code));
            item->setText(QString("%1 - %2").arg(arrLng[index].code).arg(arrLng[index].name));
            item->setCheckState(Qt::Unchecked);
            _list->addItem(item);

        }
        index++;
    } while (arrLng[index].code[0]!=0);
    m_loading=false;
}


k9prefDVD::~k9prefDVD()
{
}

/*$SPECIALIZATION$*/

void k9prefDVD::save() {
    k9Config config;
    config.setPrefOutput( Ui_prefDVD.urOutput->url().path());
    config.setPrefK3b(Ui_prefDVD.ckK3b->isChecked());
    config.setPrefAutoBurn( Ui_prefDVD.ckAutoBurn->isChecked());
    config.setPrefSize(Ui_prefDVD.sbSize->value());
    config.setQuickScan( Ui_prefDVD.ckQuickScan->isChecked());
    config.setUseDvdAuthor( Ui_prefDVD.ckDvdAuthor->isChecked());    
    config.setPrefDelTmpFiles(Ui_prefDVD.ckDelTmpFiles->isChecked());
    config.setMpegChapters(Ui_prefDVD.ckMpegChapter->isChecked());
    config.setPrefUserOps(Ui_prefDVD.ckUserOps->isChecked());
    QStringList lang;
    for (int row=0;row <Ui_prefDVD.lwAudioLang->count();row++) {
        QListWidgetItem *item=Ui_prefDVD.lwAudioLang->item(row);
        if (item->checkState()==Qt::Checked)
            lang << item->data(Qt::UserRole).toString();
    }

    config.setPrefAudioLang(lang);
    lang.clear();
    for (int row=0;row <Ui_prefDVD.lwSubtitlesLang->count();row++) {
        QListWidgetItem *item=Ui_prefDVD.lwSubtitlesLang->item(row);
        if (item->checkState()==Qt::Checked)
            lang << item->data(Qt::UserRole).toString();
    }
    config.setPrefSubtitleLang(lang);
    switch(Ui_prefDVD.cbReadAhead->currentIndex()) {
	case 0  :
	 	config.setPrefReadAhead(-1);
		break;
	case 1  :
	 	config.setPrefReadAhead(0);
		break;
	case 2:
	 	config.setPrefReadAhead(8);
		break;
    }

    config.save();
}

void k9prefDVD::lwAudioLangChanged(QListWidgetItem *item) {
    if (!m_loading) {
    }
}

void k9prefDVD::lwSubtitleLangChanged(QListWidgetItem *item) {
    if (!m_loading) {
    }
}
