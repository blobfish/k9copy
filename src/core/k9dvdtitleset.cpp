//
// C++ Implementation: k9dvdtitleset
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9dvdtitleset.h"


k9DVDTitleset::k9DVDTitleset( uint _num,uint32_t _size) { 
  m_num=_num;
  m_size=_size;
  m_selected=false;
  m_updating=false;
  for (uint i=0;i<8;i++)
    m_audioStreams[i]=false;
};


const QString k9DVDTitleset::getsize_mb()
{
  double size_mb=(double)m_size /512;
  QString c;
  c.sprintf("%.2f",size_mb);
  return c;
}

int k9DVDTitleset::getnum()
{
  return m_num;
}

void k9DVDTitleset::add(k9DVDTitle * _title)
{
  titles.append(_title);
}

int k9DVDTitleset::count()
{
  return titles.count();
}

k9DVDTitle * k9DVDTitleset::gettitle(uint _pos)
{
  return titles.at(_pos);
}

uint32_t k9DVDTitleset::getsize()
{
  return m_size;
}

bool k9DVDTitleset::getselected()
{
  return m_selected;
}

void k9DVDTitleset::setselected(bool _state)
{
  // selectionner / d�electionner tous les titres
  for (int i=0; i<count();i++)
  {
    k9DVDTitle *title=titles.at(i);
/*
    for (int j=0; j <title->getaudioStreamCount();j++)
      title->getaudioStream(j)->setselected(_state);
    for (int j=0;j<title->getsubPictureCount();j++)
      title->getsubtitle(j)->setselected(_state);
    title->setvideoSelected(_state);
*/
    title->setSelected(_state);
  }
  m_selected=_state;;
}

void k9DVDTitleset::updateSelection()
{
  if (m_updating)
    return;
  m_updating=true;


  bool selected=m_selected;
  m_selected=false;
  //check if at least on title is selected
  for (int i=0; i<count() && (!m_selected);i++)
  {
    k9DVDTitle *title=titles.at(i);
    if (title->isSelected() && title->getIndexed())
      m_selected=true;
  }

  //if titleset selected, we select all features. otherwise they are unselected
   if (selected !=m_selected)
    for (int i=0; i<count();i++)
    {
      k9DVDTitle *title=titles.at(i);
      //FIXED : parts of title selection
      if (title->getIndexed() && title->isSelected())
        for (int iTitle=0;iTitle <title->getTitles().count();iTitle++)
        {
          k9DVDTitle *title2=title->getTitles().at(iTitle);
          for (int j=0; j <title2->getaudioStreamCount();j++) 
            title2->getaudioStream(j)->setselected(title->getaudioStream(j)->getselected());
          for (int j=0;j<title2->getsubPictureCount();j++)
            title2->getsubtitle(j)->setselected(title2->getsubtitle(j)->getselected());
          title2->setvideoSelected(true);
        }
    }
  m_updating=false;
}

void k9DVDTitleset::setAudio(int _id,bool _selected) {
    m_audioStreams[_id -1]=_selected;
}

int k9DVDTitleset::getNewAudioStreamId(int _oldId) {
    int newId=0;
    //Returns the new stream Id, only if the stream is selected. Otherwise, returns 0
    if (m_audioStreams[_oldId-1]) {
/*        for (int i=0; i<_oldId;i++) {
            if (m_audioStreams[i])
                newId++;
        }
*/
        return _oldId;
    }
    return newId;
}

