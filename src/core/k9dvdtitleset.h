//
// C++ Interface: k9dvdtitleset
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9DVDTITLESET_H
#define K9DVDTITLESET_H

#include "k9common.h"
#include "k9dvd.h"
#include <QDateTime>
#include <QStringList>
#include "k9dvdtitle.h"
		
class k9DVDTitleset 
{
private:
  int m_num;
  uint32_t m_size;
  QList <k9DVDTitle*> titles;
  bool m_selected;
  bool m_updating;
  bool m_audioStreams[8];
public:
  k9DVDTitleset( uint _num,uint32_t _size); 
  virtual void add(k9DVDTitle *_title);
  virtual int count();
  virtual k9DVDTitle *gettitle(uint _pos);
  virtual uint32_t getsize();
  virtual const QString getsize_mb();
  virtual bool getselected();
  virtual void setselected(bool state);
  virtual void updateSelection();
  virtual int getnum();
  virtual void setAudio(int _id,bool _selected);
  virtual int getNewAudioStreamId(int _oldId);
  virtual ~k9DVDTitleset() {};
}
;

#endif
