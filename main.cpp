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
#include "k9common.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include "k9assistant.h"
#include <KDE/KLocale>
#include "k9dialogs.h"

#include <QDBusAbstractAdaptor>
#include <QDBusVariant>
#include <QDBusConnection>
#include <QDBusError>
#include <QTimer>
#include "k9batch.h"

static const char description[] =
        I18N_NOOP("A DVD Backup software for KDE");

static const char version[] = VERSION ;

int main(int argc, char **argv) {
    k9batch batch;
    if (batch.exec(argc,argv)) 
      return 1;
    
    KAboutData about("k9copy", 0, ki18n("k9copy"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2004-2011 Jean-Michel PETIT"), KLocalizedString(), 0, "k9copy@free.fr");
    about.addAuthor( ki18n("Jean-Michel PETIT"), KLocalizedString(), "k9copy@free.fr" );
    about.setTranslator(ki18n("_: NAME OF TRANSLATORS\\nYour names")
                        ,ki18n("_: EMAIL OF TRANSLATORS\\nYour emails"));

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;
    options.add( "input <device>", ki18n("input device"));
    options.add("output <device>", ki18n("output device"));
    options.add("dvdtitle <number>", ki18n("title to play"));
    options.add("assistant", ki18n("the k9copy backup assistant"));


    KCmdLineArgs::addCmdLineOptions( options );

    //    KCmdLineOptions options;
    //    options.add("+[URL]", ki18n( "Document to open" ));
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    k9Tools::setMainThread();

    if (app.isSessionRestored()) {
        RESTORE(k9Copy);
    } else {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        QString InputOptionArg( args->getOption("input"));
        QString OutputOptionArg( args->getOption("output"));
        k9Config::checkCodecs();
        if (!args->isSet("assistant")) {
            k9Copy  *widget = new k9Copy;
            if (InputOptionArg !="") {
		widget->setInput( InputOptionArg);
		widget->fileOpen();
            }
            if (OutputOptionArg !="")
		widget->setOutput( OutputOptionArg);

            if ((InputOptionArg !="") && (OutputOptionArg!=""))
		widget->clone( InputOptionArg,OutputOptionArg);
            widget->show();
        } else {
            k9Assistant *ast=k9Assistant::createAssistant();
            if (InputOptionArg !="")
                ast->setPath(InputOptionArg);
            k9Dialogs::setMainWidget(ast);
            QTimer::singleShot (10, ast, SLOT (run ()));

	}	

        int res=app.exec();
        k9Config config;
        if (config.getPrefDelTmpFiles())
            k9Tools::clearOutput();

        return res;
    }
}
