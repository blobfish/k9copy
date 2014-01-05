//
// C++ Interface: k9play
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9PLAY_H
#define K9PLAY_H

#include "k9common.h"
#include "k9dvdtitle.h"
#include "k9saveimage.h"
#include <QThread>
#include <QEventLoop>
#include <qfile.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

typedef struct {
    uint title;
    uint chapter;
    uint cell;
    uint32_t sector;
    uint64_t bytesWritten;
    uint64_t bytesRead;
    uint64_t bytesSkipped;
    uint64_t bytesChapters;
} k9play_st;

class k9vamps;

class k9play:public QThread {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "k9copy.free.fr.player")
private:
    k9vamps *m_vamps;
    int m_title;
    k9DVD *m_dvd;
    k9DVDTitle *m_dvdTitle;
    QString m_device;
    QFile *m_output;
    QFile m_stderr;
    uint32_t m_totalBytes;
    uint32_t m_startSector,m_endSector;
    uint32_t m_pos,m_length;
    double m_vampsFactor;
    uint64_t m_inputSize;
    uint64_t m_totalSize;
    uint64_t m_chapterSize;
    uint64_t m_dvdSize;
    uint m_chapter;
    uint m_cell;
    bool m_initstatus;
    bool m_continue;
    bool m_firstPass;
    bool m_forcedFactor;
    bool m_useCache;
    QObject *m_parent;
    k9SaveImage *m_saveImage;
    QString m_inject;
    QStringList m_audioFilter;
    QStringList m_subpictureFilter;
    QStringList m_chapterList;
    void kdebug(QString const & _msg);
    bool readNavPack (k9DVDFile *fh, dsi_t *dsi,int sector, uchar *_buffer);
    void insert_dummy_pack (int8_t *buf);
    void insert_nav_pack (int8_t *buf);
    void saveStatus(k9play_st _status);
    void readStatus(k9play_st &_status);
    bool m_aborted;
    bool m_exitOnEnd;
    bool m_terminated;
    bool m_mainThread;
    QEventLoop *m_eventLoop;
protected:
    virtual void run(void);
    void end();
public:
    k9play(QObject *parent);

    ~k9play();
    void play();
    void writeOutput(QString const & _msg);


    void setTitle(int _value) {
        m_title = _value;
    };

    void setDevice(const QString& _value) {
        m_device = _value;
    };

    void setstartSector(QString _value);
    void setendSector(QString _value);
    void setaudioFilter(QString _value);
    void setsubpictureFilter(QString _value);
    void setchapterList(QString _value);
    void setvampsFactor(QString _value);
    void setinputSize(QString _value);
    void settotalSize(QString _value);
    void setchapter(QString _value);
    void setcell(QString _value);
    void setinject(QString _value);
    void setdvdSize(QString _value);
    void setchapterSize(QString _value);
    void setinitStatus(bool _value);
    void setcontinue (bool _value);
    void setfirstPass (bool _value);
    void setforcedFactor(bool _value);
    void setuseCache(bool _value);
    void setexitOnEnd(bool _value);
    k9SaveImage *getSaveImage();
    void getPosition(uint32_t *_position,uint32_t *_length);
public slots:
    void execute();
    void abort();

    void setEventLoop(QEventLoop* theValue);

    bool terminated() const;

	void setMainThread(bool theValue);

	void setAborted(bool theValue);
	
	


signals:
    void sigError(const QString & message);
    void sigPosition(uint position,uint len);
    void sigImageReady(const QString &path);
};

#endif
