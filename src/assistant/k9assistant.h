//
// C++ Interface: k9assistant
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifdef BACKLITE
#include "../gui/k9assistant.h"
#else
#ifndef K9ASSISTANT_H
#define K9ASSISTANT_H

#include "k9common.h"

#include <KAssistantDialog>
#include <KPageWidget>

/**
	@author
*/

enum eDestination {dstDVD,dstAvi,dstMpeg,dstIso,dstFolder,dstAudio};

class k9DVD;
class k9CdDrives;

class k9Assistant : public KAssistantDialog {
    Q_OBJECT
public:


    k9Assistant(KPageWidget *_pageWidget);

    ~k9Assistant();
    static k9Assistant *createAssistant();


    void setDrives ( k9CdDrives* theValue ) {
        m_drives = theValue;
    }

    void setDvd(k9DVD* theValue) {
        m_dvd = theValue;
    }
    k9DVD* getDvd() const {
        return m_dvd;
    }
    void setDestination(const eDestination& theValue) {
        m_destination = theValue;
    }
    eDestination destination() const {
        return m_destination;
    }


public slots:
    void next();
    void back();
    void accept();
    void reject();
    void run();
    void helpClicked();
    void setCopyMenus(bool theValue) {
        m_copyMenus = theValue;
    }


    bool copyMenus() const {
        return m_copyMenus;
    }

    void setPath(const QString& theValue) {
        m_path = theValue;
    }


    QString path() const {
        return m_path;
    }

    void setDVDdestination(const eDestination& theValue) {
        m_DVDdestination = theValue;
    }


    eDestination DVDdestination() const {
        return m_DVDdestination;
    }




private:
    KPageWidget *m_pageWidget;
    KPageWidgetItem *m_sourceItem;
    KPageWidgetItem *m_destinationItem;
    KPageWidgetItem *m_titlesItem;
    KPageWidgetItem *m_streamsItem;
    KPageWidgetItem *m_mp4Item;
    KPageWidgetItem *m_mpegItem;
    KPageWidgetItem *m_DVDOptionItem;

    k9DVD *m_dvd;
    k9CdDrives *m_drives;

    eDestination m_destination;
    eDestination m_DVDdestination;
    bool m_copyMenus;
    QString m_path;
};

#endif
#endif
