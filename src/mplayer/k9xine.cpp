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
#include "k9common.h"
#include "k9config.h"
#include <qlayout.h>
#include <qslider.h>
#include "k9xine.h"
#include <qcombobox.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include <qlabel.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qevent.h>
#include <klocale.h>
#include <kmessagebox.h>

#define SERVICE_NAME "k9copy.free.fr.xineplayer"

k9xine::k9xine(QWidget  *parent):QWidget(parent) {
    Ui_mplayer.setupUi(this);
    m_seeking=FALSE;
    m_initVol=TRUE;
    m_title=0;

    m_process=new k9Process(this);
    m_ratio=4.0/3.0;

    //Ui_mplayer.bSwitchAudio->hide();
    Ui_mplayer.slider->setRange(0, 0);
    Ui_mplayer.slider->setSingleStep(1);
    Ui_mplayer.slider->setPageStep(1);

    Ui_mplayer.bPlay->setIcon(SmallIcon("media-playback-start"));
    Ui_mplayer.bStop->setIcon(SmallIcon("media-playback-stop"));
    //Ui_mplayer.bSwitchAudio->setIcon(SmallIcon("cycle"));
    connect(m_process,SIGNAL(receivedStdout( char*, int )),this,SLOT(receivedStdout(  char*, int )));
    m_timer=new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(timeout()));
    m_iface=NULL;

}


k9xine::~k9xine() {
   if (m_process->isRunning()  && m_iface) {
     m_iface->call("stop");
     m_iface->call("quit");
   }
   m_process->waitForFinished(3000);
   m_process->terminate();
}

void k9xine::quit() {
   if (m_process->isRunning()  && m_iface) {
     m_iface->call("stop");
     m_iface->call("quit");
   }

}

void k9xine::resizeEvent ( QResizeEvent * ) {
    int w,h;
    w=Ui_mplayer.Label->width();
    h=Ui_mplayer.Label->height();
    if (m_iface) {
        m_iface->setProperty("width",w);
        m_iface->setProperty("height",h);
    }
}



void k9xine::timeout() {
//    sendCmd( "get_percent_pos");
    if (m_iface && ! m_seeking) {
       int _length=65535;
       int _pos=m_iface->property("positionAbs").toInt();
       Ui_mplayer.slider->setMinimum(0);
       Ui_mplayer.slider->setMaximum(_length);
       Ui_mplayer.slider->setValue(_pos);
      // qDebug() << "position :" << _pos << "   length:" <<_length;
    }
}

void k9xine::setDevice(const QString & _device) {
    m_device=_device;
}


void k9xine::ready(const QString &name, const QString &oldValue, const QString &newValue)
{
    Q_UNUSED(oldValue);

    if (name != SERVICE_NAME || newValue.isEmpty())
        return;

    // find our remote
    m_iface = new QDBusInterface(SERVICE_NAME, "/", "k9copy.free.fr.xineplayer",
                               QDBusConnection::sessionBus(), this);
    if (!m_iface->isValid()) {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
    }
    m_ready=true;
    m_iface->call("execute");
}

void k9xine::setTitle( const QString & _numTitle,const QString &) {
    if (!m_process->isRunning()) {
	k9Config config;
	QString vout[]= {"xshm","xv","opengl","sdl"};
	QString aout[]= {"alsa","oss"};
	QString vo=vout[config.getXineVout()];
	QString ao=aout[config.getXineAout()];
	m_ready=false;
	connect(QDBusConnection::sessionBus().interface(),SIGNAL(serviceOwnerChanged(QString,QString,QString)),this,SLOT(ready(QString,QString,QString)));
	
	m_process->clearProgram();

	*m_process << qApp->applicationDirPath()+ "/k9xineplayer" << "-wid" <<  QString::number(Ui_mplayer.Label->winId()) << "-vo" << vo << "-ao" << ao;
	qDebug() << m_process->debug();

	if (!m_process->start()) {
                k9Dialogs::error (i18n("Unable to run %1").arg("k9xineplayer") , i18n("Preview"));
	} else {
            while (!m_ready) k9Tools::processEvents();
	}
    }

    bool running=m_iface->property("isRunning").toBool();

    m_iface->call("stop");
    m_iface->setProperty("device",m_device);
    m_iface->setProperty("mrl",QString("dvd:/%1").arg(_numTitle));
    m_iface->setProperty("width",Ui_mplayer.Label->width());
    m_iface->setProperty("height",Ui_mplayer.Label->height());
    if (running)
       m_iface->call("play");
    m_position=0;
    Ui_mplayer.slider->setValue(m_position);
}




/*$SPECIALIZATION$*/

void  k9xine::receivedStdout (char *, int ) {
}

void k9xine::slotLengthChanged() {}

void k9xine::slotNewPosition(int _pos,const QTime & ) {
    Ui_mplayer.slider->setValue(_pos);

}

void k9xine::sliderReleased() {
    m_seeking=FALSE;
}

void k9xine::sliderChanged( int _value) {
    if (m_seeking && m_iface) {
  	m_iface->setProperty("positionAbs",_value);
    }
}

void k9xine::sliderPressed() {
    m_seeking=TRUE;

}

void k9xine::bPlayClick() {
    setTitle( QString::number(m_title),QString::number(m_chapter));
    if (m_dvdTitle->getaudioStreamCount() >0)
        cbAudioActivated( 0);
    if (m_dvdTitle->getsubPictureCount() >0)
        cbSubActivated( 0);
    if (!m_timer->isActive())
        m_timer->start(200);


   if (m_process->isRunning()  && m_iface) {
     m_iface->call("play");
   }

}

void k9xine::bStopClick() {
   if (m_process->isRunning()  && m_iface) {
     m_iface->call("stop");
   }
}


void k9xine::bDownClick() {
  if (m_iface)
    m_iface->call("volumeDown");
}

void k9xine::bSwitchAudioClick() {
}

void k9xine::bUpClick() {
   if (m_iface)
     m_iface->call("volumeUp");
}


void k9xine::open( k9DVD *_dvd,k9DVDTitle *_title,int chapter) {
    m_dvdTitle=NULL;
    Ui_mplayer.cbSub->clear();
    Ui_mplayer.cbAudio->clear();
    for (int i=0; i< _title->getaudioStreamCount();i++) {
        k9DVDAudioStream *aud=_title->getaudioStream(i);
        Ui_mplayer.cbAudio->addItem(QString("%1-%2").arg(aud->getID()).arg(aud->getlanguage()),-1);
    }

    for (int i=0; i< _title->getsubPictureCount();i++) {
        k9DVDSubtitle *sub=_title->getsubtitle(i);
        //Ui_mplayer.cbSub->addItem(QString("%1-%2").arg(sub->getID().first()).arg(sub->getlanguage()),-1);
        Ui_mplayer.cbSub->addItem(QString("%1-%2").arg(i+1).arg(sub->getlanguage()),-1);
    }

    setDevice( _dvd->getDevice());
    m_dvdTitle=_title;
    m_title=_title->getnumTitle();
    m_chapter= chapter;
    if (m_iface)
       if (m_iface->property("isRunning").toBool())
	 bPlayClick();
}

void k9xine::titleChanged() {}

void k9xine::cbAudioActivated( int _value) {
    if (m_dvdTitle && m_iface) {
//       int value=m_dvdTitle->getaudioStream( _value)->getStreamId();
       m_iface->call("setAudioLang",_value);
//       sendCmd(QString( "switch_audio %1").arg(value));
    }
}

void k9xine::cbSubActivated( int _value) {
//    sendCmd(QString("sub_select %1").arg(_value));
   if (m_dvdTitle && m_iface) {
	k9DVDSubtitle *sub=m_dvdTitle->getsubtitle(_value);
	if (m_iface) {
		m_iface->call("setSubtitle",sub->getID().first());
	}
   }
}
