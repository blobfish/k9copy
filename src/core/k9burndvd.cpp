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

#include "k9burndvd.h"
#include <QDir>
#ifdef BACKLITE
#include <QMessageBox>
#include <QFileDialog>
#else
#include <KMessageBox>
#include <KFileDialog>
#include "k9cddrive.h"
#endif
#include <kdialog.h>
#include <QApplication>

k9BurnDVD::k9BurnDVD()
        : QObject(NULL) {
    cancelled=false;
    useK3b=false;
    autoBurn=false;
    m_eject=false;
    volId="K9Copy";
    iso=false;
    m_filename="";
    progress=new k9BurnProgress(NULL);
#ifdef BACKLITE
    progress->setMovie(":/icons/anim");
#else
    QString s= KGlobal::dirs()->findResource( "data", "k9copy/anim.mng");
    progress->setMovie(s);
#endif
    m_cdrkit=k9Tools::checkProgram("genisoimage") && k9Tools::checkProgram("wodim");
    m_config=new k9Config;
}



k9BurnDVD::~k9BurnDVD() {
   delete m_config;
   delete progress;
}

/** Read property of QString burnDevice. */
const QString& k9BurnDVD::getburnDevice() {
    return burnDevice;
}
/** Write property of QString burnDevice. */
void k9BurnDVD::setburnDevice( const QString& _newVal) {
    burnDevice = _newVal;
}
/** Write property of QString workDir. */
void k9BurnDVD::setworkDir( const QString& _newVal) {
    workDir = _newVal;
    if (!workDir.endsWith("/"))
        workDir +='/';
}

void k9BurnDVD::makeIso(QString _filename) {
    iso=true;
    m_filename=_filename;
}

void k9BurnDVD::setvolId(QString _newVal) {
    volId=_newVal.replace(" ","_");
}

void k9BurnDVD::setUseK3b(bool _newVal) {
    useK3b=_newVal;
}

void k9BurnDVD::setAutoBurn(bool _newVal) {
    autoBurn=_newVal;
}
void k9BurnDVD::burn() {
    if (!useK3b) {
        if (! m_cdrkit)
            burnWithGrowisofs();
        else
            burnWithGenisoimage();
    } else
        burnWithK3b();
}

void k9BurnDVD::burnWithK3b() {
    k9Process *k3b = new k9Process();

    k9Config config;
#ifdef BACKLITE
    *k3b << "k3b";
#else
    *k3b << config.getPrefk3bPath(true);
#endif
    *k3b << QDir::cleanPath(workDir+ "/VIDEO_TS");
    *k3b << "-videodvd";
    k3b->startDetached ();
    delete k3b;
}

void k9BurnDVD::mkisoSizeStderr(char *buffer, int buflen) {
    QString c=QString::fromLatin1( buffer,buflen);//   (proc2->readStderr());
    imageSize=c.replace("\n","");
    ;
}

void k9BurnDVD::mkisoSizeStdout(char *buffer, int buflen) {
    QString c=QString::fromLatin1(buffer,buflen);// (proc2->readStdout());
    imageSize=c.replace("\n","");
    ;
}

const QString &k9BurnDVD::getImageSize() {
    QString c;
    c="mkisofs";
    proc2=new k9Process;
    *proc2 << c;
    *proc2 <<"-quiet";
    *proc2 <<"-print-size";
    *proc2 <<"-dvd-video";
    *proc2 <<"-udf";
    *proc2 <<"-r";
    *proc2 <<"-V "+volId;
    *proc2 << KShell::quoteArg (QDir::cleanPath(workDir));

    connect( proc2, SIGNAL(receivedStderr(char *, int )),this, SLOT(mkisoSizeStderr(char *, int)) );
    connect( proc2, SIGNAL(receivedStdout(char *, int)),this, SLOT(mkisoSizeStdout(char *, int)) );
    if (proc2->start()) {
        proc2->sync();
        if (proc2->exitStatus()==0) {
            delete proc2;
            k9log::add(i18n("image size : %1").arg(imageSize),k9logLevel::INFO);
            return imageSize;
        }
    }
    delete proc2;
    imageSize="0";
    return imageSize;
}

void k9BurnDVD::getGenisoimageCmd(QStringList &_cmd,QString _fileName,bool _printSize) {
    QString progname=m_config->getPrefGenisoimagePath(true);
    _cmd << progname;
    _cmd << "-gui";
    _cmd << "-v";
    _cmd << "-graft-points";
    _cmd << "-volid" << volId;
    _cmd  <<"-appid" << "k9copy";
    _cmd << "-volset-size" << "1";
    _cmd << "-volset-seqno" << "1";
    _cmd << "-no-cache-inodes" << "-udf";
    _cmd << "-iso-level" << "1";
    _cmd << "-dvd-video";
    if (!_printSize) {
        if (_fileName !="") {
            _cmd <<"-o";
            _cmd << KShell::quoteArg(_fileName);
        }
    } else {
        _cmd << "-print-size" << "-quiet";
    }
    _cmd << KShell::quoteArg(QDir::cleanPath(workDir ));

}

void k9BurnDVD::getWodimCmd(QStringList &_cmd) {
    _cmd << "|" + m_config->getPrefWodimPath(true);
    _cmd << "-dao";
    _cmd << "-overburn";
    _cmd << "-data";
    _cmd << "-v";

    if (m_speed !=i18n("default"))
        _cmd << QString("speed=%1").arg(m_speed);
    _cmd << QString("dev=%1").arg(burnDevice);
    // _cmd << QString("tsize=%1s").arg(getImageSize());
    _cmd << QString("tsize=%1s").arg(imageSize);
    _cmd << "-";
}

void k9BurnDVD::burnWithGenisoimage() {
    time = new QTime(0,0);
    time->start();
    progress->setWindowTitle(KDialog::makeStandardCaption(i18n("Burning DVD"),progress));
    progress->setTitle(i18n("Burning DVD"));
    proc=progress->getProcess();

    bool bok=false;

    while (!cancelled && !bok) {
        burnSpeed=0;
        QString fileName="";
        if (iso) {
            fileName=m_filename;
            if (fileName =="")
                fileName=k9Dialogs::getSaveFileName (QDir::homePath(),"*.iso", 0,i18n("Save image to disk"));
            if (fileName =="") {
                cancelled=true;
            }
        }
        proc->clearProgram();
        QStringList procCmd,proc2Cmd;
        getGenisoimageCmd(procCmd,fileName,false);
        connect( proc, SIGNAL(receivedStderr(char *, int)),this, SLOT(growisoStderr( char *, int)) );
        connect( proc, SIGNAL(receivedStdout(char *, int)),this, SLOT(growisoStdout( char *, int)) );

        if (!iso) {
            k9Process *proc2=new k9Process(NULL,NULL);
            getGenisoimageCmd(proc2Cmd,"",true);
            connect( proc2, SIGNAL(receivedStderr(char *, int )),this, SLOT(mkisoSizeStderr(char *, int)) );
            connect( proc2, SIGNAL(receivedStdout(char *, int)),this, SLOT(mkisoSizeStdout(char *, int)) );
            k9log::add(i18n("computing image size"),k9logLevel::INFO);
            proc2->setShellCommand(proc2Cmd.join(" "));;
            k9log::add(i18n("starting thread : %1").arg(proc2Cmd.join(" ")),k9logLevel::INFO);
            proc2->start();
            proc2->sync();
            getWodimCmd(procCmd);
	    if (m_eject)
 	        eject(burnDevice);

            if (!autoBurn) {
#ifdef BACKLITE
                if (QMessageBox::warning((QWidget*)0,i18n("DVD burning"),i18n("Insert a recordable DVD"),QMessageBox::Retry | QMessageBox::Cancel)!= QMessageBox::Retry)
#else
                if ( k9Dialogs::warningContinueCancel ( i18n("Insert a recordable DVD"), i18n("DVD burning"))!=KMessageBox::Continue)
#endif
                    cancelled=true;
            }
        }
        proc->setShellCommand(procCmd.join(" "));

        if (!cancelled) {
            k9log::add(i18n("start burning"),k9logLevel::INFO);
            k9log::add(i18n("starting thread : %1").arg(procCmd.join(" ")),k9logLevel::INFO);

            int res=progress->execute();
            if ( res==-1 ) {
                k9log::add(i18n("Error"),k9logLevel::ERROR);
                k9Dialogs::error( i18n("Error burning DVD :\n")+i18n("Unable to run %1").arg("genisoimage"), i18n("DVD burning") );
                cancelled=true;
            } else {

                if ((proc->exitStatus()==0)&& (proc->exitCode()==0)) {
                    bok=true;
                    k9log::add(i18n("Success"),k9logLevel::INFO);

                    k9Dialogs::information( i18n("DVD Burning finished"), i18n("DVD burning") );

                } else {
                    k9log::add(i18n("Error"),k9logLevel::ERROR);
                    QString c;
		    if(!iso)    
			eject(burnDevice);

                    c=i18n("An error occured while Burning DVD") +"\n" +i18n("Insert an other DVD");
#ifdef BACKLITE
                    if ( k9Dialogs::warningContinueCancel ( c, i18n("DVD burning"),lastMsg) != QMessageBox::Ignore) {
#else
                    if ( k9Dialogs::warningContinueCancel ( c, i18n("DVD burning"),lastMsg)!=KMessageBox::Continue) {
#endif
                        cancelled=true;
                    }
                }
            }
        }
    }
}

void k9BurnDVD::burnWithGrowisofs() {
    time = new QTime(0,0);
    time->start();
    progress->setWindowTitle(KDialog::makeStandardCaption(i18n("Burning DVD"),progress));
    progress->setTitle(i18n("Burning DVD"));
    bool bok=false;


    while (!cancelled && !bok) {
        burnSpeed=0;
        QString c,progname;
        if (iso)
            progname="mkisofs";
        else
            progname="growisofs";
        proc=progress->getProcess();
        *proc << progname;

        if (!iso) {
            *proc <<"-overburn";
            *proc <<"-Z";
            *proc <<burnDevice;
            *proc <<"-use-the-force-luke=tty";
            *proc <<"-use-the-force-luke=tracksize:"+getImageSize();
            *proc <<"-use-the-force-luke=dao:" + imageSize;
            *proc <<"-dvd-compat";
            if (m_speed !=i18n("default"))
                *proc <<"-speed=" + m_speed;

        } else {
            QString fileName=m_filename;
            if (fileName =="")
                fileName=k9Dialogs::getSaveFileName (QDir::homePath(),"*.iso", 0,i18n("Save image to disk"));
            if (fileName !="") {
                *proc <<"-o";
                *proc <<fileName;
            } else
                cancelled=true;

        }

        *proc <<"-dvd-video";
        *proc <<"-udf";
        *proc << "-r";
        *proc <<"-V "+volId;
        *proc <<QDir::cleanPath(workDir );

        connect( proc, SIGNAL(receivedStderr(char *, int)),this, SLOT(growisoStderr(char *, int)) );
        connect( proc, SIGNAL(receivedStdout(char *, int)),this, SLOT(growisoStdout(char *, int)) );
        if (!autoBurn && !iso) {
            c=i18n("Insert a recordable DVD");
#ifdef BACKLITE
            if ( k9Dialogs::warningContinueCancel ( c, i18n("DVD burning"))!=QMessageBox::Ignore)
#else
            if ( k9Dialogs::warningContinueCancel ( c, i18n("DVD burning"))!=KMessageBox::Continue)
#endif
                cancelled=true;
        }
        if (!cancelled) {
            k9log::add(i18n("start burning"),k9logLevel::INFO);
            k9log::add(i18n("starting thread : %1").arg(proc->debug()),k9logLevel::INFO);

            int res=progress->execute();
            if ( res==-1 ) {
                k9log::add(i18n("Error"),k9logLevel::ERROR);

                k9Dialogs::error( i18n("Error burning DVD :\n")+i18n("Unable to run %1").arg(progname), i18n("DVD burning") );
                cancelled=true;
            } else {

                if (proc->exitStatus()==0) {
                    k9log::add(i18n("Success"),k9logLevel::INFO);

                    bok=true;

                    k9Dialogs::information( i18n("DVD Burning finished"), i18n("DVD burning") );
                    //delete temporary files
                    //        clearOutput(workDir+"dvd");
                } else {
                    k9log::add(i18n("Error"),k9logLevel::ERROR);

                    QString c;
                    c=i18n("An error occured while Burning DVD") +"\n" +i18n("Insert an other DVD");
#ifdef BACKLITE
                    if ( k9Dialogs::warningContinueCancel ( c, i18n("DVD burning"),lastMsg)!=QMessageBox::Ignore) {
#else
                    if ( k9Dialogs::warningContinueCancel ( c, i18n("DVD burning"),lastMsg)!=KMessageBox::Continue) {
#endif
                        cancelled=true;
                    }
                }
            }
        }
    }
    delete time;

}

/** No descriptions */
void k9BurnDVD::growisoStderr(char *buffer, int buflen) {
    QString str(QByteArray( buffer,buflen));// (proc->readStderr());
    QStringList strl=str.split("\n");
    foreach (QString c,strl) {
        c=c.replace("\r","");
        if (c.isEmpty())
            continue;
        //qDebug() << c;
        char s[255];
        int a,b;
        int pos;
        lastMsg << c;
        if (lastMsg.count() >50)
            lastMsg.removeFirst();
        if (c.contains("Current Write Speed")) {
            sscanf(c.toUtf8(),"%s \"Current Write Speed\" is %d.%d",s,&a,&b);
            burnSpeed=a+b/10;
        }
        if (c.contains("Speed set to")) {
            sscanf(c.toUtf8(),"Speed set to %d",&a);
            burnSpeed=a/1385;
        }

        progress->setTitle(i18n("Burning DVD"));
        progress->setLabelText(i18n("Current write speed :%1 x").arg(burnSpeed));
        if (c.contains("% done")) {
            pos=c.indexOf("%");
            if (pos!=-1) {
                c=c.mid(1,pos-4);
                //progress->setLabelText(c);
                progress->setProgress(c.toInt(),100);
                float m_percent=c.toFloat()/100;
                QTime time2(0,0);
                time2=time2.addMSecs(time->elapsed());
                if (m_percent>0) {
                    QTime time3(0,0);
                    QString m_remain;
                    time3=time3.addMSecs((uint32_t)(time->elapsed()*(1/m_percent)));
                    m_remain=time3.toString("hh:mm:ss");
                    progress->setElapsed(time2.toString("hh:mm:ss") +" / " +m_remain);

                }
            }
        }
        else k9log::add(i18n("%1").arg(c),k9logLevel::INFO);
    }
}
void k9BurnDVD::growisoStdout(char *buffer, int buflen) {
    QString c=QString::fromLatin1( buffer,buflen);// (proc->readStdout());
    int pos;
    pos=c.indexOf("STAT");
    if (pos!=-1) {
        c=c.mid(pos);
        progress->setLabelText(c);
    }
}


void k9BurnDVD::eject(const QString &device) {
    k9Process *process =new k9Process();
    if (k9Tools::checkProgram("kdeeject"))
        *process <<"kdeeject" << device;
    else    
        *process <<"eject" << device;
    process->start();
    process->wait();
    delete process;

}

void k9BurnDVD::setEject(bool theValue) {
    m_eject = theValue;
}
