/**************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
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
#include <qpixmap.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpaintdevice.h>
#include <qslider.h>
#include <qapplication.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <kdialog.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef BACKLITE
#include <kiconloader.h>
#endif
#include "kviewmpeg2.h"


k9Widget::k9Widget(QWidget *parent):QWidget(parent) {
}

void k9Widget::setImage(QImage *_image) {
    m_mutex.lock();
    m_image=*_image;
    m_mutex.unlock();
    //paintEvent( NULL);
    update();
}

void k9Widget::paintEvent( QPaintEvent *) {
//    setPaletteBackgroundColor(Qt::black);
    QPalette palette;
    palette.setColor(backgroundRole(), Qt::black);
    setPalette(palette);

    int top,left;
    if (m_mutex.tryLock()) {
        QPainter p(this);
        double wratio=(double)width()/(double)m_image.width();
        double hratio=(double)height()/(double)m_image.height();
        double ratio= wratio < hratio ? wratio:hratio;

        top =(int) (height() -m_image.height()*ratio)/2+1;
        left =(int) (width() -m_image.width()*ratio)/2 +1;

        p.scale(ratio,ratio);

        p.drawImage((int)(left/ratio),(int)(top/ratio),m_image);

        p.end();
	m_mutex.unlock();
    }
}

kViewMPEG2::kViewMPEG2(QWidget *parent):QWidget(parent) {
    Ui_viewMPEG2.setupUi(this);
    Ui_viewMPEG2.bPlay->setIcon(SmallIcon("media-playback-start"));
    Ui_viewMPEG2.bStop->setIcon(SmallIcon("media-playback-stop"));
    setWindowTitle(KDialog::makeStandardCaption(i18n("Title Preview"),this));
    m_widget=NULL;
    connect(&m_player  , SIGNAL(setPosition(uint)), this, SLOT(setPosition(uint)));
    connect(&m_player  , SIGNAL(setMax(uint)), this, SLOT(setMax(uint)));
    connect(&m_player  , SIGNAL(setMin(uint)), this, SLOT(setMin(uint)));
    connect(&m_timer    , SIGNAL(timeout()),this, SLOT(drawImage()));
    lockSlider=false;
    stopped=true;
    Ui_viewMPEG2.gbCrop->hide();
    m_layout=new QGridLayout(Ui_viewMPEG2.label);

    m_player.getDecoder()->setViewMpeg2(this);
//       connect(m_player.getDecoder()  , SIGNAL(pixmapReady(QImage *)), this, SLOT(drawPixmap(QImage *)));
        m_widget=new k9Widget(Ui_viewMPEG2.label);
        m_layout->addWidget(m_widget,0,0);

}


void kViewMPEG2::readSettings() {
    m_player.stop();
    //KSimpleConfig settings("K9Copy");
    k9Config config;

}


kViewMPEG2::~kViewMPEG2() {
    stopped=true;
    m_player.stop();
}


void kViewMPEG2::lock() {
    mutex.lock();
}

void kViewMPEG2::unlock() {
    mutex.unlock();
}

void kViewMPEG2::setPosition( uint _position) {
    if (!lockSlider)
        Ui_viewMPEG2.slider->setValue(_position);
}

void kViewMPEG2::setMax( uint _position) {
    Ui_viewMPEG2.slider->setMaximum(_position);
}

void kViewMPEG2::setMin(uint _position) {
    Ui_viewMPEG2.slider->setMinimum(_position);
}


void kViewMPEG2::drawImage(QImage *image) {
    QMutex mutex;
    img=*image;
    mutex.lock();
    m_wplay.wait(&mutex,40);
    m_widget->setImage( &img);
    
}

/** No descriptions */
void kViewMPEG2::drawImage() {
    /*if (qApp==NULL)
        return;
    if (m_mutex.tryLock()) {
        m_widget->setImage( &img);
        if (qApp!=NULL)
            m_mutex.unlock();
        }
   */
   m_wplay.wakeAll();
}


void kViewMPEG2::bPlayClick() {
    m_timer.start(40);
    m_player.play();
}


int kViewMPEG2::open (k9DVD *_dvd,k9DVDTitle * title,int chapter) {
    m_timer.start(40);
    bool play=m_player.isRunning();

    m_player.open(_dvd->getdvd()->getDvd(), _dvd->getDevice(),title,chapter);
    if (play) bPlayClick();
    return 1;
}


/** No descriptions */
void kViewMPEG2::setError(const QString & err) {
    error=true;
    errMsg=err;
}


void kViewMPEG2::resizeEvent(QResizeEvent *) {}

void kViewMPEG2::bStopClick() {
    m_player.stop();
}

void kViewMPEG2::sliderReleased() {
    pause=true;
    int i;
    i=Ui_viewMPEG2.slider->value();
    m_player.updatePos(i);
    lockSlider=false;

}

void kViewMPEG2::closeEvent( QCloseEvent* ce ) {
    stopped=true;
    ce->accept();
    return;

}
/** No descriptions */
void kViewMPEG2::sliderPressed() {
    lockSlider=true;
}
