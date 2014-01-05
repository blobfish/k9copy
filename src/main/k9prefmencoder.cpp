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


#include "k9prefmencoder.h"
#include <qtextedit.h>
#include "k9mencodercmdgen.h"
#include "k9ffmpegcmdgen.h"
#include <kpushbutton.h>
#ifndef BACKLITE
#include <kiconloader.h>
#endif
#include "k9tools.h"
k9prefMencoder::k9prefMencoder(QWidget* parent)
: QWidget(parent)
{
   Ui_prefMencoder.setupUi(this);
   m_update=false;
   load();

#ifdef BACKLITE
    Ui_prefMencoder.btnAddA->setIcon(SmallIcon("list-add"));
    Ui_prefMencoder.btnAddV->setIcon(SmallIcon("list-add"));
   Ui_prefMencoder.btnDeleteA->setIcon(SmallIcon("list-remove"));
   Ui_prefMencoder.btnDeleteV->setIcon(SmallIcon("list-remove"));
   Ui_prefMencoder.bReset->setIcon(SmallIcon("reset") );


#else

   Ui_prefMencoder.btnAddA->setGuiItem(KStandardGuiItem::Add);
   Ui_prefMencoder.btnAddV->setGuiItem(KStandardGuiItem::Add);
   Ui_prefMencoder.btnDeleteA->setGuiItem(KStandardGuiItem::Remove);
   Ui_prefMencoder.btnDeleteV->setGuiItem(KStandardGuiItem::Remove);
   Ui_prefMencoder.bReset->setGuiItem(KStandardGuiItem::Reset );
#endif
   Ui_prefMencoder.btnOpt1->setIcon(SmallIcon("configure"));
   Ui_prefMencoder.btnOpt2->setIcon(SmallIcon("configure"));
   Ui_prefMencoder.btnOpt3->setIcon(SmallIcon("configure"));
   Ui_prefMencoder.btnOptA->setIcon(SmallIcon("configure"));
}

void k9prefMencoder::load() {
   loadAudio();
   loadVideo();
}

void k9prefMencoder::loadVideo() {
   Ui_prefMencoder.lbvCodecs->clear();
   QListWidgetItem *item;
   for (int i=0;i < m_codecs.count();i++) {
       if (m_codecs.getEncoder(i)==Ui_prefMencoder.cbEncoderV->currentText()) {
            item=new QListWidgetItem(m_codecs.getCodecName(i),Ui_prefMencoder.lbvCodecs);
            item->setData(Qt::UserRole,i);
       }
   }
   Ui_prefMencoder.lbvCodecs->setCurrentRow(0);

}
void k9prefMencoder::loadAudio() {
   Ui_prefMencoder.lbaCodecs->clear();
   QListWidgetItem *item;
   for (int i=0;i < m_audioCodecs.count();i++) {
       if (m_audioCodecs.getEncoder(i)==Ui_prefMencoder.cbEncoderA->currentText()) {
           item=new QListWidgetItem(m_audioCodecs.getCodecName(i),Ui_prefMencoder.lbaCodecs);
           item->setData(Qt::UserRole,i);
        }
   }
   Ui_prefMencoder.lbaCodecs->setCurrentRow(0);
}


void k9prefMencoder::lbvCodecsCurrentChanged(QListWidgetItem *_item,QListWidgetItem *) {
   if (_item) {
        int i=_item->data(Qt::UserRole).toInt();
        m_update=true;
        Ui_prefMencoder.leFourcc->setText(m_codecs.getFourcc(i));
        Ui_prefMencoder.teOpt1->setText(m_codecs.getOptions0(i));
        Ui_prefMencoder.teOpt2->setText(m_codecs.getOptions1(i));
        Ui_prefMencoder.teOpt3->setText(m_codecs.getOptions2(i));
        Ui_prefMencoder.leVLabel->setText(_item->text());
        m_update=false;
    }
}

void k9prefMencoder::lbaCodecsCurrentChanged(QListWidgetItem *_item,QListWidgetItem *) {
   if (_item) {
        int i=_item->data(Qt::UserRole).toInt();
        m_update=true;
        Ui_prefMencoder.teOptA->setText(m_audioCodecs.getOptions(i));
        Ui_prefMencoder.leALabel->setText(_item->text());
        Ui_prefMencoder.leExtension->setText(m_audioCodecs.getExtension(i));
        m_update=false;
    }
}

void k9prefMencoder::save()
{
   m_codecs.save();
   m_audioCodecs.save();

}

SelectedEncoder k9prefMencoder::getSelectedEncoderV() {

   if (Ui_prefMencoder.cbEncoderV->currentText() =="ffmpeg")
	return FFMPEG;
   else 
	return MENCODER;

}

SelectedEncoder k9prefMencoder::getSelectedEncoderA() {

   if (Ui_prefMencoder.cbEncoderA->currentText() =="ffmpeg")
	return FFMPEG;
   else 
	return MENCODER;

}


void k9prefMencoder::btnOpt1Clicked() {
   QString s=Ui_prefMencoder.teOpt1->toPlainText();
   if (getSelectedEncoderV()==FFMPEG)
       k9ffmpegCmdGen::getFFMpegCmd(k9ffmpegCmdGen::VIDEO,s);
   else 
      k9MencoderCmdGen::getMencoderOptions(s,Ui_prefMencoder.cbEncoderV->currentText());

   Ui_prefMencoder.teOpt1->setText(s);
}
void k9prefMencoder::btnOpt2Clicked() {
   QString s=Ui_prefMencoder.teOpt2->toPlainText();
if (getSelectedEncoderV()==FFMPEG)
       k9ffmpegCmdGen::getFFMpegCmd(k9ffmpegCmdGen::VIDEO,s);
   else 
      k9MencoderCmdGen::getMencoderOptions(s,Ui_prefMencoder.cbEncoderV->currentText());

   Ui_prefMencoder.teOpt2->setText(s);
}
void k9prefMencoder::btnOpt3Clicked() {
   QString s=Ui_prefMencoder.teOpt3->toPlainText();
if (getSelectedEncoderV()==FFMPEG)
       k9ffmpegCmdGen::getFFMpegCmd(k9ffmpegCmdGen::VIDEO,s);
   else 
      k9MencoderCmdGen::getMencoderOptions(s,Ui_prefMencoder.cbEncoderV->currentText());

   Ui_prefMencoder.teOpt3->setText(s);
}
void k9prefMencoder::btnOptAClicked() {
   QString s=Ui_prefMencoder.teOptA->toPlainText();
if (getSelectedEncoderA()==FFMPEG)
       k9ffmpegCmdGen::getFFMpegCmd(k9ffmpegCmdGen::AUDIO,s);
   else 
      k9MencoderCmdGen::getMencoderOptions(s,Ui_prefMencoder.cbEncoderA->currentText());

   Ui_prefMencoder.teOptA->setHtml(s);
}

void k9prefMencoder::btnAddVClicked() {
   int i=m_codecs.count();
   QListWidgetItem *item=new QListWidgetItem(i18n("new profile"),Ui_prefMencoder.lbvCodecs);
   item->setData(Qt::UserRole,i);
   QString option;
   if (getSelectedEncoderV()==FFMPEG)
      option="-vcodec copy";
   else
       option="-ovc copy";
   m_codecs.setOptions0(i,option);
   m_codecs.setOptions1(i,option);
   m_codecs.setOptions2(i,option);
   m_codecs.setCodecName(i,i18n("new profile"));
   m_codecs.setEncoder(i,Ui_prefMencoder.cbEncoderV->currentText());

   Ui_prefMencoder.lbvCodecs->setCurrentRow(Ui_prefMencoder.lbvCodecs->count()-1);
}

void k9prefMencoder::btnDeleteVClicked() {
    if (Ui_prefMencoder.lbvCodecs->count()>0) {
       int i=Ui_prefMencoder.lbvCodecs->currentItem()->data(Qt::UserRole).toInt();
       int j=Ui_prefMencoder.lbvCodecs->currentRow();
       m_codecs.remove(i);
       QListWidgetItem *item=Ui_prefMencoder.lbvCodecs->currentItem();
       if (Ui_prefMencoder.lbvCodecs->count()>0)
         Ui_prefMencoder.lbvCodecs->setCurrentRow(j);
       delete item;
    }
}

void k9prefMencoder::btnAddAClicked() {
   int i=m_audioCodecs.count();
   QListWidgetItem *item=new QListWidgetItem(i18n("new profile"),Ui_prefMencoder.lbaCodecs);
   item->setData(Qt::UserRole,i);
   QString option;
   if (getSelectedEncoderA()==FFMPEG)
      option="-acodec copy";
   else
       option="-oac copy";
   m_audioCodecs.setOptions (i,option);
   m_audioCodecs.setCodecName(i,i18n("new profile"));
   m_audioCodecs.setEncoder(i,Ui_prefMencoder.cbEncoderA->currentText());
   Ui_prefMencoder.lbaCodecs->setCurrentRow(Ui_prefMencoder.lbaCodecs->count()-1);

}

void k9prefMencoder::bResetClicked() {
   m_codecs.reset();
   m_audioCodecs.reset();
   load();
   Ui_prefMencoder.lbvCodecs->setCurrentItem(0);
   Ui_prefMencoder.lbaCodecs->setCurrentItem(0);
//   lbvCodecsCurrentChanged (Ui_prefMencoder.lbvCodecs->item(0),0);
//   lbaCodecsCurrentChanged (Ui_prefMencoder.lbaCodecs->item(0),0);


}

void k9prefMencoder::btnDeleteAClicked() {
    if (Ui_prefMencoder.lbaCodecs->count()>0) {
       int i=Ui_prefMencoder.lbaCodecs->currentItem()->data(Qt::UserRole).toInt();
       int j=Ui_prefMencoder.lbaCodecs->currentRow();
       m_audioCodecs.remove(i);
       QListWidgetItem *item=Ui_prefMencoder.lbaCodecs->currentItem();
       if (Ui_prefMencoder.lbaCodecs->count()>0)
         Ui_prefMencoder.lbaCodecs->setCurrentRow(j);
       delete item;
   }
}


void k9prefMencoder::teOptATextChanged() {
  if (!m_update)
      m_audioCodecs.setOptions(Ui_prefMencoder.lbaCodecs->currentItem()->data(Qt::UserRole).toInt(),Ui_prefMencoder.teOptA->toPlainText());
}

void k9prefMencoder::teOpt1TextChanged() {
  if (!m_update)
      m_codecs.setOptions0(Ui_prefMencoder.lbvCodecs->currentItem()->data(Qt::UserRole).toInt(),Ui_prefMencoder.teOpt1->toPlainText());
}

void k9prefMencoder::teOpt2TextChanged() {
   if (!m_update)
   m_codecs.setOptions1(Ui_prefMencoder.lbvCodecs->currentItem()->data(Qt::UserRole).toInt(),Ui_prefMencoder.teOpt2->toPlainText());

}

void k9prefMencoder::teOpt3TextChanged() {
   if (!m_update)
   m_codecs.setOptions2(Ui_prefMencoder.lbvCodecs->currentItem()->data(Qt::UserRole).toInt(),Ui_prefMencoder.teOpt3->toPlainText());

}


void k9prefMencoder::leVLabelTextChanged(const QString &_value) {
   if (!m_update) {
       Ui_prefMencoder.lbvCodecs->currentItem()->setText(_value);
       m_codecs.setCodecName(Ui_prefMencoder.lbvCodecs->currentItem()->data(Qt::UserRole).toInt(),_value);
    }
}
void k9prefMencoder::leALabelTextChanged(const QString &_value) {
   if (!m_update) {
       Ui_prefMencoder.lbaCodecs->currentItem()->setText(_value);
       m_audioCodecs.setCodecName(Ui_prefMencoder.lbaCodecs->currentItem()->data(Qt::UserRole).toInt(),_value);
    }
}

void k9prefMencoder::leFourccTextChanged(const QString &_value) {
    if (!m_update) {
        m_codecs.setFourcc(Ui_prefMencoder.lbvCodecs->currentItem()->data(Qt::UserRole).toInt(),_value);
    }
}

void k9prefMencoder::leExtensionTextChanged(const QString &_value) {
   if (!m_update) {
        m_audioCodecs.setExtension(Ui_prefMencoder.lbaCodecs->currentItem()->data(Qt::UserRole).toInt(),_value);
   }
}
void k9prefMencoder::cbEncoderATextChanged(const QString &_value) {
    Q_UNUSED(_value);
   // if (!m_update)
   //     m_audioCodecs.setEncoder(Ui_prefMencoder.lbaCodecs->currentItem()->data(Qt::UserRole).toInt(),_value);
    loadAudio();
}

void k9prefMencoder::cbEncoderVTextChanged(const QString &_value) {
   Q_UNUSED(_value);
  //  if (!m_update)
  //      m_codecs.setEncoder(Ui_prefMencoder.lbvCodecs->currentItem()->data(Qt::UserRole).toInt(),_value);
    loadVideo();
}

k9prefMencoder::~k9prefMencoder()
{
}



/*$SPECIALIZATION$*/

