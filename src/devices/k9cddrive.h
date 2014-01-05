//
// C++ Interface: k9cddrive
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9CDDRIVE_H
#define K9CDDRIVE_H
#include "k9common.h"
#include "config.h"
#include <qobject.h>

#include <qstring.h>
#include <qstringlist.h>
#include <QList>

#include <solid/device.h>
#include <solid/devicenotifier.h>

/**
  *@author
  */
class k9HalDevice;
class k9HalConnection;
class k9CdDrives;

class k9CdDrive : public QObject {
    Q_OBJECT
friend class k9CdDrives;
public:
    k9CdDrive();
    ~k9CdDrive();
    int num;
    QString name;
    QString device;
    bool canWriteCDR;
    bool canReadDVD;
    bool canWriteDVD;

    static void setReadAhead(const QString &_device,int _value);
    void setWriteSpeeds(const QList< int >& _value);
    QList< int > getWriteSpeeds() const;

    void setDevice(Solid::Device  _value);
    Solid::Device getDevice()  { return m_Device;}
public slots:
     void ejectPressed (const QString &udi);
private:
   QList <int> writeSpeeds;
   Solid::Device m_Device;
signals:
   void volumeChanged(const QString &device, const QString &volumeName);
};

class k9CdDrives : public QObject  {
    Q_OBJECT
public:
    k9CdDrives(QObject *parent);
    ~k9CdDrives();
    /** No descriptions */
    int count();
    /** No descriptions */
    k9CdDrive * getDrive(int num);
    static void eject(const QString & device);
    void scanDrives();
    void clearDrives();
    static k9CdDrive *getDeviceFromMountpoint(k9CdDrives *drives,const QString &mountpoint);
public slots:
   void	deviceAdded (const QString &udi);
   void deviceRemoved (const QString &udi);
signals:
    void deviceAdded(k9CdDrive *drive);
    void deviceRemoved(k9CdDrive *drive);
private: // Private methods
    void readConfig();
    QList <k9CdDrive*> drives;
};
#endif
