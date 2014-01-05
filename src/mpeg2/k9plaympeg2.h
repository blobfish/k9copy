//
// C++ Interface: k9plaympeg2
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9PLAYMPEG2_H
#define K9PLAYMPEG2_H

#include "k9common.h"
#include "k9dvdtitle.h"


#include <QThread>
#include <QObject>
#include <QMutex>
#include <QWidget>
#include <QTimer>
#include "k9decodethread.h"
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9PlayMPEG2 : public QThread
{
Q_OBJECT
private:
   k9DVDTitle *m_title;
   QString m_device;
   k9DecodeThread m_decoder;
   uint32_t m_idxLect;
   uint32_t m_startSector;
   uint32_t m_lastSector;
   bool m_stopped;
   dvd_file_t *dvdfile;
   QMutex mutex;
   dvd_reader_t *m_dvd;
   int m_chapter;
   int m_position;
   QTimer m_timer;
public:
    k9PlayMPEG2();
    void open (dvd_reader_t *dvd,const QString & device,k9DVDTitle * title,int chapter);
    k9DecodeThread *getDecoder();
    void setDvd(dvd_reader_t* _value) { m_dvd = _value;}
    dvd_reader_t* getDvd() const {return m_dvd;}
    ~k9PlayMPEG2();
protected:
    void run();
    void playTitle();
public slots:
  void stop();
  void play();
  void updatePos(uint32_t _position);	
  void timeout();
	
signals:
  void setPosition(uint _position);
  void setMin(uint _sector);
  void setMax(uint _sector);

  void setError(const QString & err);

};

#endif
