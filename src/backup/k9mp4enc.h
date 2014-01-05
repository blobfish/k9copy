//
// C++ Interface: k9mp4enc
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9MP4ENC_H
#define K9MP4ENC_H
#include "k9common.h"
#include "k9dvdtitle.h"
#include "k9dvdbackupinterface.h"
#include "k9subtitle2pgm.h"
#include <qobject.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <QObject>
#include "k9play.h"
#include <QEventLoop>
#include <QWaitCondition>
class k9MP4Dlg;

class k9ResultingFile:public QObject {
public:
    k9ResultingFile(QObject *parent):QObject(parent) {
        title=0;
        audio=false;
        video=false;
   	subtitle=false;
        filename="";
    };

    int title;
    bool audio;
    bool video;
    bool subtitle;
    QString filename;
};

class k9ConvertAudio;
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9MP4Enc : public k9DVDBackupInterface {
    Q_OBJECT
private:
    enum eEncoder {FFMPEG,MENCODER};
    k9MP4Dlg  *m_progress;
    k9DVDTitle *m_title;
    int getBitRate(k9DVDTitle *_title);
    QString m_txt;
    QString m_device;
    QString m_filename;
    QString m_height;
    QString m_width;
    QString m_size;
    QString m_audioBitrate;
    QString m_videoBitrate;
    QString m_audioGain;
    QString m_fourcc;
    QString m_stderr;
    QString m_passLogFile;
    QString m_ffmpegPath;
    QString m_mencoderPath;
    eEncoder m_encoder;
    QRect m_crop;
    int m_parts,m_part;
    float m_percent;
    QTimer *timer;
    QString m_remain;
    uint32_t m_totalSize;
    uint32_t m_offset;
    uint32_t m_totalBytes;
    QString m_aspect;
    bool m_2pass;
    int m_pass;
    bool m_canceled;
    bool m_error;
    QString m_msgError;
    QStringList m_ErrorDetail;
    int m_codec;
    int m_audioCodec;
    bool m_usecache;
    bool m_mpeg2;
    bool m_extractAudio;
    bool m_extractSubtitle;
    bool m_extractMkv;
    bool m_splitChapters;

    int m_cpt;
    int m_currentChapter;
    QTime *time;
    QString replaceParams(QString _input);
    void Flush();
    bool check();
    void buildMEncoderCmd(int pass,int part,k9DVDTitle *title,const QString &passLogFileName,QStringList &cmd) ;
    void buildVideoCmd(int pass,int part,k9DVDTitle *title,const QString &passLogFileName,QStringList &cmd) ;
    void buildFFMpegCmd(int pass,k9DVDTitle *title,QStringList &cmd) ;
    void buildAudioCmd(k9DVDTitle *title);
    void buildSubtitleCmd(k9DVDTitle *title);
    QString round16(QString _wh);
    QString getChapterList(k9DVDTitle *_title);
    int getselectedSubp(k9DVDTitle *_title);
    k9play *m_player;
    QEventLoop *m_eventLoop;
    k9MemoryFifo m_fifo;
    QWaitCondition m_dataRead;
    QFile *m_outputFile;
    QMap <int,k9ConvertAudio*> m_converters;
    QList <k9ConvertAudio*> m_convertersToDelete;
    QMap <int,k9Subtitle2Pgm*> m_subtitles;
    QMultiMap<int, int>::iterator ichapterCells;
    QMultiMap <int,k9ResultingFile*> m_resultingFiles;
private slots:

    void timerDone();
    void slotCancel();
public slots:
    virtual void getOutput(uchar *buffer,uint32_t size) ;
    virtual void getOutput(eStreamType streamType,int streamNumber,uchar *buffer,uint32_t size);

public:
    k9MP4Enc(QObject *parent = 0);
    ~k9MP4Enc();

    virtual void execute(k9DVDTitle *_title );

    virtual void setNumberCD(const QString & _value) {
        m_parts = _value.toInt();
        if (m_parts==0) m_parts=1;
    }

    virtual void setDevice(const QString& _value) {
        m_device = _value.trimmed();
    }
    virtual void setFilename(const QString& _value) {
        m_filename = _value.trimmed();
    }
    virtual void setHeight(const QString& _value) {
        m_height = _value.trimmed();
    }
    virtual void setWidth(const QString& _value) {
        m_width = _value.trimmed();
    }
    virtual void setSize(const QString& _value) {
        m_size = _value.trimmed();
    }
    virtual void setAudioBitrate(const QString& _value) {
        m_audioBitrate = _value.trimmed();
    }
    virtual void setVideoBitrate(const QString& _value) {
        m_videoBitrate = _value.trimmed();
    }
    virtual void setAudioGain(const QString& _value) {
        m_audioGain = _value.trimmed();
    }
    virtual void setfourcc(const QString& _value) {
        m_fourcc = _value.trimmed();
    }

    virtual void set2Passes(bool _value) {
        m_2pass=_value;
    }

    virtual void setUseCache(bool _value) {
        m_usecache=_value;
    }

    virtual void setMpeg2(bool _value) {
        m_mpeg2=_value;
    }

    virtual void setExtractAudio(bool _value) {
        m_extractAudio=_value;
    }

    virtual void setExtractSubtitle(bool _value) {
        m_extractSubtitle=_value;
    }

    virtual void setExtractMkv(bool _value) {
        m_extractMkv=_value;
    }

    virtual void setCodec(const int& _value) {
        m_codec = _value;
    }

    virtual void setAudioCodec(const int& _value) {
        m_audioCodec = _value;
    }

    virtual QWidget *getDialog() {
        return (QWidget*)m_progress;
    }

    virtual bool isCanceled();

    QMultiMap< int, k9ResultingFile* > getResultingFiles() const;

    void setSplitChapters(bool theValue);

    void setCrop ( const QRect& theValue )
    {
            m_crop = theValue;
    }

    const QString &getAspect() {
        return m_aspect;
    }

};

#endif
