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
#include "k9phonon.h"
#include "k9dvdtitle.h"
#ifdef BACKLITE
#include <phonon/volumeslider.h>
#include <phonon/seekslider.h>
#else
#include <Phonon/VolumeSlider>
#include <Phonon/SeekSlider>
#endif
k9Phonon::k9Phonon(QWidget *parent)
 : QWidget(parent)
{
    m_media=new Phonon::MediaObject(this);
    connect(m_media,SIGNAL(stateChanged(Phonon::State,Phonon::State)),this,SLOT(stateChanged(Phonon::State, Phonon::State)));
    Ui_phonon.setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout(Ui_phonon.wVideo);

    Phonon::VideoWidget *m_vwidget = new Phonon::VideoWidget(this);
    layout->addWidget(m_vwidget);

    Phonon::AudioOutput *m_aoutput = new Phonon::AudioOutput(Phonon::VideoCategory, this);

    Phonon::createPath(m_media, m_aoutput);
    Phonon::createPath(m_media, m_vwidget);

//    connect(m_playButton, SIGNAL(clicked()), m_media, SLOT(play()));
    Phonon::VolumeSlider *m_volumeSlider = new Phonon::VolumeSlider(this);
    Phonon::SeekSlider *m_seekSlider = new Phonon::SeekSlider(this);
    m_seekSlider->setMediaObject(m_media);
    m_volumeSlider->setAudioOutput(m_aoutput);
#ifdef BACKLITE
    Ui_phonon.bPlay->setIcon(QIcon(":/icons/icon-start"));
    Ui_phonon.bStop->setIcon(QIcon(":/icons/icon-stop"));
#else
    Ui_phonon.bPlay->setIcon(SmallIcon("media-playback-start"));
    Ui_phonon.bStop->setIcon(SmallIcon("media-playback-stop"));
#endif
    QGridLayout *Grid=new QGridLayout(Ui_phonon.frControl);
    Grid->addWidget(Ui_phonon.bStop,0,0);
    Grid->addWidget(Ui_phonon.bPlay,0,1);
    Grid->addWidget(m_seekSlider,0,2);
    Grid->addWidget(m_volumeSlider,0,3);
    connect(Ui_phonon.bStop,SIGNAL(clicked()),this,SLOT(bStopClick()));
    connect(Ui_phonon.bPlay,SIGNAL(clicked()),this,SLOT(bPlayClick()));
    m_title=0;

   m_source=NULL;
}

void k9Phonon::bStopClick() {
  m_media->stop();
}

void k9Phonon::bPlayClick() {
  m_media->play();
}

void k9Phonon::stateChanged(Phonon::State newState,Phonon::State oldState) {
   Q_UNUSED(oldState);
   if (m_title!=0 && newState==Phonon::PlayingState) {
	m_controller->setCurrentTitle(m_title);
	m_title=0;
   }
}

void k9Phonon::cbSubtitleChanged(int _index) {
   if (_index >=0) {
	int i=Ui_phonon.cbSubtitle->itemData(_index).toInt();
	Phonon::SubtitleDescription sub=m_controller->availableSubtitles().at(i);
	m_controller->setCurrentSubtitle(sub);
   }

}

void k9Phonon::cbAudioChanged(int _index) {
   if (_index >=0) {
	int i=Ui_phonon.cbAudio->itemData(_index).toInt();
	Phonon::AudioChannelDescription au=m_controller->availableAudioChannels().at(i);
	m_changing=true;
	m_controller->setCurrentAudioChannel(au);
   }

}

void k9Phonon::availableAudioChanged() {
    if (m_changing)
	return;
    Ui_phonon.cbAudio->clear();
    for (int i=0; i< m_controller->availableAudioChannels().count();i++) {
	Phonon::AudioChannelDescription aud =m_controller->availableAudioChannels().at(i);
	Ui_phonon.cbAudio->addItem(aud.name(),i);

    }

}

void k9Phonon::availableSubtitlesChanged() {
    Ui_phonon.cbSubtitle->clear();
    for (int i=0; i< m_controller->availableSubtitles().count();i++) {
	Phonon::SubtitleDescription sub = m_controller->availableSubtitles().at(i);
	Ui_phonon.cbSubtitle->addItem(sub.name(),i);

    }
}

void k9Phonon::open(k9DVD *_dvd,k9DVDTitle *_title,int chapter) {
    Q_UNUSED(chapter);
    m_changing=false;

    if (m_media->state() != Phonon::PlayingState) {	
	if (m_device != _dvd->getDevice() && m_source!=NULL) {
	   delete m_source;
	   delete m_controller;
	   m_source=NULL;
    	}

        if (m_source==NULL) {
		setDevice(_dvd->getDevice());
	
		m_controller=new Phonon::MediaController(m_media);
        	connect(m_controller,SIGNAL(availableSubtitlesChanged()),this,SLOT(availableSubtitlesChanged()));
		connect(m_controller,SIGNAL(availableAudioChannelsChanged()),this,SLOT(availableAudioChanged()));
		m_controller->setAutoplayTitles(true);
	}
	m_title=_title->getnumTitle();
	//m_media->play();
    } else {
      m_controller->setCurrentTitle(_title->getnumTitle());
   // m_controller->setCurrentChapter(1);
    }
}

void k9Phonon::setDevice(const QString & _device){
    m_device=_device;
    m_source=new Phonon::MediaSource(Phonon::Dvd,m_device);
    m_media->setCurrentSource(*m_source);

}


k9Phonon::~k9Phonon()
{
   if (m_media->state() == Phonon::PlayingState)
   	m_media->stop();
}


