//
// C++ Interface: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9PREFMENCODER_H
#define K9PREFMENCODER_H

#include "k9common.h"
#include "ui_prefMencoder.h"
#include "k9videocodecs.h"
#include "k9audiocodecs.h"
#include <qstringlist.h>
#include <qstring.h>
#include <qlineedit.h>

enum SelectedEncoder {FFMPEG,MENCODER};
class k9prefMencoder : public QWidget
{
  Q_OBJECT

public:

  k9prefMencoder(QWidget* parent = 0 );
  ~k9prefMencoder();
  /*$PUBLIC_FUNCTIONS$*/
  void save(void);
  void load();
public slots:
  /*$PUBLIC_SLOTS$*/
   void teOptATextChanged();

   void teOpt1TextChanged();
   void teOpt3TextChanged();
   void teOpt2TextChanged();
   void leVLabelTextChanged(const QString &);
   void leALabelTextChanged(const QString &);
   void leFourccTextChanged(const QString &);
  void cbEncoderATextChanged(const QString &);
  void cbEncoderVTextChanged(const QString &);
   void leExtensionTextChanged(const QString&);

private:
    void loadAudio();
    void loadVideo();
    k9VideoCodecs m_codecs;
    k9AudioCodecs m_audioCodecs;
    bool m_update;
    Ui::prefMencoder Ui_prefMencoder;
    SelectedEncoder getSelectedEncoderA();
    SelectedEncoder getSelectedEncoderV();
protected slots:
  /*$PROTECTED_SLOTS$*/
  void lbvCodecsCurrentChanged(QListWidgetItem *_item,QListWidgetItem*);
  void lbaCodecsCurrentChanged(QListWidgetItem *_item,QListWidgetItem*);
  void btnOpt1Clicked();
  void btnOpt2Clicked();
  void btnOpt3Clicked();
  void btnOptAClicked();
  void btnAddVClicked();
  void btnDeleteVClicked();
  void btnAddAClicked();
  void btnDeleteAClicked();
  void bResetClicked();
};


#endif

