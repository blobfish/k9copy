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

#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef BACKLITE
#include <kiconloader.h>
#endif
#include "k9cropselect.h"
#include "kviewmpeg2.h"
#include <kdialog.h>
#include <math.h>

k9CropSelect::k9CropSelect(QWidget *parent):QWidget(parent) {
    Ui_viewMPEG2.setupUi(this);

    setWindowTitle(KDialog::makeStandardCaption(i18n("Crop selection"),this));
    Ui_viewMPEG2.bPlay->setIcon(SmallIcon("media-playback-start"));
    Ui_viewMPEG2.bStop->setIcon(SmallIcon("media-playback-stop"));

    m_widget=NULL;    
    connect(&m_player  , SIGNAL(setPosition(uint)), this, SLOT(setPosition(uint)));
    connect(&m_player  , SIGNAL(setMax(uint)), this, SLOT(setMax(uint)));
    connect(&m_player  , SIGNAL(setMin(uint)), this, SLOT(setMin(uint)));
    connect(&m_timer    , SIGNAL(timeout()),this, SLOT(drawImage()));
    lockSlider=false;
    stopped=true;
    m_layout=new QGridLayout(Ui_viewMPEG2.label);

    m_player.getDecoder()->setViewMpeg2(this);
//       connect(m_player.getDecoder()  , SIGNAL(pixmapReady(QImage *)), this, SLOT(drawPixmap(QImage *)));
        m_widget=new k9Widget(Ui_viewMPEG2.label);
        m_layout->addWidget(m_widget,0,0);
   m_drawing=false;
   
}

void k9CropSelect::setRect(QRect _crop) {
   Ui_viewMPEG2.sbTop->setValue(_crop.top());
   Ui_viewMPEG2.sbLeft->setValue(_crop.left());
   Ui_viewMPEG2.sbRight->setValue(_crop.right());
   Ui_viewMPEG2.sbBottom->setValue(_crop.bottom());
   m_rect=_crop;
}

QRect k9CropSelect::getRect() {
   QRect r;
   r.setCoords(Ui_viewMPEG2.sbLeft->value(),Ui_viewMPEG2.sbTop->value(),Ui_viewMPEG2.sbRight->value(),Ui_viewMPEG2.sbBottom->value());
   return r;
}

void k9CropSelect::readSettings() {
    m_player.stop();
    //KSimpleConfig settings("K9Copy");
    k9Config config;

}


k9CropSelect::~k9CropSelect() {
    stopped=true;
    m_player.stop();
}


void k9CropSelect::lock() {
    mutex.lock();
}

void k9CropSelect::unlock() {
    mutex.unlock();
}

void k9CropSelect::setPosition( uint _position) {
    if (!lockSlider)
        Ui_viewMPEG2.slider->setValue(_position);
}

void k9CropSelect::setMax( uint _position) {
    Ui_viewMPEG2.slider->setMaximum(_position);
}

void k9CropSelect::setMin(uint _position) {
    Ui_viewMPEG2.slider->setMinimum(_position);
}

double k9CropSelect::getDistance(QRgb color1,QRgb color2) {
	int B1,B2,R1,R2,G1,G2,A1,A2;
	QColor c1 (color1);
 	c1.getRgb (&R1,&G1,&B1,&A1);
	QColor c2 (color2);
	c2.getRgb(&R2,&G2,&B2,&A2);
	int distance= sqrt(pow((double)R1-(double)R2,2)) + sqrt(pow((double)G1-(double)G2,2)) +sqrt(pow((double)B1-(double)B2,2));
  	
	return distance==255. ? 0. : distance;
//Distance(C1,C2) = sqrt((R1-R2)² + (G1-G2)² + (B1-B2)²)  
}

void k9CropSelect::bCropDetectClick() {
   lock();
   int width=img.width();
   int height=img.height();
   int top=0,bottom=height,left=0,right=width;
 
   //scanning top crop
   bool found=false;
   for (int y=2; y <(height/2) && !found;y++) {
   	for ( int x=0; x<(width/2) && !found;x++) { 
	  double distance=getDistance(m_origImage.pixel(1,1),m_origImage.pixel(x,y));
          if (distance>10.)
	  {
	    top=y;
	    found=true;
	  }
	 
	}
   }
   //scanning bottom crop
   found=false;
   for (int y=height-2; y >(height/2) && !found;y--) {
   	for ( int x=0; x<(width/2) && !found;x++) { 
	  double distance=getDistance(m_origImage.pixel(1,1),m_origImage.pixel(x,y));
          if (distance>10.)
	  {
	    bottom=y;
	    found=true;
	  }
	 
	}
   }

   //scanning Left crop
   found=false;
   for ( int x=0; x<(width/2) && !found;x++) { 
      for (int y=0; y <(height/2) && !found;y--) {
	  double distance=getDistance(m_origImage.pixel(1,1),m_origImage.pixel(x,y));
          if (distance>10.)
	  {
	    left=x;
	    found=true;
	  }
	 
	}
   }

   //scanning Right crop
   found=false;
   for ( int x=(width-2); x>(width/2) && !found;x++) { 
      for (int y=0; y <(height/2) && !found;y--) {
	  double distance=getDistance(m_origImage.pixel(1,1),m_origImage.pixel(x,y));
          if (distance>10.)
	  {
	    right=x;
	    found=true;
	  }
	 
	}
   }

   unlock();
   Ui_viewMPEG2.sbTop->setValue(top);
   Ui_viewMPEG2.sbBottom->setValue(bottom);
   Ui_viewMPEG2.sbLeft->setValue(left);
   Ui_viewMPEG2.sbRight->setValue(right);

}

void k9CropSelect::sbValueChanged(int){
    lock();
    m_rect.setCoords(Ui_viewMPEG2.sbLeft->value(),Ui_viewMPEG2.sbTop->value(),Ui_viewMPEG2.sbRight->value(),Ui_viewMPEG2.sbBottom->value());
    unlock();
    if (!m_player.isRunning()) {
	drawImage(&m_origImage);
    }
}
void k9CropSelect::drawCropping() {
    QPainter p(&img);
    p.setOpacity(0.5);

    QBrush br(Qt::yellow);
    p.setBrush(br);
    p.fillRect(0,0,img.width(),m_rect.top(),br);
    p.fillRect(0,m_rect.bottom(),img.width(),img.height()-m_rect.bottom(),br);
    p.fillRect(0,0,m_rect.left(),img.height(),br);
    p.fillRect(m_rect.right(),0,img.width()-m_rect.right(),img.height(),br);

    p.end();
 
}

void k9CropSelect::drawImage(QImage *image) {
    if (m_drawing)
	return;
    lock();
    m_drawing=true;
    QMutex mutex;
    mutex.lock();
    img=*image;

    m_origImage=img.copy();

    drawCropping();

    m_wplay.wait(&mutex,40);

    m_widget->setImage( &img);
    m_drawing=false;
    unlock();
}

/** No descriptions */
void k9CropSelect::drawImage() {
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


void k9CropSelect::bPlayClick() {
    m_timer.start(40);
    m_player.play();
}


int k9CropSelect::open (k9DVD *_dvd,k9DVDTitle * title,int chapter) {
    Ui_viewMPEG2.sbLeft->setMaximum(title->getwidth().toInt()/2);
    Ui_viewMPEG2.sbRight->setMaximum(title->getwidth().toInt()-1);
    Ui_viewMPEG2.sbRight->setMinimum(title->getwidth().toInt()/2);
    Ui_viewMPEG2.sbTop->setMaximum(title->getheight().toInt()/2);
    Ui_viewMPEG2.sbBottom->setMinimum(title->getheight().toInt()/2);
    Ui_viewMPEG2.sbBottom->setMaximum(title->getheight().toInt()-1);
    m_timer.start(40);
    bool play=m_player.isRunning();

    m_player.open(_dvd->getdvd()->getDvd(), _dvd->getDevice(),title,chapter);
    if (play) bPlayClick();
    return 1;
}


/** No descriptions */
void k9CropSelect::setError(const QString & err) {
    error=true;
    errMsg=err;
}


void k9CropSelect::resizeEvent(QResizeEvent *) {}

void k9CropSelect::bStopClick() {
    m_player.stop();
}

void k9CropSelect::sliderReleased() {
    pause=true;
    int i;
    i=Ui_viewMPEG2.slider->value();
    m_player.updatePos(i);
    lockSlider=false;

}

void k9CropSelect::closeEvent( QCloseEvent* ce ) {
    stopped=true;
    ce->accept();
    return;

}
/** No descriptions */
void k9CropSelect::sliderPressed() {
    lockSlider=true;
}
