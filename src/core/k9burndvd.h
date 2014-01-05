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
#ifndef k9BurnDVD_H
#define k9BurnDVD_H

#include "k9common.h"
#include "k9burnprogress.h"


/**
@author Jean-Michel PETIT
*/
class k9BurnDVD: public QObject
{
Q_OBJECT
public:
  k9BurnDVD();

  ~k9BurnDVD();
  virtual void setburnDevice( const QString& _newVal);
  virtual const QString& getburnDevice();
  virtual QWidget* getDialog() {return progress;}
  void setworkDir( const QString& _newVal);
  void setvolId(QString  _newVal);
  void setUseK3b(bool _newVal);
  void setAutoBurn (bool _newVal);
  void burn();
  void makeIso(QString _filename);

  void setSpeed(const QString& _value) { m_speed = _value;}

  void setEject(bool theValue);
  bool isCancelled() { return cancelled;}
  
private:
    QTime *time;
    k9BurnProgress *progress;
    k9Process *proc;
    k9Process *proc2;
    QString volId;
    float burnSpeed;
    QString burnDevice;
    QStringList lastMsg;
    QString workDir;
    QString m_filename;
    bool cancelled;
    bool useK3b;
    bool m_eject;
    bool autoBurn;
    bool iso;
    bool m_cdrkit;
    void burnWithGrowisofs();
    void burnWithGenisoimage();
    void getGenisoimageCmd(QStringList &_cmd,QString _fileName,bool _printSize);
    void getWodimCmd(QStringList &_cmd);

    void burnWithK3b();
    void eject(const QString &device);
    const QString &getImageSize();
    QString imageSize;
    QString m_speed;
    k9Config *m_config;
private slots: // Private slots    
    void growisoStderr( char *buffer, int buflen);
    void growisoStdout(char *buffer, int buflen);
    void mkisoSizeStderr(char *buffer, int buflen);
    void mkisoSizeStdout(char *buffer, int buflen);
};

#endif
