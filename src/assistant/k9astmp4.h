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

#ifndef K9ASTMP4_H
#define K9ASTMP4_H

#include "k9common.h"
#include "ui_astmp4.h"
#include "k9assistant.h"


class k9DVD;
class k9DVDTitle;
class k9MP4Title;
class k9astMp4 : public QWidget
{
  Q_OBJECT

public:
  k9astMp4(k9Assistant* parent,k9DVD *_dvd );
  ~k9astMp4();
  void setAudio();
  void setMatroskaCodecs();
  void setAviCodecs();
  /*$PUBLIC_FUNCTIONS$*/
public slots:
  /*$PUBLIC_SLOTS$*/
  void addTitle(k9DVDTitle*);
  void removeTitle(k9DVDTitle*);

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  void cbTitleChanged(int);

private:
  Ui::astMp4 Ui_astMp4;
  k9Assistant *m_parent;
  k9MP4Title *m_mp4Title;
  k9DVD *m_dvd;
  QComboBox *cbTitle;
  QList<k9DVDTitle*>m_titles;
signals:
  void titleChanged(k9DVDTitle*);
  
};


#endif

