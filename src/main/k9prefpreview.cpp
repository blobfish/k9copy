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


#include "k9prefpreview.h"

k9prefPreview::k9prefPreview(QWidget* parent)
: QWidget(parent)
{ 
   Ui_prefPreview.setupUi(this);
   load();
}

k9prefPreview::~k9prefPreview()
{
}

/*$SPECIALIZATION$*/

void k9prefPreview::load() {
    k9Config config;

#ifdef BACKLITE
    Ui_prefPreview.rbXine->hide();
#endif
    Ui_prefPreview.cbVout->setCurrentIndex(config.getMplayerVout());
    Ui_prefPreview.cbAout->setCurrentIndex(config.getMplayerAout());
    Ui_prefPreview.rbMplayer->setChecked(config.getUseMplayer());
    Ui_prefPreview.cbXVout->setCurrentIndex(config.getXineVout());
    Ui_prefPreview.cbXAout->setCurrentIndex(config.getXineAout());
    Ui_prefPreview.rbXine->setChecked(config.getUseXine());
    Ui_prefPreview.rbPhonon->setChecked(config.getUsePhonon());
    Ui_prefPreview.rbInternal->setChecked(!config.getUseMplayer() && !config.getUseXine() && ! config.getUsePhonon());
    
    if (Ui_prefPreview.rbMplayer->isChecked()) 
	rbMplayerClick();
    else if (Ui_prefPreview.rbXine->isChecked()) 
	rbXineClick();
    else if (Ui_prefPreview.rbPhonon->isChecked()) 
	rbPhononClick();
    else
	rbInternalClick();

}



void k9prefPreview::save() {
    k9Config config;

    if (Ui_prefPreview.rbMplayer->isChecked()) {
        config.setMplayerVout( Ui_prefPreview.cbVout->currentIndex());
        config.setMplayerAout( Ui_prefPreview.cbAout->currentIndex());	
    }else if (Ui_prefPreview.rbXine->isChecked()) {
        config.setXineVout( Ui_prefPreview.cbXVout->currentIndex());
        config.setXineAout( Ui_prefPreview.cbXAout->currentIndex());	
    }else if (Ui_prefPreview.rbPhonon->isChecked()) {
    }
    config.setUseMplayer( Ui_prefPreview.rbMplayer->isChecked());
    config.setUseXine( Ui_prefPreview.rbXine->isChecked());
    config.setUsePhonon( Ui_prefPreview.rbPhonon->isChecked());

    config.save();
}


void k9prefPreview::rbPhononClick() {
  Ui_prefPreview.wsOptions->setCurrentIndex(3);
}

void k9prefPreview::rbInternalClick() {
  Ui_prefPreview.wsOptions->setCurrentIndex(0);
}

void k9prefPreview::rbMplayerClick() {
  Ui_prefPreview.wsOptions->setCurrentIndex(1);
}

void k9prefPreview::rbXineClick() {
  Ui_prefPreview.wsOptions->setCurrentIndex(2);
}


