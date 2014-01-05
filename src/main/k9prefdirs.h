//
// C++ Interface: 
//
// Description: sets independent default dirs for all outputs
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9PREFDIRS_H
#define K9PREFDIRS_H
#include "k9common.h"
#include "ui_prefDirs.h"

class k9prefDirs : public QWidget
{
  Q_OBJECT

public:
  k9prefDirs(QWidget* parent = 0);
  ~k9prefDirs();
  /*$PUBLIC_FUNCTIONS$*/
  void save();

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/

private:
  Ui::prefDirs Ui_prefDirs;
};

#endif

