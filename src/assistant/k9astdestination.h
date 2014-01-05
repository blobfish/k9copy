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

#ifndef K9ASTDESTINATION_H
#define K9ASTDESTINATION_H
#include "k9common.h"
#include "ui_astdestination.h"

#include "k9assistant.h"


class k9CdDrives;
class k9CdDrive;
class k9DVD;

class k9astDestination : public QWidget
{
  Q_OBJECT

public:
  k9astDestination(k9Assistant* parent,k9CdDrives *_drives,k9DVD *_dvd );
  ~k9astDestination();
   const QString &getPath();
   bool isOk();
   bool isMatroska();
  /*$PUBLIC_FUNCTIONS$*/
public slots:
  /*$PUBLIC_SLOTS$*/
  void updatePaths();
protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
  void deviceAdded(k9CdDrive *_drive);
  void deviceRemoved(k9CdDrive *_drive);
  void rbDVDToggled(bool);
  void rbAviToggled(bool);
  void rbMpegToggled(bool);
  void rbAudioToggled(bool);

  void rbDVDDiscToggled(bool);
  void rbDVDFolderToggled(bool);
  void rbDVDIsoToggled(bool);

private:
  Ui::astDestination Ui_astDestination;
  k9Assistant *m_parent;

  k9CdDrives *m_drives;
  k9DVD *m_dvd;

  void fillList();
  void updategbDVD();
  QString m_path;
};

#endif

