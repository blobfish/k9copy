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
#ifndef K9PHONON_H
#define K9PHONON_H
#include "k9common.h"
#include <QWidget>
#include "ui_phonon.h"
#include "k9dvd.h"
#ifdef BACKLITE
#include <phonon/mediaobject.h>
#include <phonon/mediacontroller.h>
#include <phonon/path.h>
#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/videowidget.h>
#include <phonon/mediasource.h>
#else
#include <Phonon/MediaObject>
#include <Phonon/MediaController>
#include <Phonon/Path>
#include <Phonon/AudioOutput>
#include <Phonon/Global>
#include <Phonon/VideoWidget>
#include <Phonon/MediaSource>
#endif
#include <QLayout>
/**
	@author Jean-Michel <jm_petit@laposte.net>
*/
class k9Phonon : public QWidget
{
Q_OBJECT
public:
    k9Phonon(QWidget *parent = 0);

    ~k9Phonon();

    void setDevice(const QString & _device);
public slots:
  /*$PUBLIC_SLOTS$*/
  virtual void          bStopClick();
  virtual void          bPlayClick();
  virtual void 		open(k9DVD *_dvd,k9DVDTitle *_title,int chapter);
  virtual void 		stateChanged(Phonon::State,Phonon::State);
  virtual void		availableSubtitlesChanged();
  virtual void		availableAudioChanged();
  virtual void		cbSubtitleChanged(int);
  virtual void		cbAudioChanged(int);
private:
   Ui::phonon Ui_phonon;
   QString m_device;
   Phonon::MediaObject *m_media;
   Phonon::MediaSource *m_source;
   Phonon::MediaController *m_controller;
   int m_title;
   bool m_changing;
};

#endif
