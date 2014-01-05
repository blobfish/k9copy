//
// C++ Interface: k9ffmpegcmdgen
//
// Description:
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9FFMPEGCMDGEN_H
#define K9FFMPEGCMDGEN_H

#include "k9common.h"
#include "ui_ffmpegCmdGen.h"

class k9ffmpegCmdGen : public QWidget {
    Q_OBJECT

public:
    enum FFMpegCmdType {AUDIO,VIDEO};
    k9ffmpegCmdGen(QWidget* parent = 0 );
    ~k9ffmpegCmdGen();
    /*$PUBLIC_FUNCTIONS$*/
    static bool getFFMpegCmd(FFMpegCmdType _type,QString &_cmd);
    void show (FFMpegCmdType _type);

    void setCmd(const QString& theValue);
    QString getCmd() ;

public slots:
    /*$PUBLIC_SLOTS$*/

private:
    Ui::ffmpegCmdGen Ui_ffmpegCmdGen;
    FFMpegCmdType m_type;
    QString m_cmd;
    QString m_codecs;
    void readCodecs();
protected slots:
   void getStdout(char*,int);
};


#endif

