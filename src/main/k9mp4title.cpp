//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9common.h"
#include "k9mp4title.h"
#include "k9titleencopt.h"
#include "k9dvdtitle.h"
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qframe.h>
#include <qtabwidget.h>
#include <qradiobutton.h>
#include "k9audiocodecs.h"
#include "k9videocodecs.h"
#include "k9tools.h"
#include "k9cropselect.h"
#include <kdialog.h>

k9MP4Title::k9MP4Title(QWidget* parent,eOptTitle options)
: QWidget(parent),m_options(options)
{
    m_updating=false;
    Ui_prefMPEG4.setupUi(this);
    m_titleEncOpt=NULL;
    load();
    Ui_prefMPEG4.ckUseCache->hide();
    Ui_prefMPEG4.tabWidget->setEnabled(false);
    Ui_prefMPEG4.lTitle->setText(i18n("Select a Title in the treeview..."));
    if (options == optAudio)
	Ui_prefMPEG4.tabWidget->setTabEnabled(0,false);
}


void k9MP4Title::setOptions (eOptTitle _options) {
  switch(_options) {
    case optAudio:
	Ui_prefMPEG4.tabWidget->setTabEnabled(0,false);
	Ui_prefMPEG4.tabWidget->setTabEnabled(1,true);
	break;
    case optVideo:
	Ui_prefMPEG4.tabWidget->setTabEnabled(0,true);
	Ui_prefMPEG4.tabWidget->setTabEnabled(1,false);
	break;
    case optAll:
	Ui_prefMPEG4.tabWidget->setTabEnabled(0,true);
	Ui_prefMPEG4.tabWidget->setTabEnabled(1,true);
	break;
  }
  m_options=_options;

}

k9MP4Title::~k9MP4Title()
{
}

void k9MP4Title::bCropClicked() {
   KDialog dialog(this);
   dialog.setWindowTitle(KDialog::makeStandardCaption(i18n("Crop detection")));
   k9CropSelect cropSelect(&dialog);
   QRect r;
   r.setCoords(Ui_prefMPEG4.sbLeft->value(),Ui_prefMPEG4.sbTop->value(),Ui_prefMPEG4.sbRight->value(),Ui_prefMPEG4.sbBottom->value());
   cropSelect.setRect(r);
   cropSelect.setMinimumWidth(600);
   cropSelect.setMinimumHeight(400);
   dialog.setMainWidget(&cropSelect);
   cropSelect.open(m_dvd,m_titleEncOpt->getTitle(),1);
   if (dialog.exec()== QDialog::Accepted) {
	QRect r= cropSelect.getRect();
	Ui_prefMPEG4.sbTop->setValue(r.top());
	Ui_prefMPEG4.sbLeft->setValue(r.left());
	Ui_prefMPEG4.sbRight->setValue(r.right());
	Ui_prefMPEG4.sbBottom->setValue(r.bottom());
	if (m_titleEncOpt) {
	    m_titleEncOpt->getCrop()->setCoords(r.left(),r.top(),r.right(),r.bottom());
	}
   }
}

void k9MP4Title::load() {
    k9Config config;
    
    Ui_prefMPEG4.sbMp4Size->setValue(config.getPrefMp4Size());
    Ui_prefMPEG4.sbMp4Size->setSuffix(" "+ i18n("MB"));
    Ui_prefMPEG4.sbMp4NumberCD->setValue(config.getPrefMp4NumberCD());
    Ui_prefMPEG4.sbAudioGain->setValue(config.getPrefMp4AudioGain());

    
    Ui_prefMPEG4.leMp4Width->setText(config.getPrefMp4Width());
    Ui_prefMPEG4.leMp4Height->setText(config.getPrefMp4Height());

    Ui_prefMPEG4.ckMp4AspectRatio->setChecked(config.getPrefMp4AspectRatio());
    Ui_prefMPEG4.leMp4Height->setEnabled(!Ui_prefMPEG4.ckMp4AspectRatio->isChecked());

    Ui_prefMPEG4.ck2passes->setChecked(config.getPrefMp42Passes());
    Ui_prefMPEG4.ckUseCache->setChecked(config.getPrefUseCellCache());

    Ui_prefMPEG4.leMp4AudioBitrate->setText(config.getPrefMp4AudioBitrate());
    Ui_prefMPEG4.leMp4VideoBitrate->setText(config.getPrefMp4VideoBitrate());

    loadCodecs(QStringList(),QStringList());
    
    Ui_prefMPEG4.cbMp4AudioCodec->setCurrentIndex(Ui_prefMPEG4.cbMp4AudioCodec->findData(config.getPrefMp4AudioCodec()));
    Ui_prefMPEG4.cbMp4Codec->setCurrentIndex(Ui_prefMPEG4.cbMp4Codec->findData(config.getPrefMp4Codec()));

    if(config.getPrefMp4VideoBitrate() =="") 
	Ui_prefMPEG4.rbSize->setChecked(true);
    else
	Ui_prefMPEG4.rbBitrate->setChecked(true);

}


void k9MP4Title::loadCodecs(QStringList _audioEncoders,QStringList _videoEncoders) {
    k9AudioCodecs audioCodecs;
    k9VideoCodecs videoCodecs;

    Ui_prefMPEG4.cbMp4AudioCodec->clear();
    Ui_prefMPEG4.cbMp4Codec->clear();
    Ui_prefMPEG4.cbEncoder->clear();

    for (int i=0;i <videoCodecs.count();i++) 
        if (_videoEncoders.contains(videoCodecs.getEncoder(i)) || (_videoEncoders.count()==0)) {
	    if (Ui_prefMPEG4.cbEncoder->findText(videoCodecs.getEncoder(i))==-1)
		Ui_prefMPEG4.cbEncoder->addItem(videoCodecs.getEncoder(i));
	}
     
    for (int i=0; i <audioCodecs.count();i++)
	if (_audioEncoders.contains(audioCodecs.getEncoder(i)) || (_audioEncoders.count()==0)) {
	    if (Ui_prefMPEG4.cbEncoder->findText(audioCodecs.getEncoder(i))==-1)
		Ui_prefMPEG4.cbEncoder->addItem(audioCodecs.getEncoder(i));

    }
    cbEncoderActivated(Ui_prefMPEG4.cbEncoder->currentIndex());
}

/*$SPECIALIZATION$*/
void k9MP4Title::ckMp4AspectRatioClick()
{
   Ui_prefMPEG4.leMp4Height->setEnabled(!Ui_prefMPEG4.ckMp4AspectRatio->isChecked());
   if (m_titleEncOpt)
   	m_titleEncOpt->setKeepAspectRatio(Ui_prefMPEG4.ckMp4AspectRatio->isChecked());

}

void k9MP4Title::selectionChanged(k9DVD *_dvd,bool) {
   if (_dvd==NULL)
	m_titleEncOpt=NULL;
   if (m_titleEncOpt)
   	Ui_prefMPEG4.tabWidget->setEnabled(m_titleEncOpt->getTitle()->isSelected());;
   m_dvd=_dvd;

}


void k9MP4Title::titleChanged( k9DVDTitle *_title) {
    if (_title)
        m_dvd=_title->getDvd();
    m_updating=true;
    if (_title==NULL) {
    	m_titleEncOpt=NULL;
    	Ui_prefMPEG4.tabWidget->setEnabled(false);
    	Ui_prefMPEG4.lTitle->setText(i18n("Select a Title in the treeview..."));
    }
    else {
        Ui_prefMPEG4.lTitle->setText(_title->getname());
    	Ui_prefMPEG4.tabWidget->setEnabled( _title->isSelected());
	m_titleEncOpt=_title->getEncOpt();
	k9TitleEncOpt *opt=m_titleEncOpt;
	Ui_prefMPEG4.sbMp4Size->setValue(opt->getMaxSize());
	Ui_prefMPEG4.sbMp4NumberCD->setValue(opt->getNumParts() );
	Ui_prefMPEG4.sbAudioGain->setValue(opt->getAudioGain() );
	
	Ui_prefMPEG4.leMp4Width->setText(opt->getWidth() );
	Ui_prefMPEG4.leMp4Height->setText(opt->getHeight());
	
	Ui_prefMPEG4.ckMp4AspectRatio->setChecked(opt->getKeepAspectRatio());
	Ui_prefMPEG4.leMp4Height->setEnabled(!Ui_prefMPEG4.ckMp4AspectRatio->isChecked());
	
	Ui_prefMPEG4.ck2passes->setChecked(opt->get2Passes());
        Ui_prefMPEG4.ckUseCache->setChecked(opt->getUseCache());	

	Ui_prefMPEG4.leMp4AudioBitrate->setText(opt->getAudioBr());
	Ui_prefMPEG4.leMp4VideoBitrate->setText(opt->getVideoBr());

	for (int i=0; i<Ui_prefMPEG4.cbEncoder->count();i++) {
		if (Ui_prefMPEG4.cbEncoder->itemText(i)==opt->getEncoder())
			Ui_prefMPEG4.cbEncoder->setCurrentIndex(i);
	}

        cbEncoderActivated(Ui_prefMPEG4.cbEncoder->currentIndex());
	Ui_prefMPEG4.cbMp4Codec->setCurrentIndex( Ui_prefMPEG4.cbMp4Codec->findData(opt->getCodec() ));   
        Ui_prefMPEG4.cbMp4AudioCodec->setCurrentIndex(Ui_prefMPEG4.cbMp4AudioCodec->findData(opt->getAudioCodec()));

	if(opt->getVideoBr() =="") 
		Ui_prefMPEG4.rbSize->setChecked(true);
	else
		Ui_prefMPEG4.rbBitrate->setChecked(true);
 	Ui_prefMPEG4.sbTop->setValue(opt->getCrop()->top());
	Ui_prefMPEG4.sbLeft->setValue(opt->getCrop()->left());
	Ui_prefMPEG4.sbRight->setValue(opt->getCrop()->right());
	Ui_prefMPEG4.sbBottom->setValue(opt->getCrop()->bottom());

        Ui_prefMPEG4.sbTop->setMaximum(_title->getheight().toInt()/2);
	Ui_prefMPEG4.sbLeft->setMaximum(_title->getwidth().toInt()/2);
	Ui_prefMPEG4.sbRight->setMaximum(_title->getwidth().toInt()-1);
	Ui_prefMPEG4.sbBottom->setMaximum(_title->getheight().toInt()-1);
	Ui_prefMPEG4.sbRight->setMinimum(_title->getwidth().toInt()/2);
	Ui_prefMPEG4.sbBottom->setMinimum(_title->getheight().toInt()/2);
 	testFields();

    }

   m_updating=false;
}

void k9MP4Title::testFields() {
    if (m_titleEncOpt) {
	int index= k9VideoCodecs::getAbsCodecNum(Ui_prefMPEG4.cbMp4Codec->itemData(Ui_prefMPEG4.cbMp4Codec->currentIndex()).toInt(), m_titleEncOpt->getEncoder()) ;
	k9VideoCodecs videoCodecs;
	QString sOptions;
	if (m_titleEncOpt->get2Passes())
		sOptions=videoCodecs.getOptions1(index);
	else
		sOptions=videoCodecs.getOptions0(index);
	m_videoOptions=sOptions;
	Ui_prefMPEG4.leMp4Width->setEnabled(sOptions.contains("$WIDTH"));
	Ui_prefMPEG4.ckMp4AspectRatio->setEnabled(sOptions.contains("$HEIGHT"));
	if (sOptions.contains("$VIDBR")) {
		Ui_prefMPEG4.leMp4VideoBitrate->setEnabled(m_titleEncOpt->getVideoBr() !="");
		Ui_prefMPEG4.sbMp4Size->setEnabled( m_titleEncOpt->getVideoBr() =="");
	} else {
		Ui_prefMPEG4.leMp4VideoBitrate->setEnabled(false);
		Ui_prefMPEG4.sbMp4Size->setEnabled( false);

 	}


	Ui_prefMPEG4.sbMp4NumberCD->setEnabled(sOptions.contains("$VIDBR"));
        Ui_prefMPEG4.rbSize->setEnabled(sOptions.contains("$VIDBR"));
	Ui_prefMPEG4.rbBitrate->setEnabled(sOptions.contains("$VIDBR"));
	Ui_prefMPEG4.leMp4Height->setEnabled(sOptions.contains("$HEIGHT") && !Ui_prefMPEG4.ckMp4AspectRatio->isChecked());
	Ui_prefMPEG4.sbTop->setEnabled(sOptions.contains("$CROPTOP") || sOptions.contains("$CROPHEIGHT"));
	Ui_prefMPEG4.sbLeft->setEnabled(sOptions.contains("$CROPLEFT") || sOptions.contains("$CROPWIDTH"));
	Ui_prefMPEG4.sbRight->setEnabled(sOptions.contains("$CROPRIGHT") || sOptions.contains("CROPWIDTH"));
	Ui_prefMPEG4.sbBottom->setEnabled(sOptions.contains("$CROPBOTTOM") || sOptions.contains("$CROPHEIGHT"));
	  
	index=k9AudioCodecs::getAbsCodecNum(Ui_prefMPEG4.cbMp4AudioCodec->itemData(Ui_prefMPEG4.cbMp4AudioCodec->currentIndex()).toInt(),m_titleEncOpt->getEncoder());
	k9AudioCodecs audioCodecs;
	sOptions=audioCodecs.getOptions(index);
 	m_audioOptions=sOptions;
	Ui_prefMPEG4.leMp4AudioBitrate->setEnabled(sOptions.contains("$AUDBR"));

    }

}


void k9MP4Title::cbMp4CodecActivated( int _value) {
   if (m_titleEncOpt) 
	m_titleEncOpt->setCodec(Ui_prefMPEG4.cbMp4Codec->itemData(_value).toInt());
   testFields();
}

void k9MP4Title::cbMp4AudioCodecActivated( int _value) {
   if (m_titleEncOpt)
   	m_titleEncOpt->setAudioCodec(Ui_prefMPEG4.cbMp4AudioCodec->itemData(_value).toInt());
   testFields();
}

void k9MP4Title::ck2passesClicked() {
   if (m_titleEncOpt)
   	m_titleEncOpt->set2Passes(Ui_prefMPEG4.ck2passes->isChecked());
}

void k9MP4Title::ckUseCacheClick() {
   if (m_titleEncOpt)
   	m_titleEncOpt->setUseCache(Ui_prefMPEG4.ckUseCache->isChecked());
}

void k9MP4Title::sbMp4SizeValueChanged( int _value) {
   if (m_titleEncOpt)   
   	m_titleEncOpt->setMaxSize(_value);
}

void k9MP4Title::sbMp4NumberCDValueChanged( int _value ) {
   if (m_titleEncOpt)   	
   	m_titleEncOpt->setNumParts(_value);
}

void k9MP4Title::sbAudioGainValueChanged( int _value ) {
   if (m_titleEncOpt)   	
   	m_titleEncOpt->setAudioGain(_value);
}

void k9MP4Title::leMp4WidthTextChanged( const QString & _value) {
   if (m_titleEncOpt)
	m_titleEncOpt->setWidth(_value);
}

void k9MP4Title::leMp4HeightTextChanged( const QString & _value) {
   if (m_titleEncOpt)
        m_titleEncOpt->setHeight(_value);
}

void k9MP4Title::leMp4AudioBitrateTextChanged( const QString & _value) {
   if (m_titleEncOpt)
	m_titleEncOpt->setAudioBr(_value);
}


void k9MP4Title::leMp4VideoBitrateTextChanged( const QString & _value) {
   if (m_titleEncOpt)
	m_titleEncOpt->setVideoBr(_value);
}

void k9MP4Title::sbValueChanged(int) {
   if (m_titleEncOpt && !m_updating) {
	m_titleEncOpt->getCrop()->setCoords(Ui_prefMPEG4.sbLeft->value(),Ui_prefMPEG4.sbTop->value(),Ui_prefMPEG4.sbRight->value(),Ui_prefMPEG4.sbBottom->value());

   }
}

void k9MP4Title::rgVideoSizeChanged(){
   if (Ui_prefMPEG4.rbSize->isChecked()) 
	Ui_prefMPEG4.leMp4VideoBitrate->setText("");
   Ui_prefMPEG4.leMp4VideoBitrate->setEnabled(Ui_prefMPEG4.rbBitrate->isChecked() && m_videoOptions.contains("$VIDBR"));
   Ui_prefMPEG4.sbMp4Size->setEnabled(Ui_prefMPEG4.rbSize->isChecked() && m_videoOptions.contains("$VIDBR"));
}

void k9MP4Title::cbEncoderActivated(int _index) {
    k9AudioCodecs audioCodecs;
    k9VideoCodecs videoCodecs;

    if (m_titleEncOpt)
	m_titleEncOpt->setEncoder(Ui_prefMPEG4.cbEncoder->currentText());

    Ui_prefMPEG4.cbMp4AudioCodec->clear();
    Ui_prefMPEG4.cbMp4Codec->clear();

    int cpt=0;
    for (int i=0;i <videoCodecs.count();i++) 
        if (videoCodecs.getEncoder(i)==Ui_prefMPEG4.cbEncoder->itemText(_index)) {
	    Ui_prefMPEG4.cbMp4Codec->addItem(videoCodecs.getCodecName(i),cpt++);
	}
    cpt=0;
    for (int i=0; i <audioCodecs.count();i++)
	if (audioCodecs.getEncoder(i)==Ui_prefMPEG4.cbEncoder->itemText(_index)) {
	    Ui_prefMPEG4.cbMp4AudioCodec->addItem(audioCodecs.getCodecName(i),cpt++);
    }

    if (m_titleEncOpt) {
	int index=qMax(0,Ui_prefMPEG4.cbMp4Codec->findData(m_titleEncOpt->getCodec() ));
	Ui_prefMPEG4.cbMp4Codec->setCurrentIndex(index );
	index=qMax(0,Ui_prefMPEG4.cbMp4AudioCodec->findData(m_titleEncOpt->getAudioCodec()));
        Ui_prefMPEG4.cbMp4AudioCodec->setCurrentIndex(index);
	
    } else {
        Ui_prefMPEG4.cbMp4Codec->setCurrentIndex(0);
        Ui_prefMPEG4.cbMp4AudioCodec->setCurrentIndex(0);
    }
    cbMp4CodecActivated(Ui_prefMPEG4.cbMp4Codec->currentIndex());

    cbMp4AudioCodecActivated(Ui_prefMPEG4.cbMp4AudioCodec->currentIndex());
    emit updateSelection();
}
