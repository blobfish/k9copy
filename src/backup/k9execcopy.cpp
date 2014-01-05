//
// C++ Implementation: k9execcopy
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9execcopy.h"
#include "k9dialogs.h"
#include "k9dvdsize.h"
#include "k9dvdbackup.h"
#include "k9dvdauthor.h"
#include "k9tools.h"
#include <QDir>
#include "k9dvdtitle.h"

#include "k9burndvd.h"
#include "k9play.h"
#ifdef BACKLITE
#include <QMessageBox>
#include <stdlib.h>
#else
#include <KMessageBox>
#endif
#include "k9mp4enc.h"
#include <QFileInfo>
#include "k9titleencopt.h"
#include "k9audiocodecs.h"
#include "k9videocodecs.h"

k9ExecCopy::k9ExecCopy():QObject(0) {
    m_speed= i18n("default");
    m_dvdSize=0;
}

void k9ExecCopy::copyDVD() {
    k9Config config;
    m_cancelled=false;
    QString filename;
    if (!m_dvd->getopened()) {
        k9Dialogs::error(  i18n("DVD is not opened"), i18n("DVD Copy"));
        return;
    }

    k9DVDSize *dvdsize=new k9DVDSize();
    dvdsize->setMaxSizeDyn(m_dvdSize==0 ?config.getPrefSize():m_dvdSize);
    delete dvdsize;

    QString outputDir=config.getPrefOutput() ;
    if (m_output !=oFolder)
        k9Tools::addTemporaryDir(outputDir+"/"+m_dvd->getDVDTitle());
    bool burn=false;
    int cancel;
#ifdef BACKLITE
        cancel=QMessageBox::Cancel;
#else
        cancel=KMessageBox::Cancel;
#endif

    if (m_output==oISO) {
        filename=m_path;
    } else {
        if (m_output==oFolder)
            outputDir= m_path;
        QDir dir(m_path);
        if (!dir.exists()) {
            dir.mkpath(m_path);
        }
    }

    long fs=k9Tools::getFreeSpace(outputDir);
    int maxSize=config.getPrefSize() < (m_dvd->getsizeSelected(false)/512) ? config.getPrefSize():(m_dvd->getsizeSelected(false)/512);
     if ((fs < maxSize) && (fs !=-1)) {
        if (k9Dialogs::warningContinueCancel ( i18n("Insufficient disk space on %1\n%2 MB expected.").arg(outputDir).arg(config.getPrefSize()),i18n("DVD Copy"))==cancel)
            return;
    }

// eliminates overwritten DVD copies (bkp)
    QDir dir(outputDir+"/"+m_dvd->getDVDTitle());
    if (dir.exists() && !k9Tools::getBatchCopy()) {
        if (k9Dialogs::warningContinueCancel( i18n("Directory already exists.\nPlease choose another location or rename the DVD title."), i18n("Warning!"))==cancel)
	  return;
    }

//    stopPreview();
    if (!config.getUseDvdAuthor() || m_copyMenus) {

        k9DVDBackup *backup=new k9DVDBackup(m_dvd);

        backup->setOutput(outputDir);
        backup->setDevice(m_dvd->getDevice());
        backup->setWithMenus( m_copyMenus);
	backup->setUserOps(config.getPrefUserOps());
        backup->execute();
        burn=backup->getErrMsg()=="";
        delete backup;
    } else {
        k9DVDAuthor *b=new k9DVDAuthor(m_dvd);
        b->setworkDir(outputDir);
        b->author();
        if (!b->getError())
            burn=true;

        delete b;
    }
    m_cancelled=!burn;
    if (burn && m_output!=oFolder) {
        k9BurnDVD b;

        b.setworkDir(outputDir +"/"+ m_dvd->getDVDTitle());
        b.setUseK3b(config.getPrefK3b());
        b.setAutoBurn(config.getPrefAutoBurn());
        b.setvolId(m_dvd->getDVDTitle());

        b.setSpeed(m_speed);
        if (m_output==oDVD) {
            b.setburnDevice(m_path);
            m_dvd->close();
            if (m_dvd->getDevice()==m_path) {
                b.setAutoBurn(false);
		b.setEject(true);
	    }
        } else
            b.makeIso(m_path);
        b.burn();
       m_cancelled=b.isCancelled();
    } else if (burn && m_output==oFolder)
        k9Dialogs::information( i18n("Selected titles have been successfully extracted"), i18n("DVD Copy") );

   config.setKeepMenus(m_copyMenus);
   config.save();
}

void k9ExecCopy::CreateMP4() {
    k9Config config;

    if (!m_dvd->getopened()) {
        k9Dialogs::error(  i18n("DVD is not opened"), i18n("MPEG-4 Encoding"));
        return;
    }
    QString filename=m_path;
    int cpt=0;
    bool bStop=false;

    QDir dir(config.getPrefOutput());
    if (!dir.exists()) {
        dir.mkpath(config.getPrefOutput());
    }

    for (int i=0; i < m_dvd->gettitleCount();i++) {
        k9DVDTitle *t=m_dvd->gettitle(i);

        if (t->isSelected() && t->getIndexed() ) {
            QString name;
            if (filename=="")
                filename=k9Dialogs::getSaveFileName (QDir::homePath(),"*.avi|Audio Video Interleave (*.avi)\n*.mpg|MPEG-1 and MPEG-2 systems (*.mpg)\n*.mp4|MPEG-4 format (*.mp4)\n*.asf|Advanced Streaming Format (*.asf)\n*.swf|Adobe Flash (*.swf)\n*.flv|Adobe Flash video files (*.flv)\n*.rm|RealVideo (*.rm)\n*.mov|QuickTime (*.mov)\n*.mkv|Matroska (*.mkv)", 0,i18n("Save file to disk"));
            if (filename=="") {
                bStop=true;
                break;
            }
  	    QString path=filename;
            bool matroska=filename.endsWith(".mkv");
            k9MP4Enc *mp4=new k9MP4Enc();
            if (cpt >0) {
                QString ext=filename.section(".",-1);
                if (ext!="")
                    ext="."+ext;
                path=filename.left(filename.length()-ext.length());
                path=path+QString::number(cpt)+ext;
                mp4->setFilename(path);
            } else
                mp4->setFilename(filename);

            cpt++;
            k9TitleEncOpt * opt=t->getEncOpt();
            mp4->setDevice(m_dvd->getDevice());
            mp4->setAudioBitrate(opt->getAudioBr());
            mp4->setVideoBitrate(opt->getVideoBr());
	    mp4->setCrop(*opt->getCrop());
            mp4->setCodec(k9VideoCodecs::getAbsCodecNum(opt->getCodec(),opt->getEncoder()));
            mp4->setAudioCodec(k9AudioCodecs::getAbsCodecNum(opt->getAudioCodec(), opt->getEncoder()));
            mp4->setSize(QString::number(opt->getMaxSize()));
            mp4->setAudioGain(QString::number(opt->getAudioGain()));
            mp4->setNumberCD(QString::number(opt->getNumParts()));
            mp4->setWidth(opt->getWidth() );
            mp4->setHeight(opt->getHeight());
            mp4->set2Passes( opt->get2Passes());
            mp4->setUseCache(opt->getUseCache());

            if (matroska) {
                if ( ! k9Tools::checkProgram("mkvmerge") ) {
                    k9Dialogs::error (i18n("Unable to run %1").arg("mkvmerge") , i18n("Encoding error"));
		    delete mp4;
                    return ;
                }
                mp4->setExtractAudio(false);
                mp4->setExtractMkv(true);
                mp4->setExtractSubtitle(true);
                mp4->setSplitChapters(false);
                mp4->setFilename(config.getPrefOutput()+"/"+k9Tools::randomFileName());
            }
            mp4->execute(t);
            bStop=mp4->isCanceled();
            if (matroska && !bStop) {
                createMkv(t,path,mp4->getResultingFiles(),opt->getNumParts(),mp4->getAspect());
            }
            delete mp4;
            if (bStop)
                break;
        }
    }
    if (!bStop)
        k9Dialogs::information( i18n("Selected titles have been successfully encoded"), i18n("Encoding") );

}

void k9ExecCopy::MkvStdout(char *buffer, int size) {
    QByteArray ba(buffer,size);
    m_outputMsg << ba;
    if (m_outputMsg.count() > 50)
	m_outputMsg.removeFirst();
    QStringList sl=QString(ba).split("\r");
    foreach(QString s,sl) {
        if (s.contains("gress:")) {
 	     int p =s.indexOf("%");
 	     if (p!=-1) {
            	QString pos=s.mid(p-3,3).trimmed();
		m_mkvPos=pos.toInt();
                m_progressDialog->progressBar()->setValue(m_mkvPos);
	     }
        }
    }
}



void k9ExecCopy::createMkv(k9DVDTitle *_title,const QString &_filename,QMultiMap< int, k9ResultingFile* > _files,int _nbParts, const QString &_aspect) {
    k9Process process;
    process << "mkvmerge" << "-o" << _filename;
    process.setDebugOutput(false);
    int streamId=1;
    k9Config config;

    QFile mkvChapter(config.getPrefOutput()+"/"+k9Tools::randomFileName());
    mkvChapter.open(QIODevice::ReadWrite);

    QList <k9DVDChapter*> chapters=_title->getChapters();
    short int num=1;
    QTime startTime(0,0,0);
    foreach (k9DVDChapter *chapter ,chapters) {
        if (chapter->getSelected()) {
            QString start=QString("CHAPTER%1=%2\n").arg(num,2,10,QChar('0')).arg(startTime.toString("hh:mm:ss.zzz"));
            startTime=startTime.addMSecs(QTime().msecsTo(chapter->getLength()));
            mkvChapter.write(start.toUtf8());
            QString name=QString("CHAPTER%1NAME=Chapter %2\n").arg(num,2,10,QChar('0')).arg(num);
            mkvChapter.write(name.toUtf8());
            num++;
        }
    }
    QList <k9DVDTitle*> titles=_title->getTitles();

    foreach (k9DVDTitle *title,titles) {
        chapters=title->getChapters();
        foreach (k9DVDChapter *chapter,chapters) {
            if (chapter->getSelected()) {
                QString start=QString("CHAPTER%1=%2\n").arg(num,2,10,QChar('0')).arg(startTime.toString("hh:mm:ss.zzz"));
                startTime=startTime.addMSecs(QTime().msecsTo(chapter->getLength()));
                mkvChapter.write(start.toUtf8());
                QString name=QString("CHAPTER%1NAME=Chapter %2\n").arg(num,2,10,QChar('0')).arg(num);
                mkvChapter.write(name.toUtf8());
                num++;
            }
        }
    }
    mkvChapter.close();
    for (int i=0;i<_title->getaudioStreamCount();i++) {
        if (_title->getaudioStream(i)->getselected()) {
            QString lng(_title->getaudioStream(i)->getlangCod());
            process <<"--language" << QString("%1:%2").arg(streamId++).arg(lng.replace("xx","und"));
        }
    }

    process << "--chapters" << mkvChapter.fileName();
    QString aspect=_aspect;

    process << "--aspect-ratio" << QString("0:%1").arg(aspect.replace(":","/"));
    qint64 totalSize=0;

    connect(&process,SIGNAL(receivedStdout(char*, int)),this,SLOT(MkvStdout(char*, int)));
    connect(&process, SIGNAL(finished ( int , QProcess::ExitStatus )),this,SLOT(finished(int, QProcess::ExitStatus)));
    foreach (k9ResultingFile *rf,_files) {
        process << rf->filename;
	QString fname= rf->filename;
	if (fname.endsWith(".idx"))
	    fname=fname.replace(".idx",".sub");
	QFileInfo inf(fname);
	totalSize+=inf.size();
    }
    totalSize/=(1024*1024);
    if (_nbParts >1)
    	process << "--split"<< QString("size:%1m").arg((int)(totalSize/_nbParts));
    qDebug() << process.debug();

    m_progressDialog=new KProgressDialog(k9Dialogs::getMainWidget() );
    m_progressDialog->showCancelButton(false);
    m_progressDialog->progressBar()->setMaximum(100);
    m_progressDialog->setAutoClose(true);
    m_progressDialog->setLabelText(i18n("Muxing Matroska file"));
    process.start();
    //process.sync();
    m_progressDialog->exec();
    process.wait();

    delete m_progressDialog;
    foreach (k9ResultingFile *rf,_files) {
        QFile f(rf->filename);
        f.remove();
    }
    mkvChapter.remove();
    if ( m_mkvPos !=100)
	k9Dialogs::error(i18n("An error occured while running mkvmerge"),i18n("Muxing Matroska"),m_outputMsg);
    m_outputMsg.clear();
}

void k9ExecCopy::extractMPEG2() {
    k9Config config;

    if (!m_dvd->getopened()) {
        k9Dialogs::error(  i18n("DVD is not opened"), i18n("MPEG-4 Encoding"));
        return;
    }
    QString filename=m_path;
    int cpt=0;
    bool bStop=false;

    QDir dir(config.getPrefOutput());
    if (!dir.exists()) {
        dir.mkpath(config.getPrefOutput());
    }

    for (int i=0; i < m_dvd->gettitleCount();i++) {
        k9DVDTitle *t=m_dvd->gettitle(i);

        if (t->isSelected() && t->getIndexed() ) {
            QString name;
            if (filename=="")
                filename=k9Dialogs::getSaveFileName (QDir::homePath(),"*.mpg|MPEG-1 and MPEG-2 systems (*.mpg)", 0,i18n("Save file to disk"));
            if (filename=="") {
                bStop=true;
                break;
            }

            k9MP4Enc *mp4=new k9MP4Enc();
            mp4->setMpeg2(true);
            if (cpt >0) {
                QString ext=filename.section(".",-1);
                if (ext!="")
                    ext="."+ext;
                QString path=filename.left(filename.length()-ext.length());
                path=path+QString::number(cpt)+ext;
                mp4->setFilename(path);
            } else
                mp4->setFilename(filename);

            cpt++;
            mp4->setDevice(m_dvd->getDevice());
            mp4->setUseCache(false);
            mp4->execute(t);
            bStop=mp4->isCanceled();
            delete mp4;
            if (bStop)
                break;
        }
    }
    if (!bStop)
        k9Dialogs::information( i18n("Selected titles have been successfully extracted"), i18n("Encoding") );


}

void k9ExecCopy::extractAudio() {
    k9Config config;

    if (!m_dvd->getopened()) {
        k9Dialogs::error(  i18n("DVD is not opened"), i18n("Audio Extraction"));
        return;
    }
    QString filename=m_path;
    int cpt=0;
    bool bStop=false;

    QDir dir(config.getPrefOutput());
    if (!dir.exists()) {
        dir.mkpath(config.getPrefOutput());
    }

    for (int i=0; i < m_dvd->gettitleCount();i++) {
        k9DVDTitle *t=m_dvd->gettitle(i);

        if (t->isSelected() && t->getIndexed() ) {
            QString name;
            if (filename=="")
                filename=k9Dialogs::getSaveFileName (QDir::homePath(),"*.mpg|MPEG-1 and MPEG-2 systems (*.mpg)", 0,i18n("Save file to disk"));
            if (filename=="") {
                bStop=true;
                break;
            }
            k9TitleEncOpt * opt=t->getEncOpt();

            k9MP4Enc *mp4=new k9MP4Enc();
            mp4->setExtractAudio(true);
            mp4->setSplitChapters(true);
            mp4->setAudioCodec(k9AudioCodecs::getAbsCodecNum( opt->getAudioCodec(),opt->getEncoder()) );

            if (cpt >0) {
                QString ext=filename.section(".",-1);
                if (ext!="")
                    ext="."+ext;
                QString path=filename.left(filename.length()-ext.length());
                path=path+QString::number(cpt)+ext;
                mp4->setFilename(path);
            } else
                mp4->setFilename(filename);

            cpt++;
            mp4->setDevice(m_dvd->getDevice());
            mp4->setUseCache(false);
            mp4->execute(t);
            bStop=mp4->isCanceled();
            delete mp4;
            if (bStop)
                break;
        }
    }
    if (!bStop)
        k9Dialogs::information( i18n("Selected titles have been successfully extracted"), i18n("Encoding") );


}

void k9ExecCopy::finished(int, QProcess::ExitStatus _status) {
    Q_UNUSED(_status);
    m_progressDialog->progressBar()->setValue(100);
}


void k9ExecCopy::setSpeed(const QString& theValue) {
    m_speed = theValue;
}
