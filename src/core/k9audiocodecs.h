//
// C++ Interface: k9audiocodecs
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9AUDIOCODECS_H
#define K9AUDIOCODECS_H

#include "k9common.h"
#include <QObject>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class _k9AudioCodec;
class k9AudioCodecs : public QObject
{
Q_OBJECT
public:
    k9AudioCodecs(QObject *parent = 0);

    ~k9AudioCodecs();
    void save();
    int count();
    void setOptions(int _num,QString _value);
    void setCodecName(int _num,QString _value);
    void setEncoder(int _num,QString _value);
    void setExtension(int _num,QString _value);
    QString getOptions(int _num);
    QString getCodecName(int _num);
    QString getEncoder(int _num);
    QString getExtension(int _num);
    void remove(int _num);
    void reset();
    static int getAbsCodecNum(int _num,const QString &_encoder);


private:
    QMap <int,_k9AudioCodec> m_codecs;
    k9Config *m_config;

};

#endif
