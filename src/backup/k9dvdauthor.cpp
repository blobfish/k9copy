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
#include "k9dvdauthor.h"
#include "k9dvdtitle.h"
#include "k9dvdbackup.h"

#include "k9tools.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <qdom.h>
#include <qfile.h>
#include <qnamespace.h>
#include <qprocess.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qdatastream.h>
#include <qfile.h>
#include <qlabel.h>
#include <qprogressdialog.h>
#include <kdialog.h>
#ifdef BACKLITE
#include <QMessageBox>
#else
#include "kmessagebox.h"
#include <klocale.h>
#include <KShell>
#endif
#include <qapplication.h>
#include <QTextStream>


class k9Progress;

k9DVDAuthor::k9DVDAuthor(QObject *DVDStruct)  {
    DVD=(k9DVD*)DVDStruct;
    xml=NULL;
    cancelled=false;
    error=false;
    if (k9Tools::getBatchCopy())
        progress=NULL;
    else
        progress = new k9Progress(k9Dialogs::getMainWidget());
//    files.setAutoDelete(true);

}
k9DVDAuthor::~k9DVDAuthor() {

    foreach (QTemporaryFile *it,files) {
        it->remove();
    }
    while (!files.isEmpty()) delete files.takeFirst();
    if (xml!=NULL)
        delete xml;

}
/** No descriptions */
void k9DVDAuthor::createXML() {
    int i;
    if (xml!=NULL)
        delete xml;
    xml=new QDomDocument();

    QDomElement root = xml->createElement( "dvdauthor" );
    root.setAttribute ("dest",workDir+DVD->getDVDTitle());
    xml->appendChild( root );

    // Create vmgm menu
    QDomElement vmgm = xml->createElement("vmgm");
    root.appendChild(vmgm);
    addMenus(vmgm);


    m_forced=0;
    m_forcedsh=0;
    //computes the size of related titles
    for (int iTitle=0; iTitle < DVD->gettitleCount();iTitle++) {
        k9DVDTitle *title=DVD->gettitle(iTitle);
        if (title->isSelected() && title->getIndexed()) {
            for (int iTitle2=0;iTitle2<title->getTitles().count() ;iTitle2++) {
                k9DVDTitle *title2=title->getTitles().at(iTitle2);
                if (title->getforceFactor()) {
                    m_forced+=title2->getsectors()*2048;
                    m_forcedsh+=(title2->getsectors()/title->getfactor())*2048;
                }
            }
        }
        //total size of forced titles
        if (title->isSelected() && title->getforceFactor()) {
            m_forced+=title->getsectors()*2048;
            m_forcedsh+=(title->getsectors()/title->getfactor())*2048;
        }

    }

    //total size of titles to copy
    m_totalSize=((uint64_t)DVD->getsizeSelected(false))*DVD_VIDEO_LB_LEN ;//+ m_totalPartSize -chapterssize*DVD_VIDEO_LB_LEN;
    m_firsttitle=true;

    for (i=0;i< DVD->gettitleCount();i++) {
        k9DVDTitle *tmp = DVD->gettitle(i);
        addTitle(root,tmp);
    }

    QString x = xml->toString();

    //QFile file(  locateLocal("tmp", "k9copy/k9author.xml" ));

    // if ( file.open( IO_WriteOnly ) ) {
    QTextStream stream( m_xml );
    xml->save(stream,1);
    m_xml->close();
    // }
}

void k9DVDAuthor::addMenus(QDomElement &titleSet) {
    int i;
    QDomElement menu,video,pgc,pre,vob,button;
    QDomText precmd;
    QDomText buttonText;
    QTime max(0,0);
    QString c,start;

    k9DVDTitle * l_track= DVD->getstart();
    if (l_track==NULL) {
        start="call vmgm menu;";
    } else {
        if ( DVD->getnewTitleNum(l_track)==-1) {
            error=true;
            QString sMsg;
            //sMsg.QString::sprintf(tr2i18n("'%s' not selected"),l_track->getname().latin1());
            sMsg=i18n("'%1' not selected").arg(l_track->getname());
            k9Dialogs::error( sMsg, i18n("authoring"));
        }
        start.sprintf("jump titleset %d menu;",DVD->getnewTitleNum(l_track));
    }
    factor=DVD->getfactor(false,false,true);
    menu=xml->createElement("menus");
    titleSet.appendChild(menu);
    video=xml->createElement("video");
    video.setAttribute("format",DVD->getFormat());
    menu.appendChild(video);
    pgc=xml->createElement("pgc");
    pgc.setAttribute("entry","title");
    menu.appendChild(pgc);
    pre=xml->createElement("pre");
    pgc.appendChild(pre);
    precmd=xml->createTextNode("");
    QString cmd="if (g0==0) \n { \n subtitle=0; \n g0=1;\n "+start+"\n}\n";
    pre.appendChild(precmd);
    int numt=0;

    for (i=0;i < DVD->gettitleCount();i++) {
        if (DVD->gettitle(i)->isSelected() && DVD->gettitle(i)->getIndexed()) {
            ++numt;
            cmd+=c.sprintf("if (g1==%d) {jump titleset %d menu;}\n", numt,numt);
        }
    }

    precmd.setNodeValue(cmd);
}

/** No descriptions */
void k9DVDAuthor::addTitle(QDomElement &root, k9DVDTitle *title) {
    int i,j;

    k9DVDSubtitle *l_sub;
    k9DVDAudioStream *l_auds;
    k9DVDTitle *l_track= title;
    QDomElement e,t,pgc;
    QString caud="",csub="",c,palette;

    if (l_track->isSelected() && l_track->getIndexed()) {
        double titleFactor;
        if (l_track->getforceFactor())
            titleFactor=l_track->getfactor();
        else
            titleFactor=factor;
        QDomElement titleSet = xml->createElement("titleset");
        root.appendChild(titleSet);
        QDomElement titleMenu = xml->createElement("menus");
        titleSet.appendChild(titleMenu);
        QDomElement video=xml->createElement("video");
        titleMenu.appendChild(video);
        video.setAttribute("format",l_track->getformat().toLower());

        QDomElement pgc = xml->createElement("pgc");
        titleMenu.appendChild(pgc);
        QDomElement pre = xml->createElement("pre");
        pgc.appendChild(pre);
        QDomText precmd=xml->createTextNode("");

        int subtitle=0,audio=-1;
        if (l_track->getDefSubtitle() != NULL) {
            for (int isub=0;isub<l_track->getsubPictureCount();isub++) {
                if (l_track->getsubtitle(isub)->getselected()) {
                    subtitle++;
                }
                if (l_track->getsubtitle(isub)==l_track->getDefSubtitle())
                    break;
            }
        }
        if (l_track->getDefAudio() != NULL) {
            for (int iaud=0;iaud < l_track->getaudioStreamCount();iaud++) {
                if (l_track->getaudioStream(iaud)->getselected()) {
                    audio++;
                }
                if (l_track->getaudioStream(iaud)==l_track->getDefAudio())
                    break;
            }
        }

        QString txtcmd;
        if (audio !=-1) {
            txtcmd="subtitle=%1;\n audio=%2;\n g1=0;jump title 1;";
            txtcmd=txtcmd.arg(subtitle+63).arg(audio);
        } else {
            txtcmd="subtitle=%1;\n g1=0;jump title 1;";
            txtcmd=txtcmd.arg(subtitle+63);
        }

        precmd.setNodeValue(txtcmd);
        pre.appendChild(precmd);

        //create palette for subpictures
        QTemporaryFile *paletteFile=new QTemporaryFile(m_tempPath+"/k9pXXXXXX.yuv");
        paletteFile->setAutoRemove(false);
        paletteFile->open();
        files.append(paletteFile);
        palette=paletteFile->fileName();

        //palette.sprintf("palette%d.yuv",l_track->getnumTitle());
        //palette= locateLocal("tmp", "k9copy/k9" +palette);
        QTextStream stream( paletteFile );
        for (j=0;j<16;j++) {
            l_track->getpalette(j,c);
            stream << c+"\n";
        }
        paletteFile->close();

        t=xml->createElement("titles");
        titleSet.appendChild(t);
        e=xml->createElement("video");
        e.setAttribute("aspect",l_track->getaspectRatio());
        e.setAttribute("format",l_track->getformat().toLower());
        if (l_track->getaspectRatio()!="4:3") {
            e.setAttribute("widescreen","nopanscan");
        }
        t.appendChild(e);

        for (i=0;i<l_track->getaudioStreamCount();i++) {
            l_auds=l_track->getaudioStream(i);
            if (l_auds->getselected()) {
                e=xml->createElement("audio");
                e.setAttribute("format",l_auds->getformat());
                e.setAttribute("channels",l_auds->getchannels());
                e.setAttribute("quant",l_auds->getquantization());
                e.setAttribute("lang",l_auds->getlangCod());
                t.appendChild(e);
                if (caud != "")
                    caud+=',';
                caud+=c.sprintf("%d",l_auds->getID());
            }
        }

        for (i=0;i<l_track->getsubPictureCount();i++) {
            l_sub=l_track->getsubtitle(i);
            if (l_sub->getselected()) {
                e=xml->createElement("subpicture");
                e.setAttribute("lang",l_sub->getlangCod());
                t.appendChild(e);
                if (csub !="")
                    csub+=',';
                csub+=c.sprintf("%d",(l_sub->getID()).first());
            }
        }

        pgc=xml->createElement("pgc");
        pgc.setAttribute("palette",palette);
        t.appendChild(pgc);
        /*
                for (i=0;i<l_track->getaudioStreamCount();i++) {
                    l_auds=l_track->getaudioStream(i);
                    if (l_auds->getselected()) {
                        e=xml->createElement("audio");
                        e.setAttribute("id",l_auds->getID()-1);
                        t.appendChild(e);
                        if (caud != "")
                            caud+=',';
                        caud+=c.sprintf("%d",l_auds->getID());
                    }
                }

                for (i=0;i<l_track->getsubPictureCount();i++) {
                    l_sub=l_track->getsubtitle(i);
                    if (l_sub->getselected()) {
                        e=xml->createElement("subpicture");
                        e.setAttribute("id",(l_sub->getID()).first()-1);
                        t.appendChild(e);
                        if (csub !="")
                            csub+=',';
                        csub+=c.sprintf("%d",(l_sub->getID()).first());
                    }
                }
        */
        if (caud !="")
            caud="--audiofilter "+caud;
        if (csub !="")
            csub="--subpicturefilter "+csub;

        int numPart=0;

        for (int iTitle=0;iTitle<=l_track->getTitles().count();iTitle++) {
            k9DVDTitle *title;
            if (iTitle==0)
                title=l_track;
            else
                title=l_track->getTitles().at(iTitle-1);

            for (i=0;i<title->getchapterCount();i++) {
                numPart++;
                uint icell=0;
                k9DVDChapter *l_chap=title->getChapter(i);
                if ((!l_chap->getSelected()) || (l_chap->getLength() <=QTime(0,0,0,1)))
                    continue;

                bool first=true;
                uint32_t chapterSize= (l_chap->getendSector()-l_chap->getstartSector())*DVD_VIDEO_LB_LEN;
                QString sChapter,sCell;
                foreach (k9ChapterCell *cell ,l_chap->cells)  {
                    icell++;

                    sCell = QString("--cell %1").arg(icell);
                    sChapter=QString("--chapter %1").arg(numPart);

                    //test
                    uint32_t itotSize = (cell->getlastSector()-cell->getstartSector())* DVD_VIDEO_LB_LEN;
                    QString file;
                    e=xml->createElement("vob");
                    QString playPgm="";
#ifdef BACKLITE
                    playPgm="/backlite";
#else
                    playPgm="/k9play";
#endif
                    if (!k9Tools::getBatchCopy())
                        playPgm +=" --gui";
                    file=QString(qApp->applicationDirPath()+ playPgm+" --play --input %1 --dvdtitle %2 %3 %4  %5  %6 --vampsfactor %7 --inputsize %8 --chaptersize %9 ")
                         .arg(KShell::quoteArg(DVD->getDevice()))
                         .arg(title->getnumTitle())
                         .arg(sChapter)
                         .arg(sCell)
                         .arg(caud)
                         .arg(csub)
                         .arg(titleFactor,0,'f',2)
                         .arg(itotSize)
                         .arg(chapterSize);
                    if (m_firsttitle) {
                        file +=" --initstatus ";
                        m_firsttitle=false;
                    }
                    if (l_track->getforceFactor()) {
                        file +=" --ffactor ";
                    }
                    file +=QString(" --inject %1 --totalsize %2 --dvdsize %3 |")
                           .arg(KShell::quoteArg(inject))
                           .arg(m_totalSize -m_forced )
                           .arg(((uint64_t)k9DVDSize::getMaxSize() *1024 *1024) - m_forcedsh);

                    e.setAttribute("file",file);
                    if (first)
                        e.setAttribute("chapters",l_chap->getTime().toString("0"));
                    pgc.appendChild(e);
                    first=false;
                }
                //          }
            }

        }
        QDomElement post = xml->createElement("post");
        pgc.appendChild(post);
        QDomText postcmd=xml->createTextNode("");

        k9DVDTitle * l_next=l_track->getnextTitle();
        if (l_next!=NULL) {
            c.sprintf("g1=%d;\ncall vmgm menu;",DVD->getnewTitleNum(l_next));
            if ( DVD->getnewTitleNum(l_next)==-1) {
                error=true;
                c=i18n("'%1' not selected").arg(l_next->getname());
                k9Dialogs::error( c, i18n("authoring"));
            }

            postcmd.setNodeValue(c);
        }
        post.appendChild(postcmd);
    }

}


const QString& k9DVDAuthor::getworkDir() {
    return workDir;
}
void k9DVDAuthor::setworkDir( const QString& _newVal) {
    workDir = _newVal;
    if (!workDir.endsWith('/'))
        workDir +='/';
}

void k9DVDAuthor::author() {
    k9Config config;
    if ( ! k9Tools::checkProgram("dvdauthor")) {
        k9Dialogs::error (i18n("Unable to run %1").arg("dvdauthor") , i18n("authoring"));
        error = TRUE;
        return;
    }


    bool burnOk=false;
    //nettoyage du répertoire de sortie
    k9Tools::clearOutput(workDir+DVD->getDVDTitle());

    time = new QTime(0,0);
    time->start();

    m_tempPath=config.getPrefOutput();
    QDir dir;
    dir.mkpath(workDir+DVD->getDVDTitle());
    dir.mkpath(m_tempPath);
    if (progress) {
        progress->setTitle(i18n("Authoring"));
        progress->setWindowTitle(KDialog::makeStandardCaption(i18n("Backup progression")));
    }
     setProgress(0,100);
    //progress->show();

    m_xml=new QTemporaryFile(m_tempPath+"/k9aXXXXXX.xml");
    m_xml->open();
    QString fileName=m_xml->fileName();

    m_inject=new QTemporaryFile(m_tempPath+"/k9vXXXXXX.inj");
    m_inject->open();
    inject=m_inject->fileName();

    if (!cancelled && !error)
        createXML();
//    if (error || cancelled)
//        delete progress;

    //run dvdauthor
    if (!cancelled  && !error) {
        QString c(config.getPrefDvdauthorPath(true));
        if (k9Tools::getBatchCopy())
            proc=new k9Process(this,0);
        else
            proc=progress->getProcess();//  new QProcess(c,0);

        *proc << c << "-x" <<  fileName;  //locateLocal("tmp", "k9copy/k9author.xml");
        connect( proc, SIGNAL(receivedStderr(char *, int)),
                 this, SLOT(DVDAuthorStderr(char *, int )) );
        connect( proc, SIGNAL(receivedStdout(char *, int )),
                 this, SLOT(DVDAuthorStdout(char *, int)) );
        //    connect(progress, SIGNAL(cancelled()), this, SLOT(stopProcess()));

        m_copied=0;
        m_lastPos=0;
        //if (m_totalSize >k9DVDSize::getMaxSize())
        //    m_totalSize=k9DVDSize::getMaxSize();
        proc-> setWorkingDirectory(workDir);
        int result=execProcess();
        if ( result==-1 ) {
            if (k9Tools::getBatchCopy())
                k9log::writeln(QString("dvdauthor error :%1").arg(lastMsg));
            else
                k9Dialogs::error(i18n("Dvdauthor error :\n") + lastMsg,i18n("authoring"));

        } else {
            cancelled=(result==0);
            if ((proc->exitStatus()==0) && (proc->normalExit()) && !cancelled && !error) {
                burnOk=true;
            } else {
                if (cancelled) {
                    //QMessageBox::critical( 0, tr2i18n("authoring"), tr2i18n("Authoring canceled"));
                    k9Dialogs::error( i18n("Authoring cancelled"),i18n("authoring"));                  
                } else
                    if (k9Tools::getBatchCopy())
                        k9log::writeln(QString("dvdauthor error :%1").arg(lastMsg));
                    else
                        k9Dialogs::error( i18n("An error occured while running DVDAuthor:\n")+lastMsg ,i18n("authoring"));
            }
        }
        //        delete proc;
//       delete progress;
        if (!burnOk) {
            error=true;
        }
    }

    delete time;
    m_xml->remove();
    m_inject->remove();
    delete m_inject;
    delete m_xml;
}


/** No descriptions */
void k9DVDAuthor::DVDAuthorStderr(char *buffer, int buflen ) {

    //QString m_stderr(proc->readStderr());
    QString m_stderr=QString::fromLatin1(buffer, buflen);
    float m_percent;
    QString m_remain;

    int pos=m_stderr.indexOf("INFOPOS:");
    if (pos!=-1) {
        if (progress)
            progress->setTitle(i18n("Authoring"));
        QString tmp=m_stderr.mid(pos);
        uint32_t totalBytes,totalSize;
        sscanf(tmp.toUtf8(),"INFOPOS: %d %d",&totalBytes,&totalSize);
        //if (totalBytes>m_lastPos)
        //   m_copied+=totalBytes - m_lastPos;
        m_copied=totalBytes;
        m_lastPos=totalBytes;
        //qDebug(QString("copied : %1   totalSize : %2").arg(m_copied).arg(m_totalSize*512));
        m_percent=(float)m_copied / (float)(m_totalSize/DVD_VIDEO_LB_LEN );


        QTime time2(0,0);
        time2=time2.addMSecs(time->elapsed());
        if (m_percent>0) {
            QTime time3(0,0);
            time3=time3.addMSecs((uint32_t)(time->elapsed()*(1/m_percent)));
            m_remain=time3.toString("hh:mm:ss");
        }

        m_percent*=100;
        setProgress(m_percent,100);
        if (progress) {
            progress->setElapsed(time2.toString("hh:mm:ss") +" / " +m_remain);
            progress->setLabelText("");
        }

    }
    /*    else {
            if (!m_stderr.startsWith("libdvdread") && m_stderr.startsWith("libdvdnav"))
    	   qDebug(m_stderr);
        }
    */
    int end;
    lastMsg=m_stderr;

    if (m_stderr.contains("STAT:")) {
        pos=m_stderr.indexOf("fixing VOBU");
        if (pos!=-1) {
            if (progress) {
                progress->setTitle(i18n("Authoring"));
                progress->setLabelText(i18n("Fixing VOBUS"));
            } else
                k9log::writeln("fixing VOBUS");

            end=m_stderr.indexOf("%");
            if (end!=-1) {
                pos =end -2;
                m_stderr=m_stderr.mid(pos,end-pos);
                m_stderr=m_stderr.trimmed();
                //progress->setLabelText(c);
                setProgress(m_stderr.toInt(),100);
            }
        }
    }

    pos=m_stderr.indexOf("INFOIMAGE:");
    if (pos!=-1 && progress) {
        progress->setImage(m_stderr.mid(pos+10));
    }



}

void k9DVDAuthor::DVDAuthorStdout(char *buffer, int buflen) {
    QString c=QString::fromLatin1( buffer,buflen);
    //(proc->readStdout());
    int pos;
    pos=c.indexOf("STAT");
    if (pos!=-1) {
        c=c.mid(pos);
        if (progress)
            progress->setLabelText(c);
        k9Tools::processEvents();
    }
}

/** No descriptions */
void k9DVDAuthor::stopProcess() {
    proc->kill();
    cancelled=true;
}

/** Read property of bool burnDVD. */
const bool& k9DVDAuthor::getburnDVD() {
    return burnDVD;
}
/** Write property of bool burnDVD. */
void k9DVDAuthor::setburnDVD( const bool& _newVal) {
    burnDVD = _newVal;
}



bool k9DVDAuthor::getError() {
    return error;
}


void k9DVDAuthor::setProgress(long _position,long _total) {
    if (k9Tools::getBatchCopy())
        k9log::writer(QString("current :%1").arg((double)_position/(double)_total));
    else
        progress->setProgress(_position,_total);
}


int k9DVDAuthor::execProcess() {
    if (!k9Tools::getBatchCopy())
        return progress->execute();
    else {
        if (! proc->isRunning()) {
            k9log::writeln(proc->debug());
            if (!proc->start())
                return -1;
        }

        //the sync method allows to wait for the process end while receiving stdout.
        proc->sync();

        if (proc->normalExit())
            return 1;
        else
            return -1;
    }
}
