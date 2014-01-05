//
// C++ Implementation: k9dvdchapter
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9dvdchapter.h"
#include "k9dvdtitle.h"

//************************* k9DVDChapter************************
/** Read property of int num. */
const int& k9DVDChapter::getnum() {
    return num;
}
/** Read property of QTime length. */
const QTime& k9DVDChapter::getLength() {
    return m_length;
}

/** Read property of int sectors. */
const int& k9DVDChapter::getsectors() {
    return sectors;
}

const QTime& k9DVDChapter::getTime() {
    return m_time;
}
k9DVDTitle *k9DVDChapter::getTitle() {
    return m_title;
}


void k9DVDChapter::clearCells() {
//   while (!cells.isEmpty()) delete cells.takeFirst();
    qDeleteAll(cells);
    cells.clear();
}

k9DVDChapter::~k9DVDChapter() {
    clearCells();
}

k9DVDChapter::k9DVDChapter():QObject() {
    m_length.setHMS(0,0,0);
    num=0;
    sectors=0;
    startSector=0;
    endSector=0;
    m_selected=false;
    m_time.setHMS(0,0,0);
}


bool k9DVDChapter::getSelected() const {
    return m_selected;
}


void k9DVDChapter::setSelected(bool _value) {
    if (_value &&  !m_title->isSelected() ) {
        m_title->setvideoSelected( true,false);
       // m_title->selectChapters( false);
    }
    m_selected = _value;
    emit selectionChanged();
}


void k9DVDChapter::setLength(const QTime& theValue) {
    m_length = theValue;
}


void k9DVDChapter::setTime(const QTime& theValue) {
    m_time = theValue;
}
