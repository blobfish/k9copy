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

#include "k9prefauthor.h"
#include "k9tools.h"
#include <qcombobox.h>
#include <kcolorbutton.h>

k9prefAuthor::k9prefAuthor(QWidget* parent)
: QWidget(parent)
{
    Ui_prefAuthor.setupUi(this);
    load();
}

void k9prefAuthor::load() {
    k9Config config;
    k9Tools::setComboText(Ui_prefAuthor.cbAudioFormat,config.getPrefAudioFormat());
    k9Tools::setComboText(Ui_prefAuthor.cbAudioBitrate,QString::number(config.getPrefAudioBitrate()));
    Ui_prefAuthor.inButtonWidth->setValue(config.getPrefButtonWidth());
    Ui_prefAuthor.inButtonHeight->setValue(config.getPrefButtonHeight());
    Ui_prefAuthor.cbHiliteColor->setColor(config.getPrefButtonHiliteColor());
    Ui_prefAuthor.cbTextColor->setColor(config.getPrefButtonTextColor());
    m_font=config.getPrefButtonFont();
}

k9prefAuthor::~k9prefAuthor()
{
}

/*$SPECIALIZATION$*/
void k9prefAuthor::bButtonFontClicked()
{
    k9Dialogs::getFont( m_font );
}

void k9prefAuthor::save() {
    k9Config config;
    config.setPrefAudioFormat(Ui_prefAuthor.cbAudioFormat->currentText());
    config.setPrefAudioBitrate(Ui_prefAuthor.cbAudioBitrate->currentText().toInt());
    config.setPrefButtonWidth(Ui_prefAuthor.inButtonWidth->value());
    config.setPrefButtonHeight(Ui_prefAuthor.inButtonHeight->value());
    config.setPrefButtonHiliteColor(Ui_prefAuthor.cbHiliteColor->color());
    config.setPrefButtonFont(m_font);
    config.setPrefButtonTextColor(Ui_prefAuthor.cbTextColor->color());
    config.save();
}

