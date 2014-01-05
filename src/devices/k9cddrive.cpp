//
// C++ Implementation: k9cddrive
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9common.h"
#include "k9cddrive.h"
#include "k9config.h"
#include "k9tools.h"
#include "config.h"
#include "k9process.h"
#include <solid/deviceinterface.h>
#include <solid/opticaldrive.h>
#include <solid/opticaldisc.h>
#include <solid/storageaccess.h>
#include <solid/block.h>
#include <QDir>

#include <solid/device.h>
#include <KLocale>

k9CdDrive::k9CdDrive() { 
    canReadDVD=false;
    canWriteCDR=false;
    canWriteDVD=false;
    device="";
    name="";
}
k9CdDrive::~k9CdDrive() {}

void k9CdDrive::ejectPressed (const QString &udi) {
   Q_UNUSED(udi);
   Solid::Block *block=m_Device.as<Solid::Block>();
   emit volumeChanged(block->device(), "");
}
void k9CdDrive::setDevice(Solid::Device  _value) { 
   m_Device = _value;
   Solid::OpticalDrive *drive = m_Device.as<Solid::OpticalDrive>();
   connect(drive,SIGNAL(ejectPressed (const QString &)),this,SLOT(ejectPressed(const QString&)));
}


void k9CdDrives::clearDrives() {
    while (!drives.isEmpty()) delete drives.takeFirst();
}

k9CdDrives::k9CdDrives(QObject *parent):QObject( parent) {
//    drives.setAutoDelete(true);
    connect(Solid::DeviceNotifier::instance(),SIGNAL(deviceAdded(const QString&)),this,SLOT(deviceAdded(const QString&)));
    connect(Solid::DeviceNotifier::instance(),SIGNAL(deviceRemoved(const QString&)),this,SLOT(deviceRemoved(const QString&)));
    scanDrives();

}
k9CdDrives::~k9CdDrives() {
    clearDrives();
}


void k9CdDrives::deviceAdded( const QString &udi) {
//   addDrive( _device);
    foreach (Solid::Device device, Solid::Device::allDevices())
     {
 	if (device.udi() == udi && device.is<Solid::OpticalDisc>()) {
           Solid::Block *block=device.as<Solid::Block>();
	   Solid::StorageVolume *volume=device.as<Solid::StorageVolume>();
   	  foreach  (k9CdDrive *d,drives) {
      	     if (d->device==block->device()) 
      		 emit d->volumeChanged(block->device(),volume->label() );
 	  }

	}
        if (device.udi() == udi &&  device.is<Solid::OpticalDrive>()) {
		Solid::OpticalDrive *sdrive = device.as<Solid::OpticalDrive>();
		Solid::Block *block=device.as<Solid::Block>();
	
		k9CdDrive *drive=new k9CdDrive;
		drive->setDevice( device);
		drive->device=block->device();
		drive->name=device.product();
		drive->canReadDVD= (sdrive->supportedMedia() & Solid::OpticalDrive::Dvd) == Solid::OpticalDrive::Dvd;
		drive->canWriteCDR=(sdrive->supportedMedia() & Solid::OpticalDrive::Cdr) == Solid::OpticalDrive::Cdr;
		drive->canWriteDVD=( (sdrive->supportedMedia() & Solid::OpticalDrive::Dvdr) == Solid::OpticalDrive::Dvdr ) | ((sdrive->supportedMedia() & Solid::OpticalDrive::Dvdplusr) == Solid::OpticalDrive::Dvdplusr);
		drive->setWriteSpeeds(sdrive->writeSpeeds());
		
		drive->num=drives.count();
		drives.append(drive);
		emit deviceAdded(drive);      
	}
      }

}

void k9CdDrives::deviceRemoved(const QString &udi) {
   foreach (k9CdDrive *d,drives) {
      if (d->getDevice().udi()==udi) {
      	  emit deviceRemoved( d);
          drives.removeAll(d);
          break;
      }
   }
}

/** No descriptions */
void k9CdDrives::scanDrives() {
    clearDrives();

 /*   foreach (Solid::Device device, Solid::Device::listFromType(Solid::DeviceInterface::OpticalDrive, QString()))
     {
        Solid::OpticalDrive *drive = device.as<Solid::OpticalDrive>();
        Solid::Block *block=device.as<Solid::Block>();
    }
*/
    int row=0;
    foreach (Solid::Device device, Solid::Device::listFromType(Solid::DeviceInterface::OpticalDrive, QString()))
     {
        Solid::OpticalDrive *sdrive = device.as<Solid::OpticalDrive>();
        Solid::Block *block=device.as<Solid::Block>();
         k9CdDrive *drive=new k9CdDrive;
	 drive->setDevice( device);
         drive->device=block->device();
         drive->name=device.product();
         drive->canReadDVD= (sdrive->supportedMedia() & Solid::OpticalDrive::Dvd) == Solid::OpticalDrive::Dvd;
         drive->canWriteCDR=(sdrive->supportedMedia() & Solid::OpticalDrive::Cdr) == Solid::OpticalDrive::Cdr;
         drive->canWriteDVD=( (sdrive->supportedMedia() & Solid::OpticalDrive::Dvdr) == Solid::OpticalDrive::Dvdr ) | ((sdrive->supportedMedia() & Solid::OpticalDrive::Dvdplusr) == Solid::OpticalDrive::Dvdplusr);
         drive->setWriteSpeeds(sdrive->writeSpeeds());
        drive->num=row;
        drives.append(drive);
        row++;
        emit deviceAdded(drive);      
      }
    readConfig();
}

void k9CdDrives::eject(const QString & device) {
    k9Process *process =new k9Process();
    if (k9Tools::checkProgram("kdeeject"))
        *process <<"kdeeject" << device;
    else    
        *process <<"eject" << device;
    process->start();
    process->wait();
    delete process;
}


/** reads devices that was entered manually **/
void k9CdDrives::readConfig() {
    QStringList ldev;
    QStringList llabels;
    QStringList lIO;
    k9Config config;
    ldev=config.getDevices();
    llabels=config.getDevicesLabels();
    lIO=config.getDevicesIO();
    int row=count();
    int i=0;
    foreach ( QString dev,ldev) {
        k9CdDrive *drive=new k9CdDrive;
        drive->device=dev;
        QString it2=llabels.at(i);
        QString it3=lIO.at(i);
        drive->name=(it2);
        QString c=(it3);
        if (c.contains("I")) {
            drive->canReadDVD=true;
        }
        if (c.contains("O")) {
            drive->canWriteCDR=true;
            drive->canWriteDVD=true;
        }
        drive->num=row;
        drives.append(drive);
        row++;
        i++;
        emit deviceAdded(drive);
    }
}



/** No descriptions */
int k9CdDrives::count() {
    return drives.count();
}

/** No descriptions */
k9CdDrive * k9CdDrives::getDrive(int num) {
    return (k9CdDrive *)drives.at(num);
}


QList< int > k9CdDrive::getWriteSpeeds() const {
    return writeSpeeds;
}


void k9CdDrive::setWriteSpeeds(const QList< int >& _value) {
    writeSpeeds = _value;
}

void k9CdDrive::setReadAhead(const QString &_device,int _value) {

    QString hdparm="hdparm" ;//KStandardDirs::findExe( "hdparm",NULL,false);

    if (hdparm !=NULL) {
       QString cmd=QString("%1 -a%2 %3").arg(hdparm).arg(_value).arg(_device);
       qDebug() << k9Tools::runAsRoot(cmd, i18n("The root password is needed to change the read ahead property of the dvd drive"));
    }
}

k9CdDrive *k9CdDrives::getDeviceFromMountpoint(k9CdDrives *drives,const QString &mountpoint) {
    foreach (Solid::Device device, Solid::Device::allDevices())
     {
 	if ( device.is<Solid::OpticalDisc>()) {
//           Solid::Block *block=device.as<Solid::Block>();
//	   Solid::StorageVolume *volume=device.as<Solid::StorageVolume>();
           Solid::StorageAccess *access=device.as<Solid::StorageAccess>();

   	  foreach  (k9CdDrive *d,drives->drives) {
                if (access)
                    if (QDir::cleanPath( access->filePath() +"/")== QDir::cleanPath(mountpoint+"/"))
			return d;
 	  }
	}
     }
     return NULL;
}
