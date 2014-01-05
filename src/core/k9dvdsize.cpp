//
// C++ Implementation: k9dvdsize
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9dvdsize.h"

int DVD_MAX_SIZE=4400;

k9DVDSize::k9DVDSize(QObject  *,const char *)
{
}

int k9DVDSize::getMaxSize(){
   return DVD_MAX_SIZE;
}
 
void k9DVDSize::setMaxSize(int _size){
   DVD_MAX_SIZE=_size;

}

int k9DVDSize::getMaxSizeDyn(){
   return DVD_MAX_SIZE;
}
 
void k9DVDSize::setMaxSizeDyn(int _size){
   DVD_MAX_SIZE=_size;

}


k9DVDSize::~k9DVDSize()
{
}


