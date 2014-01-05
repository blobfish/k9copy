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

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>
#include "config.h"
#include "k9xineplayer.h"
#include <QDBusAbstractAdaptor>
#include <QDBusVariant>
#include <QDBusConnection>
#include <QDBusError>
#include <QTimer>
#include <QApplication>

static const char description[] =
    I18N_NOOP("A KDE 4 Application");

static const char version[] = VERSION ;

int main(int argc, char **argv)
{
    KAboutData about("k9xineplayer", 0, ki18n("k9copy"), version, ki18n(description),
                     KAboutData::License_GPL, ki18n("(C) 2007 Jean-Michel"), KLocalizedString(), 0, "k9copy@free.fr");
    about.addAuthor( ki18n("Jean-Michel"), KLocalizedString(), "k9copy@free.fr" );
    about.setTranslator(ki18n("_: NAME OF TRANSLATORS\\nYour names")
                        ,ki18n("_: EMAIL OF TRANSLATORS\\nYour emails"));

    QApplication  app(argc, argv,false);

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;
    options.add( "wid <window id>", ki18n("wid"));
    options.add( "vo <video output>", ki18n("vo"));
    options.add( "ao <audio output>", ki18n("ao"));

    KCmdLineArgs::addCmdLineOptions( options );

    KCmdLineArgs::addCmdLineOptions(options);
 
        // no session.. just start up normally
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    QString wid(args->getOption("wid"));
    QString vo(args->getOption("vo"));  
    QString ao(args->getOption("ao"));

    QObject obj;
    k9xinePlayer player(&obj);
    player.setAo(ao);
    player.setVo(vo);
    player.init(wid.toLong());

    if (!QDBusConnection::sessionBus().registerService("k9copy.free.fr.xineplayer")) {
        fprintf(stderr, "%s\n",
                qPrintable(QDBusConnection::sessionBus().lastError().message()));        
        exit(1);
    }
    QDBusConnection::sessionBus().registerObject("/", &obj);

    return app.exec();
}
