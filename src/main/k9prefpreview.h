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

#ifndef K9PREFPREVIEW_H
#define K9PREFPREVIEW_H

#include "k9common.h"
#include "ui_prefpreview.h"

class k9prefPreview : public QWidget
{
  Q_OBJECT

public:
  k9prefPreview(QWidget* parent = 0);
  ~k9prefPreview();
  /*$PUBLIC_FUNCTIONS$*/
  void save();
public slots:
  /*$PUBLIC_SLOTS$*/
  void rbInternalClick();
  void rbMplayerClick();
  void rbXineClick();
  void rbPhononClick();
protected:
  /*$PROTECTED_FUNCTIONS$*/
  void load();
protected slots:
  /*$PROTECTED_SLOTS$*/
private:
  Ui::prefPreview Ui_prefPreview;
};

#endif

