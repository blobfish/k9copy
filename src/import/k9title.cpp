//
// C++ Implementation: k9title
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9title.h"
#include "k9newdvd.h"
#include "k9menu.h"

k9Title::k9Title(k9NewDVD *parent, const char *)
 : QObject(parent)
{
    m_num=parent->getTitles()->count();
    parent->appendTitle(this);
    m_menu=new k9Menu(this,0);
    m_menu->setFormat((k9Menu::eFormat)parent->getFormat());
}


k9Title::~k9Title()
{
   k9NewDVD *p=(k9NewDVD*)parent();
   // p->getRootMenu()->removeButton(m_button);
}

bool k9TitleItems::compareItems(k9AviFile *item1,k9AviFile *item2) {
    return (item1->getNum() < item2->getNum());
}


k9TitleItems *k9Title::getFiles() 
{
	return &m_files;
}


int k9Title::getNum() const
{
	return m_num;
}


k9MenuButton* k9Title::getButton() const {
    return m_button;
}


void k9Title::setButton(k9MenuButton* _value) {
    m_button = _value;
}


k9Menu* k9Title::getMenu() const {
    return m_menu;
}
