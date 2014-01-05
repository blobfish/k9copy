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

#include "k9common.h"
#include "k9dvdprogress.h"
#include <QProgressBar>
#include <QLabel>
#include <QCloseEvent>
#include <kdialog.h>
k9DVDProgress::k9DVDProgress(QWidget* )
{
	Ui_DVDProgress.setupUi(this);
        setWindowTitle(KDialog::makeStandardCaption(i18n("DVD Analyze"),this));
}

k9DVDProgress::~k9DVDProgress(){
}
/** No descriptions */
void k9DVDProgress::setpbTitleTotalSteps(unsigned int total){
Ui_DVDProgress.pbTitle->setMaximum(total);

}
/** No descriptions */
void k9DVDProgress::setpbTitleStep(unsigned int position){
Ui_DVDProgress.pbTitle->setValue(position);
}
/** No descriptions */
void k9DVDProgress::setpbTotalTotalSteps(int total){
  Ui_DVDProgress.pbTotal->setMaximum(total);
}
/** No descriptions */
void k9DVDProgress::setpbTotalStep(int position){
  Ui_DVDProgress.pbTotal->setValue(position);
}
/** No descriptions */
void k9DVDProgress::setlblTitle(QString & text){
Ui_DVDProgress.lblTitle->setText(text);
}
/** No descriptions */
void k9DVDProgress::setlblTotal(QString& text){
Ui_DVDProgress.lblTotal->setText(text);
}

void k9DVDProgress::closeEvent( QCloseEvent* ce )
{
  ce->ignore();
  return;
}
