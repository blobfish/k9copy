//
// C++ Interface: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9ASTSOURCE_H
#define K9ASTSOURCE_H

#include "k9common.h"
#include "ui_astsource.h"
#include "k9assistant.h"


class k9CdDrives;
class k9CdDrive;
class k9DVD;

class k9astSource : public QWidget
{
  Q_OBJECT

public:
  k9astSource(k9Assistant* parent,k9CdDrives *_drives,k9DVD *_dvd );
  ~k9astSource();
  void setPath(const QString &_path);
  bool openDvd();
  /*$PUBLIC_FUNCTIONS$*/
public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  void deviceAdded(k9CdDrive *_drive);
  void deviceRemoved(k9CdDrive *_drive);
  void rbDriveToggled(bool);
  void rbFolderToggled(bool);
  void rbIsoToggled(bool);
private:
  Ui::astSource Ui_astSource;
  k9Assistant *m_parent;

  k9CdDrives *m_drives;
  k9DVD *m_dvd;

  void fillList();
};

#endif

