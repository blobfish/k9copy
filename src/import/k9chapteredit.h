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

#ifndef K9CHAPTEREDIT_H
#define K9CHAPTEREDIT_H

#include "k9common.h"
#include "ui_chapterEdit.h"
#include "k9avidecode.h"
#include "k9avifile.h"
#include <QObject>

class k9DrawImage;
class k9ChapterEdit : public QWidget
{
  Q_OBJECT

public:
  k9ChapterEdit(QWidget* parent = 0);
  ~k9ChapterEdit();
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/
private:
  k9AviDecode *m_aviDecode;
  k9DrawImage *m_wimage;
  k9AviFile *m_aviFile;
  QTime m_minTime,m_maxTime;
  bool m_noUpdate;
  QImage m_image;
  Ui::chapterEdit Ui_chapterEdit; 
protected slots:
  /*$PROTECTED_FUNCTIONS$*/
  virtual void sliderReleased();
  virtual void sliderChanged(int _value);
  virtual void bStartClick();
  virtual void bEndClick();
  virtual void ckBreakStartClick();
  virtual void ckBreakEndClick();
  virtual void urlSelected(const KUrl&);
  virtual void startChanged(const QTime&);
  virtual void endChanged(const QTime&);
  virtual void bTitleButtonClick();
  void drawImage(QImage);
public slots:
  void setAviFile(k9AviFile *_aviFile);
};

#endif

