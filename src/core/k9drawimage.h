//
// C++ Interface: k9drawimage
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9DRAWIMAGE_H
#define K9DRAWIMAGE_H

#include <QWidget>
#include <QPixmap>
#include <QMutex>
#include <QTimer>
#include <QImage>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9DrawImage : public QWidget
{
Q_OBJECT
public:
    k9DrawImage(QWidget *parent = 0);
    void setImage(QString _fileName);
    void setImage(const QImage &_image);
    ~k9DrawImage();
private:
    QPixmap m_pixmap;
    QImage m_image;
    QMutex m_mutex;
    bool m_repaint;
    QTimer m_timer;
protected:
    void paintEvent ( QPaintEvent *);
protected slots:
    void timerDone();
};

#endif
