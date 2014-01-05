/**************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   k9copy@free.fr                                                            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef K9DVDPROGRESS_H
#define K9DVDPROGRESS_H

#include "k9common.h"
#include "ui_dvdprogress.h"

/**
  *@author 
  */

class k9DVDProgress : public QDialog{
Q_OBJECT
public: 
	k9DVDProgress(QWidget* parent );
	~k9DVDProgress();
  /** No descriptions */
  void setpbTitleTotalSteps(unsigned int total);
  /** No descriptions */
  void setpbTitleStep(unsigned int position);
  /** No descriptions */
  void setpbTotalTotalSteps(int total);
  /** No descriptions */
  void setpbTotalStep(int position);
  /** No descriptions */
  void setlblTitle(QString & text);
  /** No descriptions */
  void setlblTotal(QString& text);
protected:
 void closeEvent( QCloseEvent* ce );
private:
  Ui::DVDProgress Ui_DVDProgress;
};

#endif
