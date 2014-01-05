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

#include "k9common.h"
#include "k9prefmpeg4.h"
#include <qcombobox.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qstringlist.h>
#include <qradiobutton.h>
#include "k9audiocodecs.h"
#include "k9videocodecs.h"

k9prefMPEG4::k9prefMPEG4(QWidget* parent)
: QWidget(parent)
{
    Ui_prefMPEG4.setupUi(this);
    Ui_prefMPEG4.wCrop->hide();
    load();
}

void k9prefMPEG4::load() {
    //KSimpleConfig settings("K9Copy");
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

    Ui_prefMPEG4.leMp4AudioBitrate->setText(config.getPrefMp4AudioBitrate());
    Ui_prefMPEG4.leMp4VideoBitrate->setText(config.getPrefMp4VideoBitrate());


    k9AudioCodecs audioCodecs;
    k9VideoCodecs videoCodecs;

    QStringList encoders;

    for (int i=0;i <videoCodecs.count();i++) {
	if (!encoders.contains(videoCodecs.getEncoder(i)))
	    encoders << videoCodecs.getEncoder(i);
    }    


    for (int i=0; i <audioCodecs.count();i++) {
	if (!encoders.contains(audioCodecs.getEncoder(i)))
	    encoders << audioCodecs.getEncoder(i);
    }

    Ui_prefMPEG4.cbEncoder->clear();
    Ui_prefMPEG4.cbEncoder->addItems(encoders);
    Ui_prefMPEG4.cbEncoder->setCurrentIndex(Ui_prefMPEG4.cbEncoder->findText(config.getPrefMp4Encoder()));
    cbEncoderActivated(Ui_prefMPEG4.cbEncoder->currentIndex());

    Ui_prefMPEG4.cbMp4Codec->setCurrentIndex(config.getPrefMp4Codec());

    Ui_prefMPEG4.cbMp4AudioCodec->setCurrentIndex(config.getPrefMp4AudioCodec());

    Ui_prefMPEG4.ckUseCache->setChecked(config.getPrefUseCellCache());

    if(config.getPrefMp4VideoBitrate() =="") 
	Ui_prefMPEG4.rbSize->setChecked(true);
    else
	Ui_prefMPEG4.rbBitrate->setChecked(true);
   Ui_prefMPEG4.leMp4VideoBitrate->setEnabled(Ui_prefMPEG4.rbBitrate->isChecked());
   Ui_prefMPEG4.sbMp4Size->setEnabled(Ui_prefMPEG4.rbSize->isChecked());

}


k9prefMPEG4::~k9prefMPEG4()
{
}

void k9prefMPEG4::save() {
    k9Config config;
    config.setPrefMp4Codec(Ui_prefMPEG4.cbMp4Codec->currentIndex());
    config.setPrefMp4AudioCodec(Ui_prefMPEG4.cbMp4AudioCodec->currentIndex());
    config.setPrefMp4Size( (int)Ui_prefMPEG4.sbMp4Size->value());
    config.setPrefMp4Encoder(Ui_prefMPEG4.cbEncoder->currentText());
    config.setPrefMp4NumberCD( (int)Ui_prefMPEG4.sbMp4NumberCD->value());
    config.setPrefMp4AudioGain( (int)Ui_prefMPEG4.sbAudioGain->value());
    config.setPrefMp4Width( Ui_prefMPEG4.leMp4Width->text());
    config.setPrefMp4Height(Ui_prefMPEG4.leMp4Height->text());
    config.setPrefMp4AspectRatio( Ui_prefMPEG4.ckMp4AspectRatio->isChecked());
    config.setPrefMp4AudioBitrate( Ui_prefMPEG4.leMp4AudioBitrate->text());
    config.setPrefMp4VideoBitrate( Ui_prefMPEG4.leMp4VideoBitrate->text());
    config.setPrefMp42Passes(Ui_prefMPEG4.ck2passes->isChecked());
    config.setPrefUseCellCache(Ui_prefMPEG4.ckUseCache->isChecked());
    config.save();
}

void k9prefMPEG4::ckMp4AspectRatioClick() {
    Ui_prefMPEG4.leMp4Height->setEnabled(!Ui_prefMPEG4.ckMp4AspectRatio->isChecked());
    if (Ui_prefMPEG4.ckMp4AspectRatio->isChecked()) Ui_prefMPEG4.leMp4Height->setText("");
}

void k9prefMPEG4::rgVideoSizeChanged(){
   if (Ui_prefMPEG4.rbSize->isChecked()) 
	Ui_prefMPEG4.leMp4VideoBitrate->setText("");
   Ui_prefMPEG4.leMp4VideoBitrate->setEnabled(Ui_prefMPEG4.rbBitrate->isChecked());
   Ui_prefMPEG4.sbMp4Size->setEnabled(Ui_prefMPEG4.rbSize->isChecked());

}

void k9prefMPEG4::cbEncoderActivated(int _index) {
    k9AudioCodecs audioCodecs;
    k9VideoCodecs videoCodecs;

    Ui_prefMPEG4.cbMp4AudioCodec->clear();
    Ui_prefMPEG4.cbMp4Codec->clear();

    for (int i=0;i <videoCodecs.count();i++) 
        if (videoCodecs.getEncoder(i)==Ui_prefMPEG4.cbEncoder->itemText(_index)) {
	    Ui_prefMPEG4.cbMp4Codec->addItem(videoCodecs.getCodecName(i),i);
	}
     
    for (int i=0; i <audioCodecs.count();i++)
	if (audioCodecs.getEncoder(i)==Ui_prefMPEG4.cbEncoder->itemText(_index)) {
	    Ui_prefMPEG4.cbMp4AudioCodec->addItem(audioCodecs.getCodecName(i),i);
    }

    Ui_prefMPEG4.cbMp4Codec->setCurrentIndex(0);
    cbMp4CodecActivated(0);

    Ui_prefMPEG4.cbMp4AudioCodec->setCurrentIndex(0);
    cbMp4AudioCodecActivated(0);

}

/*$SPECIALIZATION$*/

