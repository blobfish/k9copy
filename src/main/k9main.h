//
// C++ Interface:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9MAIN_H
#define K9MAIN_H
#include "k9common.h"
#include "k9cddrive.h"
#include "ui_k9mainw.h"

#include "k9playbackoptions.h"
#include <qstring.h>
#include <qevent.h>
#include "k9copy.h"
#include <qmutex.h>
#ifndef BACKLITE
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kmainwindow.h>
#include <kaboutdata.h>

#endif
enum  eStreamType {SUB,AUD,VID,NONE,CHAP} ;
enum  eObjectType {TITLE,CHAPTER,TITLESET,STREAM,ROOT};

class LvItem : public QTreeWidgetItem {
public:
    LvItem( QTreeWidgetItem *parent,eObjectType _objectType)
            : QTreeWidgetItem( parent,1000), obj( NULL ) {
        objectType=_objectType;
    }
    LvItem( QTreeWidget *parent,eObjectType _objectType)
            : QTreeWidgetItem( parent,1000), obj( NULL ) {
        objectType=_objectType;
    }
    eObjectType objectType;
    QObject *obj;
    // virtual int rtti () const;
    bool operator< ( const QTreeWidgetItem & other ) const  ;
//    void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );
};

class k9Main;
class k9DVD;
class  k9DVDAudioStream;
class  k9DVDSubtitle;
class k9DVDTitle;
class k9PlaybackOptions;
class k9LangSelect;
class k9UpdateFactor;

class ckLvItem : public QTreeWidgetItem {
public:
    ckLvItem( QTreeWidgetItem *parent,k9Main *dlg,eObjectType _objectType);
    ckLvItem( QTreeWidget *parent,k9Main *dlg,eObjectType _objectType);
    eStreamType streamType;
    eObjectType objectType;

    k9Main *mainDlg;
    k9DVDTitle *mainTitle;
    void *obj;
    void *stream;
    QString language;
    // virtual int rtti () const;
    // void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align );
    // int compare ( QListViewItem * i, int col, bool ascending ) const;
    double getstreamSize();
    bool isOn() {
        return checkState(0)==Qt::Checked;
    }
    void setOn(bool value) {
        setCheckState(0, value ? Qt::Checked : Qt::Unchecked);
    }
    void stateChange(bool state);
    bool renamed(QString _text) {
        if (_text != m_text) {
            _text=m_text;
            return true;
        }
        return false;
    };
    bool operator< ( const QTreeWidgetItem & other ) const  ;
private:
    bool m_checked;
    QString m_text;
};


class k9DVDListItem : public QObject {
    Q_OBJECT
public:
    k9DVDAudioStream *audioStream;
    k9DVDSubtitle *subtitle;
    k9DVDTitle *title;
    ckLvItem *listItem;
    eStreamType streamType;
public:
    k9DVDListItem(void *DVD,ckLvItem *List,eStreamType type);
};


class k9ListColumn : public QObject {
    Q_OBJECT
private:
    int m_column;
public:
    k9ListColumn (QTreeWidget *parent, int column):QObject((QObject*)parent),m_column(column) {}
public slots:
    void setVisible(bool);
};


class k9Main : public QWidget {
    Q_OBJECT

public:
    enum eOutput {oDVD=0,oISO=1,oFolder=2,oMP4=3,oAudio=4,oMPEG2=5};
    enum eInput {iDVD=0,iISO=1,iFolder=2};
    
    k9Main(QWidget* parent = 0, const char* name = 0,  k9CdDrives *_drives=0 );
    ~k9Main();
    /*$PUBLIC_FUNCTIONS$*/
    void addTitle(k9DVDTitle *track);
    void addChapters(QTreeWidgetItem *_parent,k9DVDTitle *_title);
    void checkAll(bool state);
    void checkTS( bool _state,ckLvItem *_item );
    void checkTitle(bool state, ckLvItem *_item);
    void checkLang(QString lang, eStreamType streamType,bool state);
    bool getupdating();
    void saveSettings();
    void setDVDSize();
    static bool compare(double v1,double v2);
    void readSettings();
    void resizeColumns();

    bool getquickScan() {
        return m_quickScan;
    };
    void setPlaybackOptions(k9PlaybackOptions *_value) {
        m_playbackOptions=_value;
    };
    void setLangSelect(k9LangSelect *_value) {
        m_langSelect=_value;
    };
    QList <k9DVDListItem*> *getItems() {
        return &items;
    };
    void updateFactor();
    bool withMenus();
    k9DVD *dvd;
    void eject();
    QTreeWidget *getTreeView() {
        return listView1;
    };
private slots:
    virtual void          listView1CurrentChanged( QTreeWidgetItem *,QTreeWidgetItem* );
    virtual void          bSaveClick();
    virtual void	  cbOutputDevActivated(int);
    virtual void	  bInputOpenClick();
    virtual void	  bInputOpenDirClick();
    virtual void	  cbInputChanged(int);
    virtual void 	  cbOutputChanged(int);
    virtual void 	  updateFactor_internal();
    virtual void	  deviceAdded(k9CdDrive *_drive);
    virtual void	  deviceRemoved(k9CdDrive *_drive);
    virtual void	  expanded(QTreeWidgetItem*);
    virtual void	  collapsed (QTreeWidgetItem*);

    virtual void	  listView1ItemChanged(QTreeWidgetItem*,int);
    virtual void	  showPopup(const QPoint & iPosition);
public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void          PreviewTitle();
    virtual void          CreateMP4();
    virtual void	  extractMPEG2 ();
    virtual void	  extractAudio();
    virtual void          Copy();
    virtual void          Open();
    virtual void	   Clone(QString _input,QString _output);
    virtual void	   setInput(QString _input);
    virtual void 	   setOutput(QString _output);
    virtual void 	  volumeChanged(const QString &device,const QString &volumeName);
    virtual void 	  updateSelection();
    void setDrives(k9CdDrives* _value);

protected:
    /*$PROTECTED_FUNCTIONS$*/
    bool m_ready;
    QTreeWidget *listView1;
    QList <k9DVDListItem*> items;
    QList <k9ListColumn*> columns;
    k9DVDListItem *addListItem(void *DVD,ckLvItem *List,eStreamType type);

    void readDrives();
    void addDrive (k9CdDrive *_drive);
    k9Copy *m_parent;
    QVector<ckLvItem*> tsItems;
    QVector<ckLvItem*> chItems;
    ckLvItem * root;
    QList <k9CdDrive*> driveList;
    QList <k9CdDrive*> recorderList;
    k9CdDrives *drives;
    QPixmap pxVideo;
    QPixmap pxSound;
    QPixmap pxText;
    QPixmap pxChapter;
    QMutex m_mutex;
    bool updating;
    bool fspFinish;
    long fspAvail;
    void closeDVD();
    QString  getDevice(QComboBox *_combo);
    k9UpdateFactor *m_update;
    //PREFERENCES
    QString m_prefOutput;
    bool m_useDvdAuthor;
    bool m_quickScan;
    int  m_prefSize;
    bool m_prefK3b;
    bool m_prefMenu;
    bool m_prefAutoBurn;
    k9PlaybackOptions *m_playbackOptions;
    k9LangSelect *m_langSelect;

    QDockWidget *m_dockWidget;
    void resizeEvent ( QResizeEvent * event );
    void showEvent ( QShowEvent * event);
protected slots:
    /*$PROTECTED_SLOTS$*/
    void itemRenamed ( QTreeWidgetItem *item, int col );
signals: // Signals
    /** No descriptions */
    void sig_progress(QString str);
    void changeStatusbar(const QString& str,int id);
    void changeCaption(const QString& str);
    void showPreview(k9DVD *_dvd,k9DVDTitle * title,int chapter);
    void stopPreview();
    void SelectionChanged(k9DVD *_dvd,bool _withMenus);
    void changedTitle(k9DVDTitle *_title);
private:
    void fillLvLanguages();
    void updateLvLang(const eStreamType streamType,const QString & lang) ;
    long getFreeSpace(const QString & _path);
    void clearItems();
    Ui::MainDlg Ui_MainDlg;

};

#endif

