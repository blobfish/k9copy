//
// C++ Interface: k9titleencopt
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9TITLEENCOPT_H
#define K9TITLEENCOPT_H

#include <QObject>
#include <QRect>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9DVDTitle;

class k9TitleEncOpt : public QObject
{
Q_OBJECT
public:
    k9TitleEncOpt(k9DVDTitle *parent );
    ~k9TitleEncOpt();
   k9DVDTitle *getTitle() { return (k9DVDTitle*)parent();}
    
public:
   int getMaxSize();
   void setMaxSize(int);
   
   int getNumParts();
   void setNumParts(int);

   int getAudioGain();
   void setAudioGain(int);
   
   const QString & getWidth();
   void setWidth(QString);
   
   const QString & getHeight();
   void setHeight(QString);
   
   const QString & getAudioBr();
   void setAudioBr(QString);

   const QString & getVideoBr();
   void setVideoBr(QString);
   
   bool getKeepAspectRatio();
   void setKeepAspectRatio(bool);
   
   bool get2Passes();
   void set2Passes(bool);

   bool getUseCache();
   void setUseCache(bool);
   
   int getCodec();
   void setCodec(int);

   int getAudioCodec();
   void setAudioCodec(int);


   const QString & getEncoder();
   void setEncoder(const QString &);

   QRect *getCrop();
private:
   int m_maxSize;
   int m_numParts;
   QString m_width;
   QString m_height;
   bool m_keepAspectRatio;
   bool m_2Passes;
   QString m_audioBr,m_videoBr;
   int m_codec,m_audioCodec;
   int m_audioGain;
   bool m_useCache;
   QString m_encoder;
   QRect m_crop;
public slots:
   void load();
   
};

#endif
