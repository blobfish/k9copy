/***************************************************************************
 *   Copyright (C) 2007 by Jean-Michel                                     *
 *   k9copy@free.fr                                                        *
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

#ifndef K9XINE_H
#define K9XINE_H

#include "k9common.h"
#include "ui_mplayer.h"
#include "k9process.h"
#include <QtDBus>

class k9DVD;
class k9DVDTitle;
class QTimer;
class QResizeEvent;

class k9xine : public QWidget
{
  Q_OBJECT
private:
  k9Process    *m_process;
  bool m_seeking,m_initVol;
  int m_title;
  int m_chapter;
  k9DVDTitle *m_dvdTitle;
  QString m_device;
  QTimer *m_timer;
  int m_position;
  bool m_canwrite;
  float m_ratio;
  Ui::MPlayer Ui_mplayer;
  QDBusInterface *m_iface;
  bool m_ready;
public:
  //k9xine(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  k9xine(QWidget  *parent=0);
  ~k9xine();
  /*$PUBLIC_FUNCTIONS$*/
  void setDevice(const QString & _device);
  void setTitle(const QString & _numTitle,const QString &_numChapter);
  
private slots:
  void slotLengthChanged();
  void slotNewPosition(int _pos,const QTime & _time);
  void receivedStdout ( char *buffer, int buflen);
  void timeout();
  void ready(const QString &name, const QString &oldValue, const QString &newValue);
public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void          sliderReleased();
  virtual void          bPlayClick();
  virtual void          bStopClick();
  virtual void 		sliderPressed();
  virtual void		sliderChanged(int _value);
  virtual void 		open(k9DVD *_dvd,k9DVDTitle *_title,int chapter);
  virtual void		titleChanged();
  virtual void		cbAudioActivated (int _value);
  virtual void 		cbSubActivated (int _value);
  virtual void		bUpClick();
  virtual void 		bDownClick();
  virtual void          bSwitchAudioClick();
  virtual void		quit();
protected:
  /*$PROTECTED_FUNCTIONS$*/
  void resizeEvent ( QResizeEvent * _resiseEvent);
protected slots:
  /*$PROTECTED_SLOTS$*/

};

#endif

