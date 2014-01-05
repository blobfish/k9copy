//
// C++ Implementation: k9titleencopt
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
#include "k9titleencopt.h"
#include "k9dvdtitle.h"

k9TitleEncOpt::k9TitleEncOpt(k9DVDTitle *parent)
 : QObject(parent)
{
    connect (parent,SIGNAL(selectionChanged()),this,SLOT(load()));
    load();
}


k9TitleEncOpt::~k9TitleEncOpt()
{
}


void k9TitleEncOpt::load() {
    //KSimpleConfig settings("K9Copy");
    k9Config config;
    
    int size_mb=(int)getTitle()->gettotalsize_mb();
    m_crop.setCoords(0,0,getTitle()->getwidth().toInt()-1,getTitle()->getheight().toInt()-1);
    
    m_maxSize=config.getPrefMp4Size();
    if(m_maxSize  > size_mb)
       m_maxSize=size_mb ;
       
    m_numParts=config.getPrefMp4NumberCD();
    
    m_width=config.getPrefMp4Width();
    m_height=config.getPrefMp4Height();

    m_keepAspectRatio=config.getPrefMp4AspectRatio();

    m_2Passes=config.getPrefMp42Passes();

    m_audioBr=config.getPrefMp4AudioBitrate();

    m_codec=config.getPrefMp4Codec();

    m_audioCodec=config.getPrefMp4AudioCodec ();

    m_audioGain=config.getPrefMp4AudioGain();

    m_videoBr=config.getPrefMp4VideoBitrate();

    m_useCache=config.getPrefUseCellCache();

    m_encoder=config.getPrefMp4Encoder();
}

int k9TitleEncOpt::getMaxSize() {
   return m_maxSize;
}

void k9TitleEncOpt::setMaxSize(int _value) {
   m_maxSize=_value;
}


int k9TitleEncOpt::getNumParts() {
   return m_numParts;
}

void k9TitleEncOpt::setNumParts(int _value) {
   m_numParts=_value;
}

int k9TitleEncOpt::getAudioGain() {
   return m_audioGain;
}

void k9TitleEncOpt::setAudioGain(int _value) {
   m_audioGain=_value;
}

const QString & k9TitleEncOpt::getWidth() {
   return m_width;
}

void k9TitleEncOpt::setWidth(QString _value) {
   m_width=_value;
}


const QString & k9TitleEncOpt::getHeight() {
   return m_height;
}

void k9TitleEncOpt::setHeight(QString _value) {
   m_height=_value;
}

const QString & k9TitleEncOpt::getAudioBr() {
   return m_audioBr;
}
void k9TitleEncOpt::setAudioBr(QString _value) {
    m_audioBr=_value;
}

const QString & k9TitleEncOpt::getVideoBr() {
   return m_videoBr;
}
void k9TitleEncOpt::setVideoBr(QString _value) {
    m_videoBr=_value;
}

bool k9TitleEncOpt::getKeepAspectRatio() {
   return m_keepAspectRatio;
}
void k9TitleEncOpt::setKeepAspectRatio(bool _value) {
   m_keepAspectRatio=_value;
}

bool k9TitleEncOpt::get2Passes() {
   return m_2Passes;
}
void k9TitleEncOpt::set2Passes(bool _value) {
   m_2Passes=_value;
}

QRect *k9TitleEncOpt::getCrop() {
    return &m_crop;
}


int k9TitleEncOpt::getCodec() {
   return m_codec;
}
void k9TitleEncOpt::setCodec(int _value) {
   m_codec=_value;
}

int k9TitleEncOpt::getAudioCodec() {
   return m_audioCodec;
}
void k9TitleEncOpt::setAudioCodec(int _value) {
   m_audioCodec=_value;
}

bool k9TitleEncOpt::getUseCache()  {
    k9Config config;
    m_useCache=config.getPrefUseCellCache();
    return m_useCache;
}

void k9TitleEncOpt::setUseCache(bool _value) {
    m_useCache=_value;
}

void k9TitleEncOpt::setEncoder(const QString  &_value) {
   m_encoder=_value;
}

const QString & k9TitleEncOpt::getEncoder() {
   return m_encoder;
}
