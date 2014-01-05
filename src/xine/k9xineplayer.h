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
#ifndef K9XINEPLAYER_H
#define K9XINEPLAYER_H

#include <QDBusAbstractAdaptor>
#include <QWidget>
#include <xine.h>
#include <xine/xineutils.h>
#include <QMutex>


/**
	@author Jean-Michel <jm_petit@laposte.net>
*/

class k9xineThread;
class k9xinePlayer : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "k9copy.free.fr.xineplayer")
    Q_PROPERTY(QString mrl READ mrl WRITE setMrl)
    Q_PROPERTY(int width READ width WRITE setWidth)
    Q_PROPERTY(int height READ height WRITE setHeight)
    Q_PROPERTY(int position READ position WRITE seek)
    Q_PROPERTY(int positionAbs READ positionAbs WRITE seek)
    Q_PROPERTY(int length READ length);
    Q_PROPERTY(int isRunning READ isRunning)
    Q_PROPERTY(QString device READ device WRITE setDevice)
public:
    k9xinePlayer(QObject *parent = 0);
    ~k9xinePlayer();
    void init(WId _wid);
    QString mrl() const {return m_mrl;}
    void setMrl(const QString &value) {m_mrl=value;}
    QString device() const {return m_device;}
    void setDevice (const QString &value) { 
		updateConfigEntry (QString ("input.dvd_device"), value);
		m_device=value;
    }
    int width() {return m_width;}
    void setWidth(int _value);
    int height() { return m_height;}
    void setHeight(int _value);
    double pixelAspect() { return m_pixelAspect;}
    int xpos() {return m_xpos;}
    void setXpos(int _value) { m_xpos=_value;}
    void setYpos(int _value) { m_ypos=_value;}
    int ypos() {return m_ypos;}
    int position() {return m_pos;}
    int positionAbs() {return m_posabs ;}
    int length() {return m_length;}
    xine_stream_t *getStream() { return stream;}
    xine_video_port_t   *getvoport(){return vo_port;}
    bool isRunning() {return running;}
    WId wid() const { return m_wid;}
    QString title() const {return m_title;}
    void setTitle(const QString &_title) {m_title=_title;}
    void setVo(const QString& theValue) { m_vo = theValue;}
    void setAo(const QString& theValue) {m_ao = theValue;}
public slots:
    void setRunning(bool _value) { running=_value;}
    void setAudioLang (int id);
    void setSubtitle (int id);
    void setVolume (int val);
    void volumeUp();
    void volumeDown();
    void stop();
    void play();
    void pause();
    void quit();
    void execute();
    void update();
    void updatePosition();
private:
    QString m_mrl;
    QString m_device;
    WId m_wid;
    Display *display;
    int  screen;
    int  m_xpos, m_ypos, m_width, m_height;
    double m_pixelAspect;
    bool running ;
    int m_length;
    int m_pos;
    int m_posabs;
    bool m_execute;
    QMutex m_mutex;
    QString m_title;
    xine_t              *xine;  
    xine_stream_t       *stream;
    xine_video_port_t   *vo_port;
    xine_audio_port_t   *ao_port;
    QString m_vo;
    QString m_ao;
    xine_event_queue_t  *event_queue;
    k9xineThread *m_thread;
    void end();
    void seek(int _position);
    bool updateConfigEntry (const QString & name, const QString & value);
};

#endif
