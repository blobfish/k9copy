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

#ifndef K9CROPSELECT_H
#define K9CROPSELECT_H

#include "k9dvdtitle.h"

#include <qthread.h>
#include "ui_viewmpeg2.h"
#include <qwidget.h>
#include <qstring.h>
#include <qevent.h>
#include <qmutex.h>
#include <QTimer>
#include "k9plaympeg2.h"
#include "k9mplayer.h"
#ifndef BACKLITE
#include "k9xine.h"
#endif
#include "k9phonon.h"
#include "k9internalplayer.h"
#include <QWaitCondition>
#include <QRect>
class k9Widget;

class k9CropSelect : public QWidget,k9InternalPlayer  {
Q_OBJECT
public: 
	k9CropSelect(QWidget *parent);
	~k9CropSelect();
  virtual void drawImage( QImage *image);

private:
  QMutex mutex;
  k9DVDTitle *m_title;
  QString dev;
  int selTitle;
  unsigned long  idxLect;
  bool pause;
  bool lockSlider;
  bool m_drawing;
  QImage img,m_origImage;
  bool stopped;
  /** No descriptions */
  void lock();
  void unlock();
  QString file;
  QString errMsg;
  bool error;
  QString length;
  uint startSector;
  uint lastSector;
  k9PlayMPEG2 m_player;
  QGridLayout *m_layout;
  k9Widget *m_widget;
  Ui::ViewMPEG2 Ui_viewMPEG2;
  QTimer m_timer;
  QWaitCondition m_wplay;
  double getDistance(QRgb color1,QRgb color2) ;
  QRect m_rect;
protected:
 void closeEvent( QCloseEvent* );
 void resizeEvent ( QResizeEvent * );
public slots: // Public slots
  /** No descriptions */
  int open (k9DVD *_dvd,k9DVDTitle * title,int chapter);
  void setError(const QString & err);
  void setPosition(uint _position);
  void setMax(uint _position);
  void setMin(uint _position);
  void bPlayClick();
  void bStopClick();
  void readSettings();
  void sliderReleased();  
  void sliderPressed();
  void drawImage();
  void bCropDetectClick();
  void sbValueChanged(int);
  void setRect(QRect _crop);
  void drawCropping();
  QRect getRect();
};

#endif
