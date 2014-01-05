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

#include "k9astsource.h"
#include "k9dvd.h"
#include "k9cddrive.h"
#include <QListWidget>
#include "k9dialogs.h"
#include "k9dvdtitle.h"

k9astSource::k9astSource(k9Assistant* parent,k9CdDrives *_drives,k9DVD *_dvd)
        : QWidget(parent) {
    Ui_astSource.setupUi(this);
    m_parent=parent;
    m_drives=_drives;
    m_dvd=_dvd;
#ifdef BACKLITE
    Ui_astSource.rbDrive->setChecked(true);
    Ui_astSource.urFolder->setVisible(false);
    Ui_astSource.urFolder->setMode(KUrlRequester::DIRECTORY);
    Ui_astSource.urIso->setVisible(false);
    Ui_astSource.urIso->setMode(KUrlRequester::OPEN);
    Ui_astSource.urIso->setFilter("Iso Image (*.iso)");
    Ui_astSource.urIso->setTitle(i18n("Open ISO image"));
#else
    Ui_astSource.urFolder->setMode(KFile::Directory | KFile::ExistingOnly);
    Ui_astSource.rbDrive->setChecked(true);
    Ui_astSource.urFolder->setVisible(false);
    Ui_astSource.urIso->setVisible(false);
    Ui_astSource.urIso->setFilter("*.iso|Iso Image (*.iso)");
#endif
    fillList();
}


void k9astSource::fillList() {
    connect(m_drives,SIGNAL(deviceAdded( k9CdDrive*)),this,SLOT(deviceAdded( k9CdDrive* )));
    connect(m_drives,SIGNAL(deviceRemoved( k9CdDrive*)),this,SLOT(deviceRemoved( k9CdDrive* )));

}

void k9astSource::setPath(const QString &_path) {
   for (int i=0;i<m_drives->count();i++) {
       k9CdDrive *drive=m_drives->getDrive(i);
       if (drive->device==_path) {
	    Ui_astSource.cbDrives->setCurrentIndex(i);
	    Ui_astSource.rbDrive->setChecked(true);
	    return;
	}
	
   }
   if (QFile::exists(_path)) {
        QFileInfo inf(_path);
	if (!inf.isFile()) {
            Ui_astSource.rbFolder->setChecked(true);
            Ui_astSource.urFolder->setUrl(KUrl(_path));
        }else {
            Ui_astSource.rbIso->setChecked(true);
            Ui_astSource.urIso->setUrl(KUrl(_path));
	}
   }

}

void k9astSource::deviceAdded(k9CdDrive *_drive) {

 if (_drive->canReadDVD) {
         Ui_astSource.cbDrives->addItem(QString("%1 (%2)").arg(_drive->name).arg(_drive->device),_drive->device );
    }
}

void k9astSource::deviceRemoved(k9CdDrive *) {

}

void k9astSource::rbDriveToggled(bool _state) {
   Ui_astSource.cbDrives->setVisible(_state);
}
void k9astSource::rbFolderToggled(bool _state) {
   Ui_astSource.urFolder->setVisible(_state);
}
void k9astSource::rbIsoToggled(bool _state) {
   Ui_astSource.urIso->setVisible(_state);
}


k9astSource::~k9astSource() {
}


bool k9astSource::openDvd() {

    QString path;
    if (Ui_astSource.rbDrive->isChecked())
	path=Ui_astSource.cbDrives->itemData(Ui_astSource.cbDrives->currentIndex()).toString();
    else if (Ui_astSource.rbFolder->isChecked())
	path=Ui_astSource.urFolder->url().path();
    else if (Ui_astSource.rbIso->isChecked())
	path=Ui_astSource.urIso->url().path();

    k9Config config;
#ifndef BACKLITE
    k9CdDrive *drive=k9CdDrives::getDeviceFromMountpoint(m_drives,path);
    if (drive) 
	path=drive->device;
    if (path.startsWith("/dev/") && config.getPrefReadAhead()!=-1)
	k9CdDrive::setReadAhead(path,config.getPrefReadAhead());
#endif
    if (m_dvd->getopened())
        for (int i=0; i <m_dvd->gettitleCount();i++)
                m_dvd->gettitle(i)->setSelected(false);


   
    m_dvd->scandvd(path,true);
    return !m_dvd->geterror ();
}
/*$SPECIALIZATION$*/

