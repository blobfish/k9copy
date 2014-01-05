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

#ifndef K9PREFDVD_H
#define K9PREFDVD_H
#include "k9common.h"
#include "ui_prefDVD.h"

class k9prefDVD : public QWidget
{
  Q_OBJECT

public:
  k9prefDVD(QWidget* parent = 0);
  ~k9prefDVD();
  /*$PUBLIC_FUNCTIONS$*/
  void save();

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  void lwAudioLangChanged (QListWidgetItem *item);
  void lwSubtitleLangChanged (QListWidgetItem *item);
private:
  Ui::prefDVD Ui_prefDVD;
  void loadLanguages(QListWidget *_list,const QStringList &_prefered);
  bool m_loading;
};

#endif

