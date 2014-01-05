#include "k9batch.h"
#include <QTextStream>
#include "k9execcopy.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include <QApplication>
#include <QTimer>


void k9batch::showHelp()
{
    k9log::writeln(QString("Usage : %1 [options]").arg(qApp->arguments().at(0)));
    k9log::writeln("\nOptions:");
    k9log::writeln("--input <device>\t\tDevice containing the DVD to copy");
    k9log::writeln("--output <directory>\t\tDestination of the backup");
    k9log::writeln("--output-size <maxsize>\t\tSize of the DVD in Mb (max 8500)");
    k9log::writeln("--backup-all\t\t\tPerforms a complete backup of the DVD");
    k9log::writeln("--backup-titles <title,title,title,...>\tPerforms a backup of the listed titles");
    k9log::writeln("--backup-main\t\t\tPerforms a backup of the main title only");
    k9log::writeln("--backup-menus\t\t\tKeep original menus");
    k9log::writeln("--prefered-audio-lang <lang,lang,lang>\tAudio streams to backup. If the first is not found, takes the second...");
    k9log::writeln("--prefered-sub-lang <lang,lang,lang>\tSubtitles streams to backup. If the first is not found, takes the second...");
}

bool k9batch::exec(int argc, char** argv)
{
    bool isBatch=false;
    for (int i=0; i < argc; i++) 
      if (strcmp(argv[i],"--batch")==0)
	isBatch=true;
    
    if (!isBatch)
      return false;
      
      
    QApplication app(argc,argv,false);
    for (int i=0 ;i < app.arguments().count();i++) {
        QString arg=app.arguments().at(i);
        QString nextArg="";
        if (i< app.arguments().count() -1)
            nextArg=app.arguments().at(i+1);
        if (arg.startsWith("--"))
            args[arg.replace("--","")]= nextArg.startsWith("--") ? "" : nextArg;
    }

    if (args.contains("help")){
      showHelp();
      return true;
    }

    k9Tools::setBatchCopy(true);
    k9DVD dvd(NULL);
    dvd.scandvd(args["input"],true);
    k9ExecCopy execCopy;
    execCopy.setDvd ( &dvd );
    QString path;
    execCopy.setCopyMenus ( args.contains("backup-menus"));
    if (args.contains("output-size")) {
      bool ok;
      int size;
      size=args["output-size"].toInt(&ok);
      if (ok)
	execCopy.setDvdSize(qMin(size,8500));
    }
    doSelect(dvd);
    //execCopy.setSpeed ( Ui_MainDlg.cbBurnSpeed->currentText() );
    bool bFolder,bISO;
    bISO=args["output"].endsWith(".iso");
    bFolder =!bISO;

    if ( bFolder) {
        execCopy.setOutput ( k9ExecCopy::oFolder );
        // changed from home to preferred output directory (bkp)
        path= args["output"];
        if ( path=="" )
            return true;
    } else if ( bISO) {
        // changed QDir::homePath() to m_prefOutput (bkp)
        path= args["output"];
        if ( path =="" )
            return true;
        execCopy.setOutput ( k9ExecCopy::oISO );
    }
    execCopy.setPath ( path );
    execCopy.copyDVD();
    return true;
}


void k9batch::doSelectLang(k9DVDTitle *title){
    k9log::writeln(QString("title %1 - VTS %2 selected").arg(title->getnumTitle()).arg(title->getVTS()));
    QStringList prefAudio=args["prefered-audio-lang"].split(",");
    bool found=false;
    for (int iAudio=0;iAudio <title->getaudioStreamCount() && !found;iAudio++) {
        foreach(QString lang,prefAudio) {
            if (title->getaudioStream(iAudio)->getlangCod()==lang) {
                title->getaudioStream(iAudio)->setselected(true);
                k9log::writeln (QString("---selected Audio %1").arg(lang));
                found=true;
                break;
            }

        }
    }

    QStringList prefSub=args["prefered-sub-lang"].split(",");
    found=false;
    for (int iSub=0;iSub <title->getsubPictureCount() && !found;iSub++) {
        foreach(QString lang,prefSub) {
            if (title->getsubtitle(iSub)->getlangCod()==lang) {
                k9log::writeln (QString("---selected Subtitle %1").arg(lang));
                title->getsubtitle(iSub)->setselected(true);
                found=true;
                break;
            }

        }
    }

    foreach (k9DVDTitle *t,title->getTitles())
        doSelectLang(t);
    if (!title->isSelected()) title->setvideoSelected(true);
}

void k9batch::doSelect(k9DVD &dvd) {
    if (args.contains("backup-all")) {
        for (int itt=0; itt <dvd.gettitleCount(); itt++) {
            k9DVDTitle *title = dvd.gettitle(itt);
            doSelectLang(title);
        }
    } else if(args.contains("backup-titles")) {
        QStringList ltitles=args["backup-titles"].split(",");
        foreach(QString stitle,ltitles) {
            for (int itt=0; itt <dvd.gettitleCount(); itt++) {
                k9DVDTitle *title = dvd.gettitle(itt);
                if (title->getnumTitle()==stitle.toInt())
                    doSelectLang(title);
            }
        }
    } else if (args.contains("backup-main")) {
        k9DVDTitle *longest=NULL;
        for (int itt=0; itt <dvd.gettitleCount(); itt++) {
            k9DVDTitle *title = dvd.gettitle(itt);
            if (longest==NULL) {
                if (title->getIndexed())
                    longest=title;
            } else {
                if (title->gettotalsize_mb() > longest->gettotalsize_mb())
                    longest=title;
            }
        }
        doSelectLang(longest);
    }
}
