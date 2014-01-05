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

#ifndef K9TITLEFACTOR_H
#define K9TITLEFACTOR_H

#include "k9common.h"
#include "ui_titlefactor.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"


class k9TitleFactor : public QWidget
{
  Q_OBJECT

public:
  k9TitleFactor(QWidget* parent = 0);
  ~k9TitleFactor(); 
  /*$PUBLIC_FUNCTIONS$*/
private :
  k9DVDTitle *m_current;
  k9DVD	     *m_dvd;
  bool	     m_slFactorPressed;
  bool	     m_withMenus;
  bool       m_useDvdAuthor;
  float	     m_factor;
  bool	     m_updating;
  Ui::titleFactor Ui_titleFactor;
public slots:  
  /*$PUBLIC_SLOTS$*/
  virtual void          ckAutoClicked(bool _checked);
  virtual void          slFactorReleased();
  virtual void          slFactorPressed();
  virtual void          slFactor_valueChanged( int );
  virtual void 		SelectionChanged( k9DVD *_dvd,bool _withMenus);
  virtual void 		changedTitle(k9DVDTitle *_title);
  virtual void	        setUseDvdAuthor(bool _value);
protected:
  /*$PROTECTED_FUNCTIONS$*/
  void setMinFactor();
  float getFactor();

};

#endif

