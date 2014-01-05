/***************************************************************************
 *   Copyright (C) 2007 by Jean-Michel   *
 *   k9copy@free.fr             *
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

#include "src/dvdnav/common.h"
#include "k9copy.h"
#include "k9play.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>

#include <QDBusAbstractAdaptor>
#include <QDBusVariant>
#include <QDBusConnection>
#include <QDBusError>
#include <QTimer>
static const char description[] =
    I18N_NOOP("A KDE 4 Application");

static const char version[] = VERSION ;

int main(int argc, char **argv) {
    KAboutData about("k9copy", 0, ki18n("k9copy"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2007 Jean-Michel"), KLocalizedString(), 0, "k9copy@free.fr");
    about.addAuthor( ki18n("Jean-Michel"), KLocalizedString(), "k9copy@free.fr" );
    about.setTranslator(ki18n("_: NAME OF TRANSLATORS\\nYour names")
                        ,ki18n("_: EMAIL OF TRANSLATORS\\nYour emails"));

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;

    options.add("gui",ki18n("no gui"));
    options.add( "input <device>", ki18n("input device"));
    options.add("output <device>", ki18n("output device"));
    options.add("dvdtitle <number>", ki18n("title to play"));
    options.add("play", ki18n("play title to stdout"));
    options.add("startsector <number>", ki18n("start sector"));
    options.add("endsector <number>", ki18n("end sector"));
    options.add("audiofilter <number,number>", ki18n("list of audio streams"));
    options.add("subpicturefilter <number,number>", ki18n("list of spu streams"));
    options.add("vampsfactor <number>", ki18n("shrink factor"));
    options.add("ffactor", ki18n("shrink factor forced"));
    options.add("inputsize <number>", ki18n("size of the cell to be copied"));
    options.add("totalsize <number>", ki18n("total size of selected titles"));
    options.add("chaptersize <number>", ki18n("size of the chapters being copied"));
    options.add("chapterlist <list>",ki18n("selected chapters"));
    options.add( "dvdsize <number>", ki18n("new dvd size"));
    options.add("chapter <number>", ki18n("selected chapter"));
    options.add("cell <number>", ki18n("cell number in selected chapter"));
    options.add("inject <filename>", ki18n("status file name"));
    options.add("initstatus" ,ki18n("initialize status file"));
    options.add("continue" ,ki18n("continue playing from last sector"));
    options.add("firstpass" ,ki18n("don't save status at end"));
    options.add( "usecache" ,ki18n("save cell in a temporary file before encoding"));

    KCmdLineArgs::addCmdLineOptions( options );
    // no session.. just start up normally
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    QApplication app(argc, argv,false);
    QString TitleNumber(args->getOption("dvdtitle"));
    QString InputOptionArg( args->getOption("input"));
    QString OutputOptionArg( args->getOption("output"));
    QString startSectorArg(args->getOption("startsector"));
    QString endSectorArg(args->getOption("endsector"));
    QString audioFilterArg(args->getOption("audiofilter"));
    QString subpictureFilterArg(args->getOption("subpicturefilter"));
    QString vampsFactorArg(args->getOption("vampsfactor"));
    QString inputSizeArg(args->getOption("inputsize"));
    QString chapterArg(args->getOption("chapter"));
    QString cellArg(args->getOption("cell"));
    QString injectArg(args->getOption("inject"));
    QString totalSizeArg(args->getOption("totalsize"));
    QString dvdSizeArg(args->getOption("dvdsize"));
    QString chapterSizeArg(args->getOption("chaptersize"));
    QString chapterListArg(args->getOption("chapterlist"));

    bool gui=args->isSet("gui");
    k9Tools::setBatchCopy(!gui);
    bool ffactor=args->isSet("ffactor");
    bool play= args->isSet("play");
    if (play) {
        QObject obj;
        k9play player(&obj);
        player.setinitStatus( args->isSet("initstatus"));
        player.setcontinue( args->isSet("continue"));
        player.setfirstPass(args->isSet("firstpass"));
        player.setuseCache(args->isSet("usecache"));
        player.setDevice(InputOptionArg);
        player.setTitle(TitleNumber.toInt());
        player.setstartSector(startSectorArg);
        player.setendSector(endSectorArg );
        player.setaudioFilter( audioFilterArg);
        player.setsubpictureFilter( subpictureFilterArg);
        player.setvampsFactor( vampsFactorArg);
        player.setinputSize( inputSizeArg);
        player.setchapter(chapterArg);
        player.setcell(cellArg);
        player.setinject(injectArg);
        player.settotalSize(totalSizeArg);
        player.setdvdSize(dvdSizeArg);
        player.setchapterSize( chapterSizeArg);
        player.setforcedFactor(ffactor);
        player.setchapterList( chapterListArg);
	player.setexitOnEnd(true);
        player.setMainThread(true);
        QDBusConnection::sessionBus().registerObject("/", &obj);
        if (!QDBusConnection::sessionBus().registerService("k9copy.free.fr.player")) {
            fprintf(stderr, "%s\n",
                    qPrintable(QDBusConnection::sessionBus().lastError().message()));
            //  exit(1);
        }

        //QTimer::singleShot (100, &player, SLOT (execute ()));
        player.execute();
        return app.exec();

        //return 0;
    }


    return app.exec();
}
