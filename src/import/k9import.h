//
// C++ Interface:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9IMPORT_H
#define K9IMPORT_H

#include "k9common.h"
#include "k9copy.h"
#include "ui_import.h"
#include "k9newdvd.h"
#include <QObject>
class k9AviFile;
class k9MenuButton;
class k9MenuEdit;
class k9ImportFiles;
class k9LvItemImport;
class k9Import : public QWidget {
    Q_OBJECT
private:
    QTreeWidgetItem *m_root;
    k9NewDVD *m_newDVD;
//    KMdiToolViewAccessor *m_toolView;
//    KDockWidget *m_dockWidget;
    k9ImportFiles *m_parent;
    k9LvItemImport *m_selected;
    QList <k9CdDrive*> recorderList;
    k9CdDrives *drives;
    k9MenuEdit *m_menuEdit;
    Ui::import Ui_import;
public:
    k9Import(QWidget* parent = 0, const char* name = 0,k9CdDrives *_drives=0 );
    ~k9Import();
    /*$PUBLIC_FUNCTIONS$*/
    void init();
    void execute();
    void clear();
    void removeTitle(k9LvItemImport *_item);
    QTreeWidgetItem* getRoot() const;
    k9NewDVD *getNewDVD() {
        return m_newDVD;
    }
    QTreeWidget *lvDVD;

public slots:
    /*$PUBLIC_SLOTS$*/
    void aviFileUpdated(k9AviFile *_aviFile);
    void buttonUpdated(k9MenuButton *_button, const QImage &_image);
    void setMenuEdit(k9MenuEdit* _value);
    void setEnableCreate(bool _state);
    void updateTotalTime();
    void titleAdded();
protected:
    /*$PROTECTED_FUNCTIONS$*/
    void readDrives();
    void addDrive (k9CdDrive *_drive);

protected slots:
    /*$PROTECTED_SLOTS$*/
    void lvDVDCurrentChanged( QTreeWidgetItem *,QTreeWidgetItem *);
    void cbFormatActivated(const QString &_format);
    void showPopup(const QPoint & iPoint);
    void removeTitle();
    void removeChapter();
    void addChapter();
    virtual void deviceAdded(k9CdDrive *_drive);
    virtual void deviceRemoved(k9CdDrive *_drive);
    virtual void volumeChanged(const QString &device,const QString &volumeName);

signals:
    void aviFileSelected(k9AviFile *_aviFile);
    void titleSelected(k9Title *);
    void rootSelected(k9NewDVD *);
};

#endif

