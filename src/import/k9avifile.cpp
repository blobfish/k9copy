//
// C++ Implementation: k9avifile
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9common.h"
#include "k9avifile.h"
#include "k9title.h"

k9AviFile::k9AviFile(k9Title *parent, const char *)
        : QObject(parent) {
    m_num=parent->getFiles()->count();
    setTitle(parent);
    m_previous=NULL;
    m_next=NULL;
    m_breakNext=false;
    m_breakPrevious=false;
    m_reencode=true;
}


k9AviFile::~k9AviFile() {
}


QString k9AviFile::getFileName() const {
    return m_fileName;
}


void k9AviFile::setFileName ( const QString& _value ) {
    if (m_fileName!="") {
        m_breakNext=true;
        m_breakPrevious=true;
	if (m_next)
	   m_next->setBreakPrevious(true);
	if (m_previous)
	   m_previous->setBreakNext(true);

    }
    m_fileName = _value;
    emit aviFileUpdated  (this);
}


QTime k9AviFile::getStart() const {
    return m_start;
}


void k9AviFile::setStart ( const QTime& _value ) {
    m_start = _value;
    emit aviFileUpdated  (this);
}


QTime k9AviFile::getEnd() const {
    return m_end;
}


void k9AviFile::setEnd ( const QTime& _value ) {
    m_end = _value;
    emit aviFileUpdated  (this);
}


QString k9AviFile::getLabel() const {
    return m_label;
}


void k9AviFile::setLabel ( const QString& _value ) {
    m_label = _value;
    emit aviFileUpdated  (this);
}


k9Title* k9AviFile::getTitle() const {
    return m_title;
}


void k9AviFile::setTitle ( k9Title* _value ) {
    m_title = _value;
    m_title->getFiles()->append(this);
    qSort(m_title->getFiles()->begin(),m_title->getFiles()->end(),k9TitleItems::compareItems);
    // m_title->getFiles()->sort();
}


int k9AviFile::getNum() const {
    return m_num;
}




k9AviFile* k9AviFile::getPrevious() const {
    return m_previous;
}


void k9AviFile::setPrevious ( k9AviFile* _value ) {
    m_previous = _value;
    m_breakPrevious=false;
}


k9AviFile* k9AviFile::getNext() const {
    return m_next;
}


void k9AviFile::setNext ( k9AviFile* _value ) {
    m_next = _value;
    m_breakPrevious=false;
}

void k9AviFile::setBreakPrevious(bool _state) {
    m_breakPrevious=_state;
    if ( ! _state) {
        if (m_previous !=NULL) {
            m_previous->setEnd(this->getStart());
            m_previous->m_breakNext=false;
        }
    } else {
        if (m_previous!=NULL) {
            m_previous ->m_breakNext=true;
        }
    }

}

bool k9AviFile::getBreakPrevious() {
    return m_breakPrevious;
}

void k9AviFile::setBreakNext(bool _state) {
    m_breakNext=_state;
    if ( ! _state) {
        if (m_next !=NULL) {
            m_next->setStart(this->getEnd());
            m_next->m_breakPrevious=false;
        }
    } else {
        if (m_next!=NULL) {
            m_next ->m_breakPrevious=true;
        }
    }


}

bool k9AviFile::getBreakNext() {
    return m_breakNext;
}



QImage k9AviFile::getImage() const {
    return m_image;
}


void k9AviFile::setImage(const QImage& _value) {
    m_image = _value;
    emit aviFileUpdated  (this);
    emit imageChanged(_value);
}


k9MenuButton* k9AviFile::getButton() const {
    return m_button;
}


void k9AviFile::setButton(k9MenuButton* theValue) {
    m_button = theValue;
}


bool k9AviFile::getReencode() const {
    return m_reencode;
}


void k9AviFile::setReencode(bool theValue) {
    m_reencode = theValue;
}
