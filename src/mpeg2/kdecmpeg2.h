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

#ifndef KDECMPEG2_H
#define KDECMPEG2_H
#include "k9common.h"
#include "k9internalplayer.h"

#include <stdio.h>
#include <stdlib.h>
#include <qimage.h>
#include <qmutex.h>
#include <qwidget.h>
#include <qobject.h>
#include <qthread.h>
#include <qdatetime.h>

extern "C" {
#include <mpeg2dec/mpeg2.h>
}
/**
  *@author 
  */

class kDecMPEG2;
class k9SaveImage;


class k9DecodeThread;


class kDecMPEG2 : public QObject  {
Q_OBJECT
public: 
  kDecMPEG2(k9DecodeThread *_thread);
  kDecMPEG2();
  ~kDecMPEG2();
  int decode (uint8_t * buf, uint8_t * end, int flags);
  void restart();
  void start();
  void stop();
  void pause();

	
private:
  bool m_pause;
  k9DecodeThread *m_thread;
  QImage pix;
  QTime m_timer;
  bool m_opened;
  int demux_pid;
  int demux_track;
  QMutex mutex;
  mpeg2dec_t * decoder;

  int demux (uint8_t * buf, uint8_t * end, int flags);
  void save_ppm (int width, int height, uint8_t * buf, int num);
  void decode_mpeg2(uint8_t * current, uint8_t * end);
  void init();
  void sync();
};                       

#endif
