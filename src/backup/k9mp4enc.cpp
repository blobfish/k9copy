//
// C++ Implementation: k9mp4enc
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9mp4enc.h"
#include "k9mp4dlg.h"
#include <qapplication.h>
#include <qstringlist.h>
#include <qdir.h>

#include <qstringlist.h>
#include "k9tools.h"
#include "k9audiocodecs.h"
#include "k9videocodecs.h"
#include "k9convertaudio.h"
#include <QByteArray>
#include "k9dialogs.h"
#include <QMutex>
#include <math.h>
#include "k9ifo2.h"
#include "k9cell.h"
#include "k9vamps.h"
#include "k9log.h"

#ifndef BACKLITE
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <KShell>

#endif

#define BASE_CONV_AUDIO 0
#define BASE_CONV_VIDEO 10



k9MP4Enc::k9MP4Enc(QObject *parent)
        : k9DVDBackupInterface(parent) {
    m_fourcc=m_height=m_width=m_audioBitrate=m_videoBitrate=m_filename="";
    m_codec=0; //lavc_mp4;
    m_audioCodec=0;
    m_cpt=-1;
    m_parts=1;
    m_mpeg2=false;
    m_extractAudio=false;
    m_extractMkv=false;
    m_extractSubtitle=false;

    QStringList laudio;
    QStringList llabels;
    QStringList lvideo;

    k9Config config;
    m_splitChapters=config.getMpegChapters();
    m_ffmpegPath=config.getPrefFFmpegPath(true);
    m_mencoderPath=config.getPrefMencoderPath(true);
    timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), this, SLOT(timerDone()) );
    m_progress=new k9MP4Dlg(k9Dialogs::getMainWidget());
    m_progress->setModal(true);

}

QString k9MP4Enc::round16(QString _wh) {
    if (_wh !="") {
        int value=_wh.toInt()/16;
        return QString::number(value*16);

    } else
        return _wh;


}

QString k9MP4Enc::getChapterList(k9DVDTitle *_title) {
    QString res="";
    QList <k9DVDChapter*> chapters=_title->getChapters();
    foreach (k9DVDChapter *chapter ,chapters) {
        QTime t;
        long l=t.msecsTo(chapter->getLength());
        if (chapter->getSelected() && l>1 ) {
            res+=res=="" ? QString::number( chapter->getnum()) : ","+QString::number( chapter->getnum());
        }
    }
    QList <k9DVDTitle*> titles=_title->getTitles();

    foreach (k9DVDTitle *title,titles) {
        chapters=title->getChapters();
        foreach (k9DVDChapter *chapter,chapters) {
            if (chapter->getSelected()) {
                res+=res=="" ? QString::number( chapter->getnum()) : ","+QString::number( chapter->getnum());
            }
        }
    }
    return res;

}


int k9MP4Enc::getselectedSubp(k9DVDTitle *_title) {
    for (int i=0;i< _title->getsubPictureCount();i++) {
        if (_title->getsubtitle( i)->getselected()) {
            return _title->getsubtitle(i)->getID().first()-1;
        }
    }
    //nos subtitle selected
    return -1;
}

bool k9MP4Enc::check() {
    if (m_mpeg2)
        return true;

    QString sCodecA,sCodecV;

    k9AudioCodecs *audioCodecs=new k9AudioCodecs(0);
    sCodecA=audioCodecs->getEncoder(m_audioCodec);

    if (m_extractAudio && sCodecA !="ffmpeg") {
        k9log::add(i18n("Audio extraction can only be done with ffmpeg"),k9logLevel::ERROR);
        k9Dialogs::error(i18n("Audio extraction can only be done with ffmpeg"),i18n("Encoding error"));
        return false;
    }

    k9VideoCodecs *videoCodecs=new k9VideoCodecs(0);
    sCodecV=videoCodecs->getEncoder (m_codec);

    if (m_extractMkv && sCodecV!="ffmpeg") {
        k9log::add(i18n("Matroska encoding can only be done with ffmpeg"),k9logLevel::ERROR);
        k9Dialogs::error(i18n("Matroska encoding can only be done with ffmpeg"),i18n("Encoding error"));
        return false;
    }

    if (!m_extractAudio) {
        if ( ! k9Tools::checkProgram(sCodecV) ) {
            k9log::add(i18n("Unable to run %1").arg(sCodecV),k9logLevel::ERROR);
            k9Dialogs::error (i18n("Unable to run %1").arg(sCodecV) , i18n("Encoding error"));
            m_error = TRUE;
            return false;
        }
    }



    if ((sCodecA!=sCodecV) || m_extractAudio) {
        if ( ! k9Tools::checkProgram(sCodecA) ) {
            k9log::add(i18n("Unable to run %1").arg(sCodecV),k9logLevel::ERROR);
            k9Dialogs::error (i18n("Unable to run %1").arg(sCodecA) , i18n("Encoding error"));
            m_error = TRUE;
            return false;
        }
    }


    return true;
}

void k9MP4Enc::execute(k9DVDTitle *_title) {
    if (m_mpeg2)
        k9log::add(i18n("Starting extraction of %1, chapters %2").arg(_title->getname()).arg(getChapterList( _title)),k9logLevel::INFO);
    else
        k9log::add(i18n("Starting encoding of %1, chapters %2").arg(_title->getname()).arg(getChapterList( _title)),k9logLevel::INFO);
    k9log::add(i18n("source : %1").arg(m_device),k9logLevel::INFO);
    k9log::add(i18n("destination : %1").arg(m_filename),k9logLevel::INFO);
    k9log::add(i18n("disk cache : %1").arg( m_usecache ? i18n("activated"):i18n("disabled")),k9logLevel::INFO);
    m_currentChapter=0;
    m_error=false;
    m_outputFile=NULL;
    m_converters[BASE_CONV_VIDEO]=NULL;
    if (m_mpeg2 || m_extractAudio) {
        m_parts=1;
        m_2pass=false;
    }

    m_error = !check();
    if (m_error)
        return;

    time = new QTime(0,0);
    m_percent=m_totalBytes=m_offset=0;
    m_remain="--:--:--";

    m_totalSize=_title->getChaptersSize(true);
    k9log::add(i18n("size : %1 MB").arg(m_totalSize/512),k9logLevel::INFO);
    if (!m_mpeg2) {
        k9log::add(i18n("audio bitrate : %1").arg(m_audioBitrate),k9logLevel::INFO);
        if (!m_extractAudio) {
            if (m_videoBitrate !="")
                k9log::add(i18n("user defined video bitrate : %1").arg(m_videoBitrate),k9logLevel::INFO);
            else
                k9log::add(i18n("calculated video bitrate : %1").arg(getBitRate(_title)),k9logLevel::INFO);
            }
    }
    QString injectName;
    k9Config config;
    injectName=config.getPrefOutput()+"/k9v"+k9Tools::randomFileName()+".tmp";

    int maxPass=0;
    int pass=0;

    //build the cell list for mpeg2 extraction
    QMultiMap <int,int> chapterCells;
    QStringList chapters;


    for ( m_part =1 ; (m_part <=m_parts) && !m_error ;m_part++) {
        if (m_2pass) {
            maxPass=2;
            pass=1;
        }
        QString passLogFileName;
        k9Config config;
        passLogFileName=QDir::cleanPath(config.getPrefOutput()+"/k9v"+k9Tools::randomFileName() +".temp");
        m_passLogFile=passLogFileName;

        do {
            if (!m_mpeg2)
                k9log::add(i18n("starting pass %1 of %2").arg(pass==0 ? 1:pass).arg(maxPass==0 ? 1 : maxPass),k9logLevel::INFO);
            m_totalBytes=0;
            m_vamps=new k9vamps(this,m_usecache);;
            m_player=new k9play(this);

            uint32_t nbSectors= m_totalSize / m_parts   ;

            uint32_t startSector= nbSectors*(m_part-1);
            uint32_t endSector= startSector+nbSectors;

            m_stderr="";
            m_title=_title;
            if (m_width=="") {
                m_width=_title->getwidth();
            }
            if (m_width.toInt() % 2 !=0)
                m_width=QString::number(m_width.toInt()+1);
	    double h=(double)_title->getheight().toInt();
	    double w=(double)_title->getwidth().toInt();
	    
	    double ratio=( m_width.toDouble()/w);
	    
  	    k9VideoCodecs videoCodecs;

	    // if a cropping zone has been defined, the aspect ratio is based on it else we keep
	    // the original aspect ratio
	    if ((m_crop.height() != (int) h) || (m_crop.width() !=(int) w)) {
		// n is the ratio between the video resolution and the rendering aspect ratio
		double n;
		if (_title->getaspectRatio() =="16:9")
		    n=(16.0 *h)/(9.0*w);
		else
		    n=(4.0 *h)/(3.0*w);

		m_aspect=QString::number(n*( (double)m_crop.width()/(double)m_crop.height()));
	    }
	    else
		m_aspect=_title->getaspectRatio();

	    if (videoCodecs.getEncoder(m_codec)=="mencoder") 
		m_aspect=m_aspect.replace(":","/");	

            if (m_height=="" || m_height=="0") {
                double height=h*ratio;
                m_height=QString::number((int)round(height+0.1));
  	        //with mencoder, video is resized after the cropping has been applied
		//=> height=height - (cropheight*aspectratio)
  	        if (videoCodecs.getEncoder(m_codec)=="mencoder") 
		   m_height= QString::number((int)(m_height.toInt() -(h-m_crop.height())*ratio));
            }

	 
	    if (videoCodecs.getEncoder(m_codec)=="ffmpeg") { 
		//with ffmpeg, crop is applied on the resized video => crop size must be calculated according to ratio
		QRect newrect(m_crop.left()*ratio,m_crop.top()*ratio,m_crop.width()*ratio,m_crop.height()*ratio);
		m_crop=newrect;
 	    }

            if (m_height.toInt()%2 !=0)
                m_height=QString::number(m_height.toInt()+1);

            if (m_audioBitrate=="")
                m_audioBitrate="128";
            if (m_size=="")
                m_size="700";
            if (m_filename=="")
                m_filename=k9Dialogs::getSaveFileName (QDir::homePath(),"*.avi", 0,i18n("Save file to disk"));
            if (m_filename =="")
                return;

            QDir d=QDir::root();
            if (d.exists(m_filename))
                d.remove(m_filename);

            m_progress->setbitrate(QString::number(getBitRate(_title)));
            if (! m_videoBitrate.isEmpty()) {
                if (!m_mpeg2 && !m_extractAudio && !m_extractMkv) {
                    m_progress->setsize(QString::number(m_parts) +" * "+m_size +i18n("MB"));
                } else
                    m_progress->setsize(m_size +i18n("MB") );
            } else
                m_progress->setsize("");
            m_player->setendSector(QString::number(endSector));
            QStringList cmd;

            m_player->setinject(injectName);
            m_player->setDevice(m_device);
            m_player->setTitle(_title->getnumTitle());

            m_player->setchapterList(getChapterList( _title));
            if (m_part==1 || m_mpeg2 ||m_extractAudio ) {
                m_player->setinitStatus(true);
                m_player->setcontinue(false);
            } else {
                m_player->setcontinue(true);
                m_player->setinitStatus(false);
            }
            if (pass==1)
                m_player->setfirstPass(true);
            else
                m_player->setfirstPass(false);

            if (pass !=1) {
                QStringList streams;
                for (int i=0;i<_title->getaudioStreamCount();i++) {
                    if (_title->getaudioStream(i)->getselected()) {
                        streams <<QString::number(_title->getaudioStream(i)->getID());
                    }
                }

                if (streams.count() >0)
                    m_player->setaudioFilter(streams.join(","));


                if (getselectedSubp( _title) !=-1) {
                    streams.clear();
                    for (int i=0; i<_title->getsubPictureCount();i++) {
                        if (_title->getsubtitle(i)->getselected()) {
                            streams << QString::number(_title->getsubtitle(i)->getID().first());
                        }

                    }
                    m_player->setsubpictureFilter(streams.join(","));
                }
            }

            m_player->setuseCache(m_usecache);


            if (m_mpeg2) {
                m_progress->setbitrate("--");
                double size;

                if (_title->getforceFactor()) {
                    size = _title->getChaptersSize_mb(true)/_title->getfactor();
                    m_player->setvampsFactor(QString::number(_title->getfactor()));
                    m_player->setforcedFactor(true);
                } else
                    size = _title->getChaptersSize_mb(true);
                m_progress->setsize(QString::number(size) +i18n("MB"));


                if (!m_splitChapters) {
                    m_outputFile=new QFile(m_filename);
                    //removes an existing file;
                    m_outputFile->remove();
                    m_outputFile->open(QIODevice::WriteOnly);
                }

            } else if (m_extractAudio) {
                //       m_offset+=m_totalBytes;
                //      buildAudioCmd(_title);
                if (!m_splitChapters)
                    buildAudioCmd(_title);
            } else {
                //cmd << "-of" << "avi";
                if (m_extractSubtitle && pass!=1)
                    buildSubtitleCmd(_title);
                buildVideoCmd(pass,m_part,_title,passLogFileName,cmd);
            }
            if ((m_parts >1 ) && (m_mpeg2|| m_extractAudio))
                ++ichapterCells;


            time->start();

            connect(m_progress,SIGNAL(sigCancel()),this,SLOT(slotCancel()));
            m_canceled=false;
            m_progress->show();

            timer->start(500);

            //connect(m_player,SIGNAL(sigPosition(uint, uint)),this,SLOT(getPosition(uint, uint)));

            QEventLoop eventLoop;

            //enters an new event loop which is stopped at end of the playing thread
            m_eventLoop= &eventLoop;
            m_player->setEventLoop(&eventLoop);
            m_player->execute();         
            Flush();

            foreach (k9ConvertAudio *c,m_converters) {
                if (c) {
                    c->end(true);
                }
            }
            foreach(k9Subtitle2Pgm *s,m_subtitles) {
                if (s) {
                    s->end();
                }
            }

            qDeleteAll(m_subtitles);
            if (m_canceled) {
                m_error=true;
            }

            if (maxPass >0)
                pass++;
            delete m_player;
            m_player = NULL;
        } while (pass<=maxPass && !m_error && m_2pass && !m_canceled);
        if (!m_canceled)
            m_offset+=m_totalBytes+ (m_vamps !=NULL ? m_vamps->getSkippedBytes():0);

    }
    if (m_mpeg2 && m_outputFile) {
        m_outputFile->close();
        delete m_outputFile;
    }
    foreach (k9ConvertAudio *c,m_convertersToDelete) {
        c->end();
        delete c;
    }
    QFile::remove(injectName);
    if (!m_msgError.isEmpty())
        k9Dialogs::error (m_msgError, i18n("Encoding error"),m_ErrorDetail);

}


void k9MP4Enc::buildSubtitleCmd(k9DVDTitle *title) {
    for (int i=0; i< title->getsubPictureCount();i++) {
        if (title->getsubtitle(i)->getselected()) {
            QStringList cmd;
            QString path=QDir::cleanPath( QString("%1/title%2-part%3-subtitle%4%5").arg(m_filename).arg(m_title->getnumTitle()).arg(m_part).arg(i+1).arg(""));
            QString path2=QDir::cleanPath( QString("%1/title%2-part%3-subtitle%4%5").arg(m_filename).arg(m_title->getnumTitle()).arg(m_part).arg(i+1).arg("mpg"));

            QFile::remove(path+".sub");
            QFile::remove(path+".idx");
            k9Subtitle2Pgm *subtitle2pgm=new k9Subtitle2Pgm(this);
            m_subtitles[title->getsubtitle(i)->getID().first()]=subtitle2pgm;
            subtitle2pgm->setBaseName(path);
            for (int ipalette=0;ipalette<16;ipalette++) {
                QString s;
                title->getpalette(ipalette,s);
                subtitle2pgm->setPalette(ipalette,s.toUInt());
            }
            subtitle2pgm->setLangCod(title->getsubtitle(i)->getlangCod());
            subtitle2pgm->setTrackCod(0x20+title->getsubtitle(i)->getID().first()-1);

            k9ResultingFile *rf=new k9ResultingFile(this);
            rf->title=m_title->getnumTitle();
            rf->filename=path+".idx";
            rf->subtitle=true;
            m_resultingFiles.insert(rf->title,rf);

        }

    }

}

void k9MP4Enc::buildAudioCmd(k9DVDTitle *title) {
    k9AudioCodecs *audioCodecs=new k9AudioCodecs(0);
    m_encoder=FFMPEG;
    for (int i=0;i<title->getaudioStreamCount();i++) {
        if (title->getaudioStream(i)->getselected()) {

            k9DVDAudioStream *stream = title->getaudioStream(i);
            QStringList cmd;
            QString sCodec="";

            sCodec=audioCodecs->getCodecName(m_audioCodec);

            QString sAOption=replaceParams(audioCodecs->getOptions(m_audioCodec)).trimmed();

            m_progress->setTitleLabel(i18n("Encoding %1").arg(sCodec));
            QString path,ext=audioCodecs->getExtension(m_audioCodec) ;
            if (!ext.startsWith("."))
                ext="."+ext;

            QDir().mkpath(m_filename);
            path=QDir::cleanPath( QString("%1/title%2-part%3-audio%4-%6-%5").arg(m_filename).arg(m_title->getnumTitle()).arg(m_currentChapter).arg(i+1).arg(ext).arg(stream->getlanguage()) );
            QFile::remove(path);


            cmd  << m_ffmpegPath << "-i" << "/dev/stdin" <<sAOption.split(" ") <<  KShell::quoteArg(path) ;

            k9log::add(i18n("starting thread : %1").arg(cmd.join(" ")),k9logLevel::INFO);
            k9ConvertAudio *converter=new k9ConvertAudio("",cmd);
            m_converters[BASE_CONV_AUDIO+i]=converter;
            m_convertersToDelete << converter;
            
        }
    }

    delete audioCodecs;

}

void k9MP4Enc::buildVideoCmd(int pass,int part,k9DVDTitle *title,const QString &passLogFileName,QStringList &cmd) {
    k9VideoCodecs videoCodecs;
    if (videoCodecs.getEncoder(m_codec)=="ffmpeg")
        buildFFMpegCmd(pass,title,cmd);
    else
        buildMEncoderCmd(pass,part,title,passLogFileName,cmd);

}


void k9MP4Enc::buildFFMpegCmd(int pass,k9DVDTitle *title,QStringList &cmd) {

    QString sPass="";
    QString sCodec="";

    k9VideoCodecs *videoCodecs=new k9VideoCodecs(0);
    k9AudioCodecs *audioCodecs=new k9AudioCodecs(0);
    m_encoder=FFMPEG;
    QString sVOption;
    m_pass=pass;
    switch (pass) {
    case 1:
        sVOption=replaceParams(videoCodecs->getOptions1(m_codec));
        break;
    case 2:
        sVOption=replaceParams(videoCodecs->getOptions2(m_codec));
        break;
    default:
        sVOption=replaceParams(videoCodecs->getOptions0(m_codec));
        break;
    }


    sCodec=videoCodecs->getCodecName(m_codec);
    sVOption=sVOption.trimmed();
    cmd << m_ffmpegPath << "-i" << "/dev/stdin";

    cmd << sVOption.split(" ");

    if (pass >0)
        m_progress->setTitleLabel(i18n("Encoding %1").arg(sCodec)+" - "+i18n("pass %1").arg(pass));
    else
        m_progress->setTitleLabel(i18n("Encoding %1").arg(sCodec));

    if (m_fourcc !="")
        cmd << "-vtag" << m_fourcc;
    else if (videoCodecs->getFourcc(m_codec)!="")
        cmd << "-vtag" << videoCodecs->getFourcc(m_codec);

    QString sAOption="";
    sAOption=replaceParams(audioCodecs->getOptions(m_audioCodec)).trimmed();

    QStringList slNewAudio;
    delete audioCodecs;
    delete videoCodecs;
    bool audio=false;
    //looking for first audio selected
    int nbAudio=0;
    if (m_pass != 1 ) {
        for (int i=0;i<title->getaudioStreamCount();i++) {
            if (title->getaudioStream(i)->getselected()) {
                if (nbAudio>0)
                    slNewAudio << "-newaudio";
                else
                    cmd << sAOption.split(" ");
                audio=true;
                nbAudio++;
            }
        }
    }

    QString path=m_filename;

    if (m_pass==1) {
        path="/dev/null";
    } else if (m_extractMkv) {
        QString ext(".avi");
        QDir().mkpath(m_filename);
        path=QDir::cleanPath( QString("%1/title%2-part%3-video%4").arg(m_filename).arg(m_title->getnumTitle()).arg(m_part).arg(ext));
        QFile::remove(path);

        k9ResultingFile *rf=new k9ResultingFile(this);
        rf->title=m_title->getnumTitle();
        rf->filename=path;
        rf->video=true;
        m_resultingFiles.insert(rf->title,rf);
    }else {
	if (m_parts>1) {
		QString ext=m_filename.section(".",-1);
		if (ext!="")
		ext="."+ext;
		path=m_filename.left(m_filename.length()-ext.length());
		path=path+QString::number(m_part)+ext;
	}
    }

    if (path.toUpper().endsWith("MPEG") || path.toUpper().endsWith("MPG"))
        cmd << "-f" << "mpeg";
    else if (path.toUpper().endsWith("AVI") || m_pass==1)
        cmd << "-f" << "avi";
    else {
        //      cmd << "-of" << "lavf";
        //      cmd << "-lavfopts" << "i_certify_that_my_video_stream_does_not_use_b_frames";
    }
    //synchronize audio
    cmd << "-async" << "1" ;
    cmd << "-y"  << KShell::quoteArg(path);
    cmd << slNewAudio;
//    if (m_extractMkv) {
    k9log::add(i18n("starting thread : %1").arg(cmd.join(" ")),k9logLevel::INFO);
    k9ConvertAudio *converter=new k9ConvertAudio("",cmd);
    //converter->setDebug(true);
    m_converters[BASE_CONV_VIDEO]=converter;
    m_convertersToDelete << converter;
    if (m_extractMkv && m_pass!=1)
        for (int i=0;i<title->getaudioStreamCount();i++)
            if (title->getaudioStream(i)->getselected())
                m_converters[BASE_CONV_AUDIO+i]=converter;
//   }
}



void k9MP4Enc::buildMEncoderCmd(int pass,int part,k9DVDTitle *title,const QString &passLogFileName,QStringList &cmd) {
    m_encoder=FFMPEG;
    bool audio=false;
    QString sPass="";
    QString sCodec="";

    k9AudioCodecs *audioCodecs=new k9AudioCodecs(0);
    k9VideoCodecs *videoCodecs=new k9VideoCodecs(0);

    QString sVOption;
    m_pass=pass;
    switch (pass) {
    case 1:
        sVOption=replaceParams(videoCodecs->getOptions1(m_codec));
        break;
    case 2:
        sVOption=replaceParams(videoCodecs->getOptions2(m_codec));
        break;
    default:
        sVOption=replaceParams(videoCodecs->getOptions0(m_codec));
        break;
    }
    sCodec=videoCodecs->getCodecName(m_codec);
    sVOption=sVOption.trimmed();
    int pos;
    cmd << m_mencoderPath << "/dev/stdin";
    if (pass >0)
        cmd << "-passlogfile" << passLogFileName;

    cmd << sVOption.split(" ");

    QString sAOption="";
    sAOption=replaceParams(audioCodecs->getOptions(m_audioCodec)).trimmed();


    if (pass >0)
        m_progress->setTitleLabel(i18n("Encoding %1").arg(sCodec)+" - "+i18n("pass %1").arg(pass));
    else
        m_progress->setTitleLabel(i18n("Encoding %1").arg(sCodec));

    if (m_fourcc !="")
        cmd << "-ffourcc" << m_fourcc;
    else if (videoCodecs->getFourcc(m_codec)!="")
        cmd << "-ffourcc" << videoCodecs->getFourcc(m_codec);

    delete audioCodecs;
    delete videoCodecs;

    //looking for first audio selected
    for (int i=0;i<title->getaudioStreamCount();i++) {
        if (title->getaudioStream(i)->getselected()) {
            //cmd << "-oac" << sAOption;
            pos=sAOption.indexOf("-af");
            if (pos==-1)
                cmd << "-af" <<QString("volume=%1").arg(m_audioGain);
            else
                sAOption=sAOption.insert(pos+4,QString("volume=%1,").arg(m_audioGain));
            cmd << sAOption.split(" ");

            audio=true;
            break;
        }
    }

    if (getselectedSubp( title) !=-1) {
        cmd << "-sid" << QString::number(getselectedSubp( title));
        cmd << "-ffactor" << "1" << "-spualign" << "2" << "-sub-bg-alpha" << "1";
    }

    if (!audio)
        cmd << "-nosound";

    QString path=m_filename;

    if (m_parts>1) {
        QString ext=m_filename.section(".",-1);
        if (ext!="")
            ext="."+ext;
        path=m_filename.left(m_filename.length()-ext.length());
        path=path+QString::number(part)+ext;
    }
    if (pass==1)
        cmd << "-o" << "/dev/null";
    else
        cmd <<"-o" << KShell::quoteArg(path);
    if (path.toUpper().endsWith("MPEG") || path.toUpper().endsWith("MPG"))
        cmd << "-of" << "mpeg";
    else if (path.toUpper().endsWith("AVI"))
        cmd << "-of" << "avi";
    else {
        //      cmd << "-of" << "lavf";
        //      cmd << "-lavfopts" << "i_certify_that_my_video_stream_does_not_use_b_frames";
    }
    k9log::add(i18n("starting thread : %1").arg(cmd.join(" ")),k9logLevel::INFO);
    k9ConvertAudio *converter=new k9ConvertAudio("",cmd);
    converter->setDebug(false);
    m_converters[BASE_CONV_VIDEO]=converter;
    m_convertersToDelete << converter;
}

void k9MP4Enc::slotCancel() {
    k9log::add(i18n("cancel requested"),k9logLevel::WARNING);
    m_canceled=true;
    timer->stop();
    m_player->abort();
    foreach (k9ConvertAudio *c,m_converters) {
        if (c)  {
            c->end(false);
        }
    }

}


QString k9MP4Enc::replaceParams(QString _value) {
    QString str=_value;
    str.replace("$PASSLOGFILE",m_passLogFile);
    str.replace("$PASS",QString::number(m_pass));
    str.replace("$WIDTH",m_width);
    str.replace("$HEIGHT",m_height);
    str.replace("$VIDBR",QString::number(getBitRate(m_title)));
    str.replace("$AUDBR",m_audioBitrate);
    int c=m_crop.top();
    if (c%2 !=0) c++;
    str.replace("$CROPTOP",QString::number(c));
    c=m_crop.left();
    if (c%2 !=0) c++;
    str.replace("$CROPLEFT",QString::number(c));
    c=m_height.toInt()- m_crop.bottom()-1;
    if (c%2 !=0) c++;
    str.replace("$CROPBOTTOM",QString::number(c));
    c=m_width.toInt()-m_crop.right()-1;
    if (c%2 !=0) c++;
    str.replace("$CROPRIGHT",QString::number(c));
    str.replace("$CROPWIDTH",QString::number(m_crop.width()));
    str.replace("$CROPHEIGHT",QString::number(m_crop.height()));
    //str.replace("$ASPECT",QString::number((double)m_crop.width()/(double)m_crop.height()));
    str.replace("$ASPECT",m_aspect);
    return str;
}


int k9MP4Enc::getBitRate(k9DVDTitle *_title) {
    // bitrate video = (MB *8388.608) /SEC    - bitrate audio

    if (m_videoBitrate!="") {
        m_progress->setsize("");
        return  m_videoBitrate.toInt();
    } else {
        int size=m_size.toInt();
        float titleSize=_title->getChaptersSize_mb( true);
        if ( titleSize< (float)size)
            size=(int)(titleSize/m_parts) ;
        m_progress->setsize(QString::number(m_parts)+" * " +QString::number(size) +i18n("MB") );
        QTime t1(0,0);
        int sec=t1.secsTo(_title->getSelectedLength());
        if (sec==0) return 0;
        int nbAudioStreams=0;
        for (int i=0;i<_title->getaudioStreamCount();i++)
            if (_title->getaudioStream(i)->getselected())
                nbAudioStreams++;

        nbAudioStreams=1;

        int bitrate=((8*size*m_parts*1024)-(m_audioBitrate.toInt()*nbAudioStreams*sec))/sec;


//	qDebug() << "bitrate:" << bitrate << " sec:" <<sec;
        return bitrate;
    }
}


void k9MP4Enc::timerDone() {
    if (m_player) {
        if (m_converters[BASE_CONV_VIDEO])
            m_progress->setfps(m_converters[BASE_CONV_VIDEO]->getFps());
        if (m_totalSize !=0) {

            m_percent=(float)(m_offset+m_totalBytes+( m_vamps!=NULL? m_vamps->getSkippedBytes():0)) / (float)m_totalSize;
//            qDebug() <<"offset" << m_offset << " total readen:" <<  m_totalBytes << " total skipped : " << (m_vamps!=NULL? m_vamps->getSkippedBytes():0) << " total title :" << m_totalSize << "percent" << m_percent;

        }
        QTime time2(0,0);
        time2=time2.addMSecs(time->elapsed());
        if (m_percent>0) {
            QTime time3(0,0);
            time3=time3.addMSecs((uint32_t)(time->elapsed()*(1/m_percent)));
            m_remain=time3.toString("hh:mm:ss");
        }

        m_percent*=100;
        m_progress->setProgress((int)m_percent);
        m_progress->setremain(time2.toString("hh:mm:ss") +" / " +m_remain);
        m_progress->setImage(m_player->getSaveImage()->getFileName());
       // m_progress->Update();
    }
}

bool k9MP4Enc::isCanceled() {
    return (m_canceled || m_error);
}


void k9MP4Enc::getOutput(eStreamType streamType,int streamNumber,uchar *buffer,uint32_t size) {

    if (isCanceled())
        return;
    if ((streamType==NAV) && !m_mpeg2) {
        pci_t	 pci_pack;

        k9Ifo2::navRead_PCI(&pci_pack,buffer-14+ 0x2d);
        k9DVDChapter *ch=m_title->getChapterFromSector(pci_pack.pci_gi.nv_pck_lbn);
        bool chapterChanged=ch->getnum()!=m_currentChapter;
        m_currentChapter=ch->getnum();
        if (m_extractAudio && m_splitChapters && chapterChanged) {
            foreach( k9ConvertAudio *c, m_convertersToDelete) {
                if (!c->isRunning()) {
                    m_convertersToDelete.removeOne(c);
                    delete c;
                }

            }

            foreach (k9ConvertAudio *c,m_converters) {
                if (c)  {
                    c->end(false);
                }
            }
            buildAudioCmd(m_title);
        }
    }

    if (m_converters.contains(streamNumber -1 +BASE_CONV_AUDIO ) && (streamType==AUDIO)) {
        k9ConvertAudio *c=m_converters[BASE_CONV_AUDIO -1 +streamNumber];
        if (!c->Error())
            c->addData(buffer,size);
        else {
            m_player->setAborted(true);
            m_error=true;
            m_msgError=i18n("An error occured while encoding the %1 stream").arg(i18n("audio"));
            m_ErrorDetail=c->getOutput();
            k9log::add(m_msgError,k9logLevel::ERROR);
        }
    }

    if (streamType==NAV) {
        foreach(k9Subtitle2Pgm *sub,m_subtitles) {
            sub->addData(buffer,size);
        }

    }

    //bool copyNav=(m_encoder==FFMPEG ) && (streamType==NAV);
    bool copyNav=(streamType==NAV);


    if ((streamType==VIDEO || copyNav) ||( !m_mpeg2 && !m_extractAudio && !m_extractMkv)) {
        k9ConvertAudio *c=m_converters[BASE_CONV_VIDEO];
        if (c)  {
            if (!c->Error())
                c->addData(buffer,size);
            else {
                m_player->setAborted(true);
                m_error=true;
                m_msgError=i18n("An error occured while encoding the %1 stream").arg(i18n("video"));
                m_ErrorDetail=c->getOutput();
                k9log::add(m_msgError,k9logLevel::ERROR);
            }
        }
    }


    if ((streamType==SUBPICTURE) && (m_subtitles.contains(streamNumber))) {

        m_subtitles[streamNumber]->addData(buffer,size);
    }
}


void k9MP4Enc::getOutput(uchar *buffer,uint32_t size) {
    m_totalBytes+= size/2048  ;
    if (m_mpeg2 ) {
        //waiting for free space in the fifo buffer (readen by mencoder process)
        m_fifo.addData(buffer,size);
        //Exits the eventloop executed by flush when fifo buffer is empty
        m_eventLoop->exit();
    }
}


void k9MP4Enc::Flush() {
    QIODevice *output;
    if (!m_splitChapters)
        output=m_outputFile;
    else
        output=NULL;
    uchar buffer[40960];
    int size;
    do {
        while (m_fifo.count()>0 ) {
            size=m_fifo.readData(buffer,2048);

            if (m_mpeg2 && m_splitChapters && k9Cell::isNavPack(buffer)) {
                pci_t	 pci_pack;

                k9Ifo2::navRead_PCI(&pci_pack,buffer+ 0x2d);
                k9DVDChapter *ch=m_title->getChapterFromSector(pci_pack.pci_gi.nv_pck_lbn);

                if ((ch->getnum() != m_currentChapter) || !output) {
                    m_currentChapter=ch->getnum();
                    QString path=m_filename;

                    QString ext=m_filename.section(".",-1);
                    if (ext!="")
                        ext="."+ext;
                    path=m_filename.left(m_filename.length()-ext.length());
                    //path=QString("%1-chapter%2-cell%3%4").arg(path).arg(ichapterCells.key()).arg(ichapterCells.value()).arg(ext);
                    path=QString("%1-chapter%2%3").arg(path).arg(m_currentChapter).arg(ext);

                    if (m_outputFile)
                        m_outputFile->close();
                    m_outputFile=new QFile(path);
                    //removes an existing file;
                    m_outputFile->remove();
                    m_outputFile->open(QIODevice::WriteOnly);
                    output=m_outputFile;
                }
            }

            if (output)
                output->write((char*)buffer,size);
        }
        if (!m_player->terminated() && !m_canceled)
            m_eventLoop->exec();
    } while (!m_player->terminated() && !m_canceled && m_mpeg2);

    m_player->wait();

}



k9MP4Enc::~k9MP4Enc() {
    delete m_progress;

}



QMultiMap< int, k9ResultingFile* > k9MP4Enc::getResultingFiles() const {
    return m_resultingFiles;
}


void k9MP4Enc::setSplitChapters(bool theValue) {
    m_splitChapters = theValue;
}

