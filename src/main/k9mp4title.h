//
// C++ Interface: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9MP4TITLE_H
#define K9MP4TITLE_H

#include "k9common.h"
#include "ui_prefMPEG4.h"
#include "k9titleencopt.h"


class k9DVDTitle;
class k9DVD;
class k9MP4Title : public QWidget
{
  Q_OBJECT

public:
  enum eOptTitle {optAudio,optVideo,optAll} ;
  k9MP4Title(QWidget* parent = 0,eOptTitle options=optAll);
  void setOptions (eOptTitle _options);
  void loadCodecs(QStringList _audioEncoders,QStringList _videoEncoders); 
  void testFields();
  ~k9MP4Title();
  /*$PUBLIC_FUNCTIONS$*/
private:
  k9TitleEncOpt *m_titleEncOpt;
  Ui::prefMPEG4 Ui_prefMPEG4;
  eOptTitle m_options;
  k9DVD *m_dvd;
  bool m_updating;
  QString m_videoOptions,m_audioOptions;
public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void ckMp4AspectRatioClick();
  virtual void load();
  virtual void titleChanged(k9DVDTitle *_title);
  virtual void selectionChanged(k9DVD *_dvd,bool);
protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void cbMp4CodecActivated( int );
  virtual void cbMp4AudioCodecActivated( int );

  virtual void ck2passesClicked();
  virtual void sbMp4SizeValueChanged( int );
  virtual void sbAudioGainValueChanged( int );

  virtual void sbMp4NumberCDValueChanged( int );
  virtual void leMp4WidthTextChanged( const QString & );
  virtual void leMp4HeightTextChanged( const QString & );
  virtual void leMp4AudioBitrateTextChanged( const QString & );
  virtual void leMp4VideoBitrateTextChanged( const QString & );

  virtual void rgVideoSizeChanged();

  virtual void ckUseCacheClick();
  virtual void cbEncoderActivated(int);
  virtual void bCropClicked();
  virtual void sbValueChanged(int);
signals:
  void updateSelection();    
};

#endif

