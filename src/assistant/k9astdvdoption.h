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

#ifndef K9ASTDVDOPTION_H
#define K9ASTDVDOPTION_H

#include "k9common.h"
#include "ui_astdvdoption.h"

#include "k9assistant.h"


class k9DVD;
class k9DVDTitle;
class k9astDVDOption : public QWidget
{
  Q_OBJECT

public:
  k9astDVDOption(k9Assistant* parent,k9DVD *_dvd );
  ~k9astDVDOption();
  /*$PUBLIC_FUNCTIONS$*/
  void updateOptions();
  void clear();
public slots:
  /*$PUBLIC_SLOTS$*/
  void addTitle(k9DVDTitle*);
  void removeTitle(k9DVDTitle*);

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  void rbMenuToggled(bool);
private:
  Ui::astDVDOption Ui_astDVDOption;
  k9Assistant *m_parent;
  k9DVD *m_dvd;
  QListWidget *m_titles;
signals:
  void titleChanged(k9DVDTitle*);
  
};


#endif

