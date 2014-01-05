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
#include "k9xineplayer.h"
#include <KApplication>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <QTimer>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/X.h>
#include <QThread>
#include <KMessageBox>

#define INPUT_MOTION (ExposureMask | ButtonPressMask | KeyPressMask | \
                      ButtonMotionMask | StructureNotifyMask |        \
                      PropertyChangeMask | PointerMotionMask)

class k9xineThread : public QThread {
public:
    k9xineThread(k9xinePlayer *_player) : QThread() {
        m_player=_player;
    }
    void stop() {
        m_execute=false;
    }
    void setDisplay(Display *_display) {
        display=_display;
    }
private:
    k9xinePlayer *m_player;
    bool m_execute;
    Display *display;
protected:
    void run ();
};


static void dest_size_cb(void *data, int , int , double ,int *dest_width, int *dest_height, double *dest_pixel_aspect)  {
    k9xinePlayer *player=(k9xinePlayer*)data;
    *dest_width        = player->width();
    *dest_height       = player->height();
    *dest_pixel_aspect = player->pixelAspect();
}

static void frame_output_cb(void *data, int video_width, int video_height,
                            double video_pixel_aspect, int *dest_x, int *dest_y,
                            int *dest_width, int *dest_height,
                            double *dest_pixel_aspect, int *win_x, int *win_y) {

    k9xinePlayer *player=(k9xinePlayer*)data;

    *dest_x            = 0;
    *dest_y            = 0;
    *win_x             = player->xpos();
    *win_y             = player->ypos();
    *dest_width        = player->width();
    *dest_height       = player->height();
    *dest_pixel_aspect = player->pixelAspect();
}

static void event_listener(void *user_data, const xine_event_t *event) {
    k9xinePlayer *player=(k9xinePlayer*)user_data;

    switch (event->type) {
    case XINE_EVENT_UI_PLAYBACK_FINISHED:
        player->setRunning (false);
        break;
    case XINE_EVENT_UI_SET_TITLE:
	xine_ui_data_t * data = (xine_ui_data_t *) event->data;
        QString s(data->str);
	if (player->title() !="" && s!=player->title())
	    player->stop();
	else
	    player->setTitle(s);
	break;
    }
}


k9xinePlayer::k9xinePlayer(QObject *parent)
        : QDBusAbstractAdaptor(parent) {
    m_thread=new k9xineThread(this);
    running=false;
}


k9xinePlayer::~k9xinePlayer() {
}

void k9xinePlayer::setWidth(int _value) {
    bool changed=m_width!=_value;
    m_width=_value;
    if (changed)
        XResizeWindow(display, m_wid, m_width, m_height);
}

void k9xinePlayer::setHeight(int _value) {
    bool changed=m_height!=_value;
    m_height=_value;
    if (changed)
        XResizeWindow(display, m_wid, m_width, m_height);
}

bool k9xinePlayer::updateConfigEntry (const QString & name, const QString & value) {
    bool changed = false;
    xine_cfg_entry_t cfg_entry;
    if (!xine_config_lookup_entry (xine, name.toAscii (), &cfg_entry))
        return false;
    if (cfg_entry.type == XINE_CONFIG_TYPE_STRING ||
            cfg_entry.type == XINE_CONFIG_TYPE_UNKNOWN) {
        changed = strcmp (cfg_entry.str_value, value.toAscii ());
        cfg_entry.str_value = (char *) value.toAscii ().data();
    } else {
        changed = cfg_entry.num_value != value.toInt ();
        cfg_entry.num_value = value.toInt ();
    }
    xine_config_update_entry (xine,  &cfg_entry);
    return changed;
}

void k9xinePlayer::stop() {
    if (!running) return;
    xine_stop (stream);
    running=false;
}

void k9xinePlayer::pause () {
    if (!running) return;
    if (xine_get_status (stream) == XINE_STATUS_PLAY) {
        if (xine_get_param (stream, XINE_PARAM_SPEED) == XINE_SPEED_PAUSE)
            xine_set_param (stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
        else
            xine_set_param (stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
    }
}

void k9xinePlayer::setAudioLang (int id) {
    m_mutex.lock();
    if (xine_get_status (stream) == XINE_STATUS_PLAY)
        xine_set_param(stream, XINE_PARAM_AUDIO_CHANNEL_LOGICAL, id);
    m_mutex.unlock();
}

void k9xinePlayer::setSubtitle (int id) {
    m_mutex.lock();
    if (xine_get_status (stream) == XINE_STATUS_PLAY) {
        xine_set_param (stream, XINE_PARAM_SPU_CHANNEL, id);
    }
    m_mutex.unlock();

}

void k9xinePlayer::updatePosition () {
    if (running)  { 
	m_mutex.lock();
	xine_get_pos_length (stream, 0, &m_pos, &m_length);
	xine_get_pos_length (stream, &m_posabs,0,0);
	m_mutex.unlock();
    }
    QTimer::singleShot (500, this, SLOT (updatePosition ()));
}

void k9xinePlayer::setVolume (int val) {
    if (running) {
        xine_set_param( stream, XINE_PARAM_AUDIO_VOLUME, val);
    }
}

void k9xinePlayer::volumeUp() {
    if (running)
        xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME,(xine_get_param(stream, XINE_PARAM_AUDIO_VOLUME) + 10));
}
void k9xinePlayer::volumeDown() {
    if (running)
        xine_set_param(stream, XINE_PARAM_AUDIO_VOLUME,(xine_get_param(stream, XINE_PARAM_AUDIO_VOLUME) - 10));
}


void k9xinePlayer::seek (int val) {
        if (m_mutex.tryLock()) {
		xine_play (stream, val,0);
  		running=true;
		m_mutex.unlock();
	}
}

void k9xinePlayer::init(WId _wid) {
    char              configfile[2048];
    x11_visual_t      vis;
    double            res_h, res_v;

    if (!XInitThreads()) {
        printf("XInitThreads() failed\n");
        return ;
    }

    xine = xine_new();
    sprintf(configfile, "%s%s", xine_get_homedir(), "/.xine/config");
    xine_config_load(xine, configfile);
    xine_init(xine);

    display=XOpenDisplay(NULL);
    screen       = XDefaultScreen(display);
    m_xpos         = 0;
    m_ypos         = 0;
    m_width        = 320;
    m_height       = 200;

    XLockDisplay(display);
    m_wid = XCreateSimpleWindow(display, _wid,m_xpos, m_ypos, m_width, m_height, 1, 0, 0);
    XSelectInput (display, _wid, INPUT_MOTION);
    XSelectInput (display, m_wid, INPUT_MOTION);

    XMapRaised(display, m_wid);

    res_h = (DisplayWidth(display, screen) * 1000 / DisplayWidthMM(display, screen));
    res_v = (DisplayHeight(display, screen) * 1000 / DisplayHeightMM(display, screen));
    XSync(display, False);
    XUnlockDisplay(display);

    vis.display           = display;
    vis.screen            = screen;
    vis.d                 = m_wid;
    vis.dest_size_cb      = dest_size_cb;
    vis.frame_output_cb   = frame_output_cb;
    vis.user_data         = this;
    m_pixelAspect          = res_v / res_h;

    if (fabs(m_pixelAspect - 1.0) < 0.01)
        m_pixelAspect = 1.0;

    if ((vo_port = xine_open_video_driver(xine,
                                          m_vo.toAscii(), XINE_VISUAL_TYPE_X11, (void *) &vis)) == NULL) {
      //  printf("I'm unable to initialize '%s' video driver. Giving up.\n", m_vo.toAscii());
      //  return ;
    }

    ao_port     = xine_open_audio_driver(xine , m_ao.toAscii(), NULL);
    stream      = xine_stream_new(xine, ao_port, vo_port);
    event_queue = xine_event_new_queue(stream);
    xine_event_create_listener_thread(event_queue, event_listener, this);

    xine_port_send_gui_data(vo_port, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) m_wid);
    xine_port_send_gui_data(vo_port, XINE_GUI_SEND_VIDEOWIN_VISIBLE, (void *) 1);
    m_execute=true;

}

void k9xinePlayer::play() {
    running=false;
    m_title="";
    if ((!xine_open(stream, m_mrl.toUtf8())) || (!xine_play(stream, 0, 0))) {
    //    printf("Unable to open mrl '%s'\n", m_mrl.toUtf8());
        return ;
    }
    QTimer::singleShot (500, this, SLOT (updatePosition ()));
    running = true;
}

void k9xinePlayer::quit() {
    end();
}

void k9xinePlayer::update() {
    if (stream && !running ) {
        if (m_mutex.tryLock()) {
            QApplication::syncX();
           xine_port_send_gui_data(vo_port , XINE_GUI_SEND_DRAWABLE_CHANGED, (void *) m_wid);
           m_mutex.unlock();
        }
    }
}

void k9xinePlayer::execute() {
    m_thread->setDisplay(display);
    m_thread->start();
  //  kapp->exec();
}
void k9xineThread::run() {
    m_execute=true; 
    while (m_execute) {
        XEvent xevent;
        int    got_event;

        XLockDisplay(display);
        got_event = XPending(display);
        if ( got_event )
            XNextEvent(display, &xevent);
        XUnlockDisplay(display);

        if ( !got_event ) {
            xine_usec_sleep(2000);
            continue;
        }
        m_player->update();

        switch (xevent.type) {

        case Expose:
            if (xevent.xexpose.count != 0)
                break;
            xine_port_send_gui_data(m_player->getvoport(), XINE_GUI_SEND_EXPOSE_EVENT, &xevent);
            break;
        }
    }
}


void k9xinePlayer::end() {
    stop();
    m_mutex.lock();
    if (m_thread->isRunning()) {
        m_thread->stop();
        m_thread->terminate();
        m_thread->wait();
    }

    xine_close(stream);
    xine_event_dispose_queue(event_queue);
    xine_dispose(stream);

    if (ao_port)
        xine_close_audio_driver(xine, ao_port);
    xine_close_video_driver(xine, vo_port);
    xine_exit(xine);

    XLockDisplay(display);
    XDestroyWindow(display,  m_wid);
    XUnlockDisplay(display);
    XFlush (display);

    XCloseDisplay (display);
    m_mutex.unlock();
    delete m_thread;
    kapp->exit();
}


