/***************************************************************************
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

#ifndef K9BACKUPDLG_H
#define K9BACKUPDLG_H

#include "k9common.h"
#include "ui_backupdlg.h"
#include <qmutex.h>
#include "k9decodethread.h"
#include "k9internalplayer.h"
#include <QDialog>
class k9DrawImage;

class k9BackupDlg : public QDialog,k9InternalPlayer
{
  Q_OBJECT

public:
  k9BackupDlg(QWidget* parent = 0);
  ~k9BackupDlg();
  /*$PUBLIC_FUNCTIONS$*/
  void setTotalSteps(uint32_t _totalSteps);
  void setProgress(uint32_t _position);
  void setProgressLabel(QString _text);
  void setProgressTotal(uint32_t _position);
  void setTotalMax(uint32_t _max);
  void setFactor(QString _factor);
  bool getAbort();
  void playMovie(uchar *_data,uint32_t _size);
public slots:
  /*$PUBLIC_SLOTS$*/
private :
  bool Abort;
  QTimer *timer;
  QTime time;
  QTime m_timer;
  uint64_t totalCopied,m_progressTotal;;
  QString m_progressLabel;
  uint32_t m_totalSteps,m_progress;
  QString  m_factor;
  bool m_stop;
  void updateMe();
  k9DecodeThread m_decoder;
  uint m_count;
  QMutex m_mutex;
  bool m_playmovie;
  k9DrawImage *m_wimage;
  Ui::backupDlg ui_backupDlg;
  QImage m_image;
  int m_outputSize;
protected:
  /*$PROTECTED_FUNCTIONS$*/
  void showEvent ( QShowEvent * event );
public:
  void drawImage(QImage *image);
protected slots:
  /*$PROTECTED_SLOTS$*/
  void timerDone();
  
  void bAbortClick();
  void gbOutputToggled(bool state);
  void bPlayToggled(bool state);


};

#endif

