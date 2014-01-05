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

#include "k9astdestination.h"
#include "k9dvd.h"
#include "k9cddrive.h"
#include <QComboBox>
#include "QDebug"
#ifndef BACKLITE
#include <KFileDialog>
#endif

k9astDestination::k9astDestination(k9Assistant* parent,k9CdDrives *_drives,k9DVD *_dvd)
        : QWidget(parent) {
    Ui_astDestination.setupUi(this);
    m_parent=parent;
    m_drives=_drives;
    m_dvd=_dvd;
    fillList();
    m_parent->setDVDdestination(dstDVD);
    Ui_astDestination.rbDVD->setChecked(false);
    Ui_astDestination.rbDVD->setChecked(true);
#ifdef BACKLITE
        Ui_astDestination.urFolder->setMode(KUrlRequester::DIRECTORY);
        Ui_astDestination.urIso->setMode(KUrlRequester::SAVE);
        Ui_astDestination.urIso->setFilter("Iso Image (*.iso)");
        Ui_astDestination.urIso->setTitle(i18n("Save iso image"));


        Ui_astDestination.urMpeg->setVisible(false);
        Ui_astDestination.urAvi->setVisible(false);
        Ui_astDestination.urAudio->setVisible(false);

        Ui_astDestination.urAvi->setFilter("Audio Video Interleave (*.avi);;MPEG-1 and MPEG-2 systems (*.mpg);;MPEG-4 format (*.mp4);;Advanced Streaming Format (*.asf);;Adobe Flash (*.swf);;Adobe Flash video files (*.flv);;RealVideo (*.rm);;QuickTime (*.mov);;Matroska (*.mkv)");
        Ui_astDestination.urAvi->setMode(KUrlRequester::SAVE) ;

        Ui_astDestination.urMpeg->setFilter("Mpeg video file (*.mpg)");
        Ui_astDestination.urMpeg->setMode(KUrlRequester::SAVE);

        Ui_astDestination.urAudio->setMode(KUrlRequester::DIRECTORY);

#else
    Ui_astDestination.urFolder->setMode(KFile::Directory | KFile::ExistingOnly);

    Ui_astDestination.urMpeg->setVisible(false);
    Ui_astDestination.urAvi->setVisible(false);
    Ui_astDestination.urAudio->setVisible(false);
    Ui_astDestination.urAvi->setFilter("*.avi|Audio Video Interleave (*.avi)\n*.mpg|MPEG-1 and MPEG-2 systems (*.mpg)\n*.mp4|MPEG-4 format (*.mp4)\n*.asf|Advanced Streaming Format (*.asf)\n*.swf|Adobe Flash (*.swf)\n*.flv|Adobe Flash video files (*.flv)\n*.rm|RealVideo (*.rm)\n*.mov|QuickTime (*.mov)\n*.mkv|Matroska (*.mkv)");
    Ui_astDestination.urAvi->setMode(KFile::File) ;
    Ui_astDestination.urAvi->fileDialog()->setOperationMode(KFileDialog::Saving);
    Ui_astDestination.urIso->setFilter("*.iso|Iso Image (*.iso)");
    Ui_astDestination.urIso->setMode(KFile::File) ;
    Ui_astDestination.urIso->fileDialog()->setOperationMode(KFileDialog::Saving);
    Ui_astDestination.urMpeg->setFilter("*.mpg|Mpeg video file (*.mpg)");
    Ui_astDestination.urMpeg->setMode(KFile::File);
    Ui_astDestination.urMpeg->fileDialog()->setOperationMode(KFileDialog::Saving);
    Ui_astDestination.urAudio->setMode(KFile::Directory | KFile::ExistingOnly);
#endif

    updategbDVD();
}


void k9astDestination::fillList() {
    connect(m_drives,SIGNAL(deviceAdded( k9CdDrive*)),this,SLOT(deviceAdded( k9CdDrive* )));
    connect(m_drives,SIGNAL(deviceRemoved( k9CdDrive*)),this,SLOT(deviceRemoved( k9CdDrive* )));

}


void k9astDestination::updatePaths() {
  k9Config config;
   QString title=m_parent->getDvd()->getDVDTitle();
   title= title.isEmpty() ? i18n("unknown"):title;
#ifdef BACKLITE
   Ui_astDestination.urFolder->setUrl(KUrl(QDir::cleanPath(config.getPrefOutput())));
   Ui_astDestination.urIso->setUrl(KUrl(QDir::cleanPath(QDir::currentPath() +"/"+title+".iso") ));
#else
   Ui_astDestination.urFolder->setUrl(KUrl(QDir::cleanPath(config.getPrefOutput())));
   Ui_astDestination.urIso->setUrl(KUrl(QDir::cleanPath(config.getPrefOutputIso()+"/"+title+".iso") ));
   Ui_astDestination.urAudio->setUrl(KUrl(QDir::cleanPath(config.getPrefOutputAudio())));
   Ui_astDestination.urAvi->setUrl(KUrl(QDir::cleanPath(config.getPrefOutputMpeg4()+"/"+title+".avi") ));
   Ui_astDestination.urMpeg->setUrl(KUrl(QDir::cleanPath(config.getPrefOutputMpeg2()+"/"+title+".mpg") ));  
#endif
}

void k9astDestination::deviceAdded(k9CdDrive *_drive) {

    if (_drive->canWriteDVD) {
         Ui_astDestination.cbDrives->addItem(QString("%1 (%2)").arg(_drive->name).arg(_drive->device),_drive->device );

    }
}


void k9astDestination::rbDVDToggled(bool _state) {
   Ui_astDestination.gbDVD->setVisible(_state);
   if (_state) {
    Ui_astDestination.rbAvi->setChecked(false);
    Ui_astDestination.rbMpeg->setChecked(false);
    Ui_astDestination.rbAudio->setChecked(false);
   }
   m_parent->setDestination(dstDVD);
}

void k9astDestination::rbAviToggled(bool _state) {
   Ui_astDestination.urAvi->setVisible(_state);
   if (_state) {
   	Ui_astDestination.rbDVD->setChecked(false);
        Ui_astDestination.rbMpeg->setChecked(false);
        Ui_astDestination.rbAudio->setChecked(false);
   }
   m_parent->setDestination(dstAvi);
}

void k9astDestination::rbMpegToggled(bool _state) {
   Ui_astDestination.urMpeg->setVisible(_state);
   if (_state) {
      Ui_astDestination.rbDVD->setChecked(false);
      Ui_astDestination.rbAvi->setChecked(false);
      Ui_astDestination.rbAudio->setChecked(false);
   }
   m_parent->setDestination(dstMpeg);
}

void k9astDestination::rbAudioToggled(bool _state) {
   Ui_astDestination.urAudio->setVisible(_state);
   if (_state) {
      Ui_astDestination.rbDVD->setChecked(false);
      Ui_astDestination.rbAvi->setChecked(false);
      Ui_astDestination.rbMpeg->setChecked(false);
   }
   m_parent->setDestination(dstAudio);
}

void k9astDestination::updategbDVD() {
    Ui_astDestination.cbDrives->setVisible(Ui_astDestination.rbDVDDisc->isChecked());
    Ui_astDestination.urFolder->setVisible(Ui_astDestination.rbDVDFolder->isChecked());
    Ui_astDestination.urIso->setVisible(Ui_astDestination.rbDVDIso->isChecked());

}

void k9astDestination::rbDVDDiscToggled(bool _state) {
   updategbDVD();
   if (_state) {
       m_parent->setDVDdestination(dstDVD);
   }

}
void k9astDestination::rbDVDFolderToggled(bool _state) {
   updategbDVD();
   if (_state) {
       m_parent->setDVDdestination(dstFolder);
   }

}
void k9astDestination::rbDVDIsoToggled(bool _state) {
   updategbDVD();
   if (_state) {
       m_parent->setDVDdestination(dstIso);
   }
}


const QString & k9astDestination::getPath() {
   m_path="";
   if (Ui_astDestination.rbDVD->isChecked()) {
      if (Ui_astDestination.rbDVDIso->isChecked())
#ifdef BACKLITE
        m_path=Ui_astDestination.urIso->url().toLocalFile() ;
#else
        m_path=Ui_astDestination.urIso->url().toLocalFile(KUrl::RemoveTrailingSlash) ;
#endif
      if (Ui_astDestination.rbDVDFolder->isChecked())
	m_path=Ui_astDestination.urFolder->url().path();
      if (Ui_astDestination.rbDVDDisc->isChecked())
	m_path=Ui_astDestination.cbDrives->itemData(Ui_astDestination.cbDrives->currentIndex()).toString();
   }else if (Ui_astDestination.rbMpeg->isChecked())
	m_path=Ui_astDestination.urMpeg->url().path();
   else if (Ui_astDestination.rbAvi->isChecked())
	m_path=Ui_astDestination.urAvi->url().path();
   else if (Ui_astDestination.rbAudio->isChecked())
	m_path=Ui_astDestination.urAudio->url().path();


   return m_path;
}

bool k9astDestination::isOk() {
  return getPath() !="";
}


void k9astDestination::deviceRemoved(k9CdDrive *) {

}


bool k9astDestination::isMatroska() {
   return getPath().endsWith(".mkv");
}

k9astDestination::~k9astDestination() {
}


/*$SPECIALIZATION$*/

