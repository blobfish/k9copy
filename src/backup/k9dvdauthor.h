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
#ifndef K9DVDAUTHOR_H
#define K9DVDAUTHOR_H

#include "k9common.h"
#include "k9dvd.h"
#include "k9progress.h"

#include <qdom.h>
#include <qdatastream.h>
#include <qprogressdialog.h>
#include <QTemporaryFile>
#include <QList>
/**
  *@author
  */

class k9DVDAuthor : public QObject  {
Q_OBJECT
public:
	k9DVDAuthor(QObject *DVDStruct) ;
	~k9DVDAuthor();
  virtual void setworkDir( const QString& _newVal);
  virtual const QString& getworkDir();
  virtual bool getError();
  virtual void author();
  virtual void setburnDVD( const bool& _newVal);
  virtual const bool& getburnDVD();
  virtual QWidget *getDialog(){return progress;};
private: // Private attributes
  /**  */
  QTime *time;
  k9DVD* DVD;
  double factor;
  QDomDocument *xml;
  QString lastMsg,totSize,inject;
  QString m_tempPath;
  k9Process *proc;
  QString workDir;
  k9Progress *progress;
  bool cancelled;
  bool error;
  bool burnDVD;
  bool m_firsttitle;
  uint64_t m_totalSize;
  uint64_t m_forced,m_forcedsh;
  uint32_t m_copied,m_lastPos;
  QTemporaryFile *m_xml,*m_inject;
  QList <QTemporaryFile*> files;
  void createXML();
  void addTitle(QDomElement &root, k9DVDTitle *title);
  void createMenus(bool preview);
  void addMenus(QDomElement &titleSet);
  void processMenu();
  void spumux();
  void setProgress(long _position,long _total) ;
  int execProcess();
private slots: // Private slots
  /** No descriptions */
  void DVDAuthorStderr(char *buffer, int buflen);
  void DVDAuthorStdout(char *buffer, int buflen);

  /** No descriptions */
  void stopProcess();
};

#endif

