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
#ifndef KCONFIGDLG_H
#define KCONFIGDLG_H

#include "k9common.h"
#include <ui_configDlg.h>
#include <QTableWidget>
#include <qstringlist.h>
#include <qfile.h>

/**
@author Jean-Michel PETIT
*/
class kConfigDlg : public QWidget
{
Q_OBJECT
public:
    kConfigDlg(QWidget *parent);
    ~kConfigDlg();
    void save();

public slots:
    void bAddClick();
    void bRemoveClick();
private:
    Ui::configDlg Ui_configDlg;
};

#endif
