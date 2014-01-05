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

#ifndef K9NEWTITLE_H
#define K9NEWTITLE_H

#include "k9common.h"
#include "ui_newTitle.h"
#include "k9lvitemimport.h"
#include <qstring.h>
#include <QObject>
class k9Import;
class k9NewTitle : public QWidget
{
  Q_OBJECT
private:
   k9Import *m_k9Import;
   QString m_fileName;
   QImage m_image;
   Ui::newTitle Ui_newTitle;
   bool isMpeg2(const QString &_fileName);
   
public:
  k9NewTitle(QWidget* parent = 0 );
  ~k9NewTitle();
  void setK9Import ( k9Import* _value );
	
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  virtual void urlSelected(const KUrl &);
  virtual void bAddClicked();
  virtual void rbNumberClicked();
  virtual void rbLengthClicked();
  virtual void drawImage (QImage _image);
  virtual void ckReencodeClicked();
};

#endif

