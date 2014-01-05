//
// C++ Interface: k9videocodecs
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9VIDEOCODECS_H
#define K9VIDEOCODECS_H

#include "k9common.h"
#include <QObject>
#include <QMap>


/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class _k9VideoCodec;

class k9VideoCodecs : public QObject
{
Q_OBJECT
public:
    k9VideoCodecs(QObject *parent = 0);

    ~k9VideoCodecs();
    void save();
    void setFourcc(int _num,QString _value);
    void setOptions0(int _num,QString _value);
    void setOptions1(int _num,QString _value);
    void setOptions2(int _num,QString _value);
    void setCodecName(int _num,QString _value);
    void setEncoder(int _num,QString _value);

    void remove(int _num);
    QString getFourcc(int _num);
    QString getOptions0(int _num);
    QString getOptions1(int _num);
    QString getOptions2(int _num);
    QString getCodecName(int _num);
    QString getEncoder(int _num);

    int count();
    void reset();
    static int getAbsCodecNum(int _num,const QString &_encoder);

private:
    QMap <int,_k9VideoCodec> m_codecs;
    k9Config *m_config;


};

#endif
