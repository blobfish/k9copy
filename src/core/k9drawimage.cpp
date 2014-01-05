//
// C++ Implementation: k9drawimage
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9drawimage.h"
#include <QPainter>
#include <QApplication>

k9DrawImage::k9DrawImage ( QWidget *parent )
        : QWidget ( parent ) {

    m_repaint=false;
    connect ( &m_timer,SIGNAL ( timeout() ),this,SLOT ( timerDone() ) );
    m_timer.start ( 500 );
}


k9DrawImage::~k9DrawImage() {
}

void k9DrawImage::timerDone() {
    if ( m_repaint ) {
            update();
            m_repaint=false;
    }
}

void k9DrawImage::setImage ( const QImage &_image ) {
    m_mutex.lock();
//    m_pixmap=QPixmap::fromImage(_image);

    m_image=_image;
    m_mutex.unlock();
    m_repaint=true;
}

void k9DrawImage::setImage ( QString _fileName ) {
    m_mutex.lock();
    m_image.load ( _fileName );
    m_mutex.unlock();
    m_repaint=true;
 
}

void k9DrawImage::paintEvent ( QPaintEvent * ) {
	int top,left;
	QPainter p ( this );
	
	if ( m_mutex.tryLock() ) {
		m_pixmap=QPixmap::fromImage ( m_image );	
		double wratio= ( double ) width() / ( double ) m_pixmap.width();
		double hratio= ( double ) height() / ( double ) m_pixmap.height();
		double ratio= wratio < hratio ? wratio:hratio;
		
		top = ( int ) ( height() -m_pixmap.height() *ratio ) /2+1;
		left = ( int ) ( width() -m_pixmap.width() *ratio ) /2 ;
	
		p.scale ( ratio,ratio );
		p.drawPixmap ( ( int ) ( left/ratio ), ( int ) ( top/ratio ),m_pixmap );
		m_repaint=false;
		m_mutex.unlock();
	}
}
