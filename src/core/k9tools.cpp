//
// C++ Implementation: k9tools
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9tools.h"
#include "k9common.h"
#include <QDir>
#include <QComboBox>
#include <QApplication>
#include <QStyle>
#ifdef __FreeBSD__
#include <sys/mount.h>
#else
#include <sys/vfs.h>
#endif

#include "k9dialogs.h"
#ifndef BACKLITE
#include <kdesu/su.h>
#include <KPasswordDialog>
#include <KUser>
#endif
#include <QDebug>
#include <stdlib.h>
#include <time.h>
#include <QProcess>
#include <QThread>

QStringList m_temporaryDirs;
QString g_password="";
bool gBatchCopy=false;
Qt::HANDLE g_mainThread;

void k9Tools::setMainThread(){
    g_mainThread=QThread::currentThreadId();
}

static bool isMainThread();

const QString  k9Tools::getTempPath() {
QString sPath;
#ifdef BACKLITE
sPath=QString("%1/%2/").arg(QDir::tempPath()).arg("/k9copy/");
#else
sPath=KStandardDirs::locateLocal("tmp","k9copy/",true);
#endif
return sPath;
}

void k9Tools::addTemporaryDir(const QString & dir) {
    m_temporaryDirs << dir;
}

void k9Tools::processEvents() {
    if (QThread::currentThreadId()== g_mainThread)
        qApp->processEvents(QEventLoop::AllEvents,1);
    else
        qDebug() << "Not the main thread processEvents skipped";
}

bool k9Tools::getBatchCopy() {
    return gBatchCopy;
}

void k9Tools::setBatchCopy(bool _value) {
    gBatchCopy=_value;
}

QString k9Tools::findExe(const QString _program) {            
    QString path(qgetenv("PATH"));

    QStringList paths=path.split(":");
    foreach (QString p,paths) {
        QString fullPath=QDir::cleanPath(p+"/"+_program);
        if (QFile::exists(fullPath))
            return fullPath;
    }
    return _program;
}

bool k9Tools::checkProgram(QString _progName,bool _msg) {
#ifdef BACKLITE
    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.start("whereis "+_progName);
    if (proc.waitForFinished()) {
        QString s(proc.readAll());
        if (s.trimmed() !=_progName+":")
            return true;
    }
    if (_msg)
        k9Dialogs::error(i18n("Error starting program %1").arg(_progName),i18n("Running program"));
    return false;
#else
    bool res= KStandardDirs::findExe( _progName,NULL,false) !=NULL ;
    if (_msg && !res)
        k9Dialogs::error(i18n("Error starting program %1").arg(_progName),i18n("Running program"));
    return res;
#endif
}

bool k9Tools::runAsRoot(const QString &_command,const QString &_description) {
#ifndef BACKLITE
    KDESu::SuProcess suprocess("root",_command.toUtf8());
    int res=3;
    while (res==3) {
        if (suprocess.checkInstall(g_password.toUtf8())!=0) {
            KPasswordDialog pwd(k9Dialogs::getMainWidget(),KPasswordDialog::UsernameReadOnly  );
            KUser user;
            QString username=suprocess.useUsersOwnPassword()?user.loginName():QString("root");
            pwd.setUsername(username);
            pwd.setPrompt(i18n("Enter the root password"));
            pwd.addCommentLine("",_description);
            if (!pwd.exec())
                return false;
            g_password=pwd.password();
            res=suprocess.exec(g_password.toUtf8());
        } else
            res=suprocess.exec(g_password.toUtf8());
        if (res==3)
            k9Dialogs::error(i18n("The root password is not valid"),i18n("Running program"));
    }
    return res==0;
#else
    return true;
#endif
}

void k9Tools::clearOutput() {
    foreach(QString dir,m_temporaryDirs) {
        qDebug() << "clear " << dir;
        k9Tools::clearOutput(dir);
    }

}

const QString k9Tools::randomFileName() {
    static int prev=0;
    QTime t(0,0,0);
    srand(t.msecsTo(QTime::currentTime()));
    int res=rand();
    if (res==prev) res=rand();
    prev=res;
    return QString::number(res % 99999999);
}

void k9Tools::clearOutput(QString name) {
    QDir dir(name);
    //delete files in directory
    QStringList lst = dir.entryList(QStringList( "*"),QDir::Files |QDir::Hidden );
    foreach (QString c,lst) {
        //QString c(( *it ).latin1() );
        dir.remove (c);
    }
    //scanning subdir
    QStringList lstdir = dir.entryList( QStringList("*"),QDir::Dirs );
    foreach ( QString c,lstdir) {
        if ((c!=".") && c!="..") {
            clearOutput(dir.absoluteFilePath(c));
            dir.rmdir(c);
        }
    }

}

void k9Tools::setComboText(QComboBox *_cb,const QString &_text) {
    int i = _cb->findText(_text);
    if (i != -1)
        _cb->setCurrentIndex(i);
    else if (_cb->isEditable())
        _cb->setEditText(_text);
    else
        _cb->setItemText(_cb->currentIndex(), _text);
}

long k9Tools::getFreeSpace(const QString &_path) {
    struct statfs sfs;
    if (statfs(_path.toAscii(),&sfs) !=-1) {
        uint64_t fs=((uint64_t)sfs.f_bavail*(uint64_t)sfs.f_bsize);
        fs/=(1024*1024);
        return fs;
    } else
        return -1;

}

#ifdef BACKLITE
QIcon getStdIcon(const QString &name) {

    if  (name =="media-playback-stop")
            return qApp->style()->standardIcon(QStyle::SP_MediaStop);
    else if (name == "media-playback-start")
            return qApp->style()->standardIcon(QStyle::SP_MediaPlay);
    else if (name=="media-optical")
            return qApp->style()->standardIcon(QStyle::SP_DriveDVDIcon);
    else if (name=="folder-open")
            return qApp->style()->standardIcon(QStyle::SP_DirIcon);
    else if (name=="document-open")
            return qApp->style()->standardIcon(QStyle::SP_DialogOpenButton);
    else if (name=="document-open-folder")
            return qApp->style()->standardIcon(QStyle::SP_DirIcon);
    else if (name=="folder")
            return qApp->style()->standardIcon(QStyle::SP_DirClosedIcon);
    else if (name=="media-optical-recordable")
            return qApp->style()->standardIcon(QStyle::SP_DriveDVDIcon);
    else if (name=="media-optical-recordable")
            return qApp->style()->standardIcon(QStyle::SP_DriveDVDIcon);
    else if (name=="reset")
            return qApp->style()->standardIcon(QStyle::SP_DialogResetButton);
    else
            return QIcon(":/icons/"+name);

}


QIcon SmallIcon(const QString &name) {
    QIcon icon= getStdIcon(name) ;
    QIcon res(icon.pixmap(QSize(22, 22)));
    return res;
}

inline static bool isSpecial( QChar cUnicode )
{
    static const uchar iqm[] = {
    0xff, 0xff, 0xff, 0xff, 0xdd, 0x07, 0x00, 0xd8,
    0x00, 0x00, 0x00, 0x10, 0x01, 0x00, 0x00, 0x38
     }; // 0-32 \'"$`<>|;&(){}*?#

     uint c = cUnicode.unicode ();
     return (c < sizeof(iqm) * 8) && (iqm[c / 8] & (1 << (c & 7)));
}

 QString KShell::quoteArg( const QString &arg )
 {
    if (!arg.length())
        return QString::fromLatin1("''");
    for (int i = 0; i < arg.length(); i++)
         if (isSpecial( arg.unicode()[i] )) {
            QChar q( QLatin1Char('\'') );
             return QString( arg ).replace( q, QLatin1String("'\\''") ).prepend( q ).append( q );
         }
    return arg;
}

#endif
