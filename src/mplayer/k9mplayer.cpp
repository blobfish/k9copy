#include "k9common.h"
#include <qlayout.h>
#include <qslider.h>
#include "k9mplayer.h"
#include <qcombobox.h>
#ifndef BACKLITE
#include <kiconloader.h>
#endif
#include <kpushbutton.h>
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include <qlabel.h>
#include <qtimer.h>
#include <qapplication.h>
#include <qevent.h>

K9Mplayer::K9Mplayer(QWidget  *parent):QWidget(parent) {
    Ui_mplayer.setupUi(this);
    m_seeking=FALSE;
    m_initVol=TRUE;
    m_title=0;

    m_process=new k9Process(this);
    m_ratio=4.0/3.0;

    Ui_mplayer.slider->setRange(0, 100);
    Ui_mplayer.slider->setSingleStep(1);
    Ui_mplayer.slider->setPageStep(1);

    Ui_mplayer.bPlay->setIcon(SmallIcon("media-playback-start"));
    Ui_mplayer.bStop->setIcon(SmallIcon("media-playback-stop"));
    //Ui_mplayer.bSwitchAudio->setIcon(SmallIcon("cycle"));
    connect(m_process,SIGNAL(receivedStdout(  char*, int )),this,SLOT(receivedStdout( char*, int )));
    m_timer=new QTimer(this);
   // connect(m_process,SIGNAL(wroteStdin( KProcess* )),this,SLOT(wroteStdin( KProcess* )));
    connect(m_timer,SIGNAL(timeout()),this,SLOT(timeout()));
    m_container=new QWidget(Ui_mplayer.Label);
}


K9Mplayer::~K9Mplayer() {
    if (m_process->isRunning()) {
        sendCmd("quit");
    }

}

void K9Mplayer::resizeEvent ( QResizeEvent * ) {
    int w,h;
    w=Ui_mplayer.Label->width();
    h=Ui_mplayer.Label->width() /m_ratio;
    if (h>Ui_mplayer.Label->height()) {
        h=Ui_mplayer.Label->height();
        w=Ui_mplayer.Label->height()*m_ratio;
    }

    m_container->resize(w,h);
    m_container->move((Ui_mplayer.Label->width()-w)/2,(Ui_mplayer.Label->height()-h)/2);
}


void K9Mplayer::wroteStdin( ) {
    m_canwrite = TRUE;
}

void K9Mplayer::timeout() {
    sendCmd( "get_percent_pos");

}

void K9Mplayer::setDevice(const QString & _device) {
    m_device=_device;
}


void K9Mplayer::setTitle( const QString & _numTitle,const QString &_numChapter) {
   m_title=_numTitle.toInt();;
   m_chapter=_numChapter.toInt();
}




/*$SPECIALIZATION$*/

void  K9Mplayer::receivedStdout (char *buffer, int buflen) {
    QString buf = QString::fromLatin1(buffer, buflen);
    
    sscanf(buf.toUtf8(),"ANS_PERCENT_POSITION=%d",&m_position);
    if (!m_seeking && m_position>0) {
        Ui_mplayer.slider->setValue(m_position);
    }
    int audio=0;
    if (buf.contains("ANS_switch_audio")){
        int pos=buf.indexOf("ANS_switch_audio");
	QString tmp=buf.mid(pos,20);
        sscanf(tmp.toUtf8(),"ANS_switch_audio=%d",&audio);
    }
    if (audio >0) {
        for (int i=0;i < m_dvdTitle->getaudioStreamCount();i++) {
            k9DVDAudioStream * str=m_dvdTitle->getaudioStream(i);
            if (str->getStreamId() == audio) {
                Ui_mplayer.cbAudio->setCurrentIndex(i);
            }
        }
    }
}

void K9Mplayer::slotLengthChanged() {}

void K9Mplayer::slotNewPosition(int _pos,const QTime & ) {
    Ui_mplayer.slider->setValue(_pos);

}

void K9Mplayer::sliderReleased() {
    sendCmd( QString("seek %1 1").arg((int)Ui_mplayer.slider->value()));
    m_seeking=FALSE;
}

void K9Mplayer::sliderChanged( int ) {}

void K9Mplayer::sliderPressed() {
    m_seeking=TRUE;

}

void K9Mplayer::bPlayClick() {
    if (m_process->isRunning()) {
        sendCmd("quit");
        m_process->waitForFinished( 1000);
    }
    k9Config config;
    QString vout[]= {"x11","xv","gl2","sdl"};
    QString aout[]= {"alsa","oss","sdl"};
    QString vo=vout[config.getMplayerVout()];
    QString ao=aout[config.getMplayerAout()];


    m_process->clearProgram();

    *m_process << config.getPrefMplayerPath(true);
    *m_process << "-vo" << vo << "-ao" << ao << "-sws" << "0";
    *m_process << "-framedrop";
    *m_process << "-wid" << QString::number(m_container->winId()) ;
    *m_process << "-slave" ;
    *m_process <<  "-idle";
    *m_process << "-dvd-device" << m_device;
    if (m_chapter !=0)
        *m_process << "-chapter" <<  QString::number( m_chapter);

    *m_process << QString("dvd://%1").arg(m_title);

    qDebug() << m_process->debug();
    if (!m_process->start()) {
         k9Dialogs::error (i18n("Unable to run %1").arg("mplayer") , i18n("Preview"));
    }

    m_canwrite=TRUE;

    m_position=0;
    Ui_mplayer.slider->setValue(m_position);

}

void K9Mplayer::bStopClick() {
    sendCmd("quit");
}


void K9Mplayer::bDownClick() {
    sendCmd( QString("volume -1"));

}

void K9Mplayer::bSwitchAudioClick() {
    sendCmd( QString("switch_audio"));
    sendCmd( QString("get_property switch_audio"));
}

void K9Mplayer::bUpClick() {
    sendCmd( QString("volume +1"));

}


void K9Mplayer::open( k9DVD *_dvd,k9DVDTitle *_title,int chapter) {
    m_dvdTitle=NULL;
    Ui_mplayer.cbSub->clear();
    Ui_mplayer.cbAudio->clear();
    for (int i=0; i< _title->getaudioStreamCount();i++) {
        k9DVDAudioStream *aud=_title->getaudioStream(i);
        Ui_mplayer.cbAudio->addItem(QString("%1-%2").arg(aud->getID()).arg(aud->getlanguage()),-1);
    }

    for (int i=0; i< _title->getsubPictureCount();i++) {
        k9DVDSubtitle *sub=_title->getsubtitle(i);
        Ui_mplayer.cbSub->addItem(QString("%1-%2").arg(sub->getID().first()).arg(sub->getlanguage()),-1);
    }

    if(_title->getaspectRatio()=="16:9")
        m_ratio=16.0/9.0;
    else
        m_ratio=4.0/3.0;
    resizeEvent( NULL);

    setDevice( _dvd->getDevice());
    m_dvdTitle=_title;
    m_title=_title->getnumTitle();
    m_chapter=chapter;
//    setTitle( QString::number(m_title),QString::number(chapter));
    if (_title->getaudioStreamCount() >0)
        cbAudioActivated( 0);
    if (_title->getsubPictureCount() >0)
        cbSubActivated( 0);
    if (!m_timer->isActive())
        m_timer->start(200);
    if (m_process->isRunning())
	bPlayClick();
}

void K9Mplayer::titleChanged() {}

void K9Mplayer::cbAudioActivated( int _value) {
    if (m_dvdTitle) {
       int value=m_dvdTitle->getaudioStream( _value)->getStreamId();
       sendCmd(QString( "switch_audio %1").arg(value));
    }
}

void K9Mplayer::cbSubActivated( int _value) {
   if (m_dvdTitle) {
	k9DVDSubtitle *sub=m_dvdTitle->getsubtitle(_value);
        sendCmd(QString("sub_select %1").arg(sub->getID().first() -1));
   }


}


void K9Mplayer::sendCmd( QString _cmd) {
    while (!m_canwrite) {
        k9Tools::processEvents();
        if ( !m_process->isRunning())
            return;
    }

    _cmd +="\n";
    m_canwrite=TRUE;
    m_process->write( _cmd.toUtf8() ,_cmd.length());

}
