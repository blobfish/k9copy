/**************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   k9copy@free.fr                                                            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#ifndef K9DVDTITLE_H
#define K9DVDTITLE_H

#include "k9common.h"
#include "k9dvd.h"
#include "k9dvdchapter.h"
#include <QDateTime>
#include <QStringList>

class k9DVDTitleSet;

class k9DVDVideoStream {
    friend class k9DVDTitle;
private: // Private attributes
public: // Public methods

};

class k9DVDAudioStream {
    friend class k9DVDTitle;
    friend class k9DVD;
private: // Private attributes
    /**  */
    QString langCod;
    /**  */
    QString language;
    /**  */
    QString format;
    /**  */
    QString frequency;
    /**  */
    QString quantization;
    /**  */
    int channels;
    /**  */
    int appMode;
    /**  */
    QString content;
    /**  */
    float size_mb;
    /**  */
    bool selected;
    int id;
    k9DVDTitle *title;
    int m_streamId;
public: // Public methods
    k9DVDAudioStream();
    /** Read property of QString langCod. */
    virtual const QString& getlangCod();
    /** Read property of QString language. */
    virtual const QString& getlanguage();
    /** Read property of QString format. */
    virtual const QString& getformat();
    /** Read property of QString frequency. */
    virtual const QString& getfrequency();
    /** Read property of QString quantization. */
    virtual const QString& getquantization();
    /** Read property of int channels. */
    virtual const int& getchannels();
    /** Read property of int appMode. */
    virtual const int& getappMode();
    /** Read property of QString content. */
    virtual const QString& getcontent();
    /** Read property of float size_mb. */
    virtual const float& getsize_mb();
    /** Write property of bool selected. */
    virtual void setselected( const bool& _newVal);
    /** Read property of bool selected. */
    virtual const bool& getselected();

    virtual k9DVDTitle* gettitle();
    virtual  int getID();

    virtual int getStreamId() const {
        return m_streamId;
    };
    virtual ~k9DVDAudioStream() {};

};

class k9DVDSubtitle {
    friend class k9DVDTitle;
    friend class k9DVD;
private: // Private attributes
    /**  */
    QString langCod;
    /**  */
    QString language;
    /**  */
    int num;
    /**  */
    QString content;
    /**  */
    float size_mb;
    /**  */
    bool selected;
    QList<int> id;
    k9DVDTitle *title;
public: // Public methods
    k9DVDSubtitle();
    /** Read property of QString langCod. */
    virtual const QString& getlangCod();
    /** Read property of QString language. */
    virtual const QString& getlanguage();
    /** Read property of int num. */
    virtual const int& getnum();
    /** Read property of QString content. */
    virtual const QString& getcontent();
    /** Read property of float size_mb. */
    virtual const float& getsize_mb();
    /** Write property of bool selected. */
    virtual void setselected( const bool& _newVal);
    /** Read property of bool selected. */
    virtual const bool& getselected();
    virtual k9DVDTitle* gettitle();
    virtual QList<int>& getID();
    virtual ~k9DVDSubtitle() {};
};


class k9TitleEncOpt;

class k9DVDTitle : public QObject {
    Q_OBJECT
    friend class k9DVD;
private: // Public attributes
    unsigned int ts_nr;
    int chapterCount;
    int streamCount;
    int audioStreamCount;
    int subPictureCount;
    QTime length;
    k9DVDVideoStream videoStream;
    QList< k9DVDAudioStream*> audioStreams;
    QList <k9DVDSubtitle*> subtitles;
    int VTS;
    int TTN;
    float FPS;
    QString format;
    QString aspectRatio;
    int numTitle;
    QString width;
    QString height;
    QStringList palette;
    QString DF;
    float size_mb;
    int angleCount;
    float videosize_mb;
    float vobusize_mb;
    QString name;
    k9DVDTitle* nextTitle;
    int id;
    k9DVDTitleset *m_titleset;
    k9DVDAudioStream *m_defAudio;
    k9DVDSubtitle *m_defSubtitle;
    bool m_defAudioSet;
    bool m_defSubtitleSet;
    QList <k9DVDTitle*> m_titles;
    uint32_t m_sectors;
    bool m_forceFactor;
    float m_factor;

    k9TitleEncOpt *m_encOpt;
    k9DVDTitle *m_mainTitle;
public: // Public methods
    k9DVDTitle(k9DVD *dvd);
    ~k9DVDTitle();
    virtual k9DVDTitleset *gettitleset() {
        return m_titleset;
    };
    virtual const int& getchapterCount();
    virtual const int& getstreamCount();
    virtual const int& getaudioStreamCount();
    virtual const int& getsubPictureCount();
    virtual const QTime& getlength();
    virtual QTime gettotallength();
    virtual QTime getSelectedLength();

    virtual const int& getVTS();
    virtual const int& getTTN();
    virtual const float& getFPS();
    virtual const QString& getformat();
    virtual const QString& getaspectRatio();
    virtual const QString& getwidth();
    virtual const QString& getheight();
    virtual void getpalette(int i,QString & c);
    virtual const QString& getDF();
    virtual const int& getangleCount();
    virtual const int& getnumTitle();
    virtual k9DVDAudioStream *getaudioStream(int num);
    virtual k9DVDSubtitle *getsubtitle(int num);
    virtual const float& getsize_mb();
    virtual float  gettotalsize_mb();
    virtual float  gettotalvideosize_mb();
    virtual float  getChaptersSize_mb(bool _selected);
    virtual uint64_t  getChaptersSize(bool _selected);
    virtual const float& getvideosize_mb();
    virtual bool isSelected();
    virtual void setname( const QString& _newVal);
    virtual const QString& getname();
    virtual int getpgc();
    virtual void setnextTitle(k9DVDTitle * nTitle);
    virtual k9DVDTitle* getnextTitle();
    virtual k9DVDChapter *getChapter(int num);
    virtual bool getIndexed() {
        return indexed;
    };
    virtual void setvideoSelected(bool _state,bool _updateChapters=true);
    virtual void setSelected(bool _state);
    virtual int getId() {
        return id;
    };
    virtual void setDefAudio(k9DVDAudioStream* _value);
    virtual k9DVDAudioStream* getDefAudio() const;
    virtual void setDefSubtitle(k9DVDSubtitle* _value);
    virtual k9DVDSubtitle* getDefSubtitle() const;
    virtual k9DVDChapter *getChapterFromSector(uint32_t _sector);
    virtual bool getDefAudioSet() const {
        return m_defAudioSet;
    };
    virtual bool getDefSubtitleSet() const {
        return m_defSubtitleSet;
    };
    virtual QList< k9DVDTitle* > getTitles() const {
        return m_titles;
    };
    virtual uint32_t getsectors() {
        return m_sectors;
    };
    virtual float getfactor() {
        return m_factor;
    };
    virtual bool getforceFactor() {
        return m_forceFactor;
    };
    virtual void selectChapters(bool _state);
    virtual QList< k9DVDChapter *> getChapters() const {
        return chapters;
    };
    virtual k9TitleEncOpt* getEncOpt() ;
public slots:
    virtual void setfactor(float _value) {
        m_factor=_value;
    };
    virtual void setforceFactor(bool _value) {
        m_forceFactor=_value;
    };

    k9DVDTitle* getMainTitle() const;

	k9DVD* getDvd() const;
	


signals:
    void selectionChanged();
private: // Private methods
    k9DVDAudioStream* addAudioStream();
    k9DVDChapter* addChapter(int num);
    k9DVDSubtitle* addSubtitle(int num);
    void clearAudioStreams();
    void clearSubtitles();
    void clearChapters();
    QList <k9DVDChapter*> chapters;
    bool indexed;
    bool forceSelection;
    int pgc;
    k9DVD *m_dvd;

public: // Public attributes
};




#endif

