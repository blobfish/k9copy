//
// C++ Interface: k9execcopy
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9EXECCOPY
#define K9EXECCOPY
#include "k9dvd.h"
#include <QObject>
#ifdef BACKLITE
#include <QProgressDialog>
class KProgressDialog:public QProgressDialog {
public:
    KProgressDialog (QWidget *parent):QProgressDialog(parent) {}
    QProgressDialog * progressBar() { return this;}
    void showCancelButton(bool _visible) {
        if (_visible)
           setCancelButtonText(i18n("Cancel"));
        else
           setCancelButtonText("");
    }
};
#else
#include <KProgressDialog>
#endif
class k9ResultingFile;
class k9ExecCopy:public QObject  {
Q_OBJECT
public:
    enum eOutput {oDVD,oISO,oFolder};

    k9ExecCopy();
    void CreateMP4();
    void extractMPEG2();
    void extractAudio();
    void setDvd(k9DVD* theValue) {
        m_dvd = theValue;
    }

    void setOutput(const eOutput& theValue) {
        m_output = theValue;
    }

    void setCopyMenus(bool theValue) {
        m_copyMenus = theValue;
    }

    void setPath(const QString& theValue) {
        m_path = theValue;
    }

    void setSpeed(const QString& theValue);
	
    bool isCancelled() {
        return m_cancelled;
    }
    
    void setDvdSize( int theValue) {
      m_dvdSize=theValue;
    }
private:
    k9DVD *m_dvd;
    bool m_cancelled;
    QString m_speed;
    QString m_path;
    eOutput m_output;
    bool m_copyMenus;
    KProgressDialog *m_progressDialog;
    void createMkv(k9DVDTitle *_title,const QString &_filename,QMultiMap< int, k9ResultingFile* > _files,int _nbParts, const QString &_aspect) ;
    QStringList m_outputMsg;
    int m_mkvPos;
    int m_dvdSize;
protected slots:
    void MkvStdout(char *, int );
    void finished(int, QProcess::ExitStatus);
public slots:
    void copyDVD();
};



#endif
