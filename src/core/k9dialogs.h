/***************************************************************************
 *   Copyright (C) 2007 by Jean-Michel   *
 *   jm_petit@laposte.net   *
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
#ifndef K9DIALOGS_H
#define K9DIALOGS_H


#include <QString>
#include <QStringList>
#include <QFont>
/**
	@author Jean-Michel <jm_petit@laposte.net>
*/
class k9Dialogs
{
public:
    static QString getOpenFileName (const QString &_initialDir,const QString &_filter,QWidget *_parent,const QString &_title);
    static QString getSaveFileName (const QString &_initialDir,const QString &_filter,QWidget *_parent,const QString &_title);
    static QString getExistingDirectory (const QString &_initialDir) ;
    static void error(const QString &_message,const QString &_title,const QStringList &_detail=QStringList()) ;
    static void information(const QString &_message,const QString &_title) ;
    static int warningContinueCancel(const QString &  _message,const QString &  _title,const QStringList &_detail=QStringList());
    static void setMainWidget(QWidget *_widget);	
    static QWidget *getMainWidget();	
    static bool getFont(QFont &font);

};

#endif
