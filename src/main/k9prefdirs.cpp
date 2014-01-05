//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9prefdirs.h"

#include <kurlrequester.h>
#include "k9tools.h"
#include "k9dvd.h"
k9prefDirs::k9prefDirs(QWidget* parent)
: QWidget(parent)
{
    Ui_prefDirs.setupUi(this);
    k9Config config;
#ifdef BACKLITE
 //   Ui_prefDirs.urDirDVD->setMode(KUrlRequester::DIRECTORY);
    Ui_prefDirs.urDirISO->setMode(KUrlRequester::DIRECTORY);
    Ui_prefDirs.urDirMPEG4->setMode(KUrlRequester::DIRECTORY);
    Ui_prefDirs.urDirMPEG2->setMode(KUrlRequester::DIRECTORY);
    Ui_prefDirs.urDirAudio->setMode(KUrlRequester::DIRECTORY);
    Ui_prefDirs.urDvdauthor->setMode(KUrlRequester::OPEN);
    Ui_prefDirs.urFFmpeg->setMode(KUrlRequester::OPEN);
    Ui_prefDirs.urGenisoimage->setMode(KUrlRequester::OPEN);
    Ui_prefDirs.urMencoder->setMode(KUrlRequester::OPEN);
    Ui_prefDirs.urMplayer->setMode(KUrlRequester::OPEN);
    Ui_prefDirs.urWodim->setMode(KUrlRequester::OPEN);
    Ui_prefDirs.urK3b->setMode(KUrlRequester::OPEN);
#else
   // Ui_prefDirs.urDirDVD->setMode(KFile::Directory);
    Ui_prefDirs.urDirISO->setMode(KFile::Directory);
    Ui_prefDirs.urDirMPEG4->setMode(KFile::Directory);
    Ui_prefDirs.urDirMPEG2->setMode(KFile::Directory);
    Ui_prefDirs.urDirAudio->setMode(KFile::Directory);
    Ui_prefDirs.urDvdauthor->setMode(KFile::ExistingOnly | KFile::File);
    Ui_prefDirs.urFFmpeg->setMode(KFile::ExistingOnly | KFile::File);
    Ui_prefDirs.urGenisoimage->setMode(KFile::ExistingOnly | KFile::File);
    Ui_prefDirs.urMencoder->setMode(KFile::ExistingOnly | KFile::File);
    Ui_prefDirs.urMplayer->setMode(KFile::ExistingOnly | KFile::File);
    Ui_prefDirs.urWodim->setMode(KFile::ExistingOnly | KFile::File);
    Ui_prefDirs.urK3b->setMode(KFile::ExistingOnly | KFile::File);
#endif
    //Ui_prefDirs.urDirDVD->setUrl(KUrl(config.getPrefOutput()));
    Ui_prefDirs.urDirISO->setUrl(KUrl(config.getPrefOutputIso()));
    Ui_prefDirs.urDirMPEG4->setUrl(KUrl(config.getPrefOutputMpeg4()));
    Ui_prefDirs.urDirMPEG2->setUrl(KUrl(config.getPrefOutputMpeg2()));
    Ui_prefDirs.urDirAudio->setUrl(KUrl(config.getPrefOutputAudio()));
    Ui_prefDirs.urDvdauthor->setUrl(KUrl(config.getPrefDvdauthorPath(false)));
    Ui_prefDirs.urFFmpeg->setUrl(KUrl(config.getPrefFFmpegPath(false)));
    Ui_prefDirs.urGenisoimage->setUrl(KUrl(config.getPrefGenisoimagePath(false)));
    Ui_prefDirs.urMencoder->setUrl(KUrl(config.getPrefMencoderPath(false)));
    Ui_prefDirs.urMplayer->setUrl(KUrl(config.getPrefMplayerPath(false)));
    Ui_prefDirs.urWodim->setUrl(KUrl(config.getPrefWodimPath(false)));
    Ui_prefDirs.urK3b->setUrl(KUrl(config.getPrefk3bPath(false)));
    
}

k9prefDirs::~k9prefDirs()
{
}

/*$SPECIALIZATION$*/

void k9prefDirs::save() {
    k9Config config;
   // config.setPrefOutput( Ui_prefDirs.urDirDVD->url().path());

    config.setPrefOutputAudio ( Ui_prefDirs.urDirAudio->url().path());
    config.setPrefOutputIso( Ui_prefDirs.urDirISO->url().path());
    config.setPrefOutputMpeg2( Ui_prefDirs.urDirMPEG2->url().path());
    config.setPrefOutputMpeg4( Ui_prefDirs.urDirMPEG4->url().path());

    config.setPrefDvdauthorPath( Ui_prefDirs.urDvdauthor->url().path());
    config.setPrefFFmpegPath( Ui_prefDirs.urFFmpeg->url().path());
    config.setPrefGenisoimagePath( Ui_prefDirs.urGenisoimage->url().path());
    config.setPrefMencoderPath( Ui_prefDirs.urMencoder->url().path());
    config.setPrefMplayerPath( Ui_prefDirs.urMplayer->url().path());
    config.setPrefWodimPath( Ui_prefDirs.urWodim->url().path());
    config.setPrefk3bPath( Ui_prefDirs.urK3b->url().path());
   
    config.save();
}

