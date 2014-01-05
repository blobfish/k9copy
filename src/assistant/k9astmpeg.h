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

#ifndef K9ASTMPEG_H
#define K9ASTMPEG_H
#include "k9common.h"
#include "ui_astmpeg.h"

#include "k9assistant.h"


class k9DVD;
class k9DVDTitle;
class k9TitleFactor;
class k9astMpeg : public QWidget
{
  Q_OBJECT

public:
  k9astMpeg(k9Assistant* parent,k9DVD *_dvd );
  ~k9astMpeg();
  /*$PUBLIC_FUNCTIONS$*/
public slots:
  /*$PUBLIC_SLOTS$*/
  void addTitle(k9DVDTitle*);
  void removeTitle(k9DVDTitle*);
  void updateFactor();
protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  void cbTitleChanged(int);

private:
  Ui::astMpeg Ui_astMpeg;
  k9Assistant *m_parent;
  k9DVD *m_dvd;
  QComboBox *cbTitle;
  QList<k9DVDTitle*>m_titles;
  k9TitleFactor *m_titleFactor;
signals:  void titleChanged(k9DVDTitle*);
  
};


#endif

