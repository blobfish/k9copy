//
// C++ Interface: k9copy
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef _K9COPY_H_
#define _K9COPY_H_

#include <config.h>
#include <kxmlguiwindow.h>
#include "k9common.h"
#include "k9cddrive.h"
#ifdef BACKLITE
#include <QLabel>
#else
#include <kapplication.h>
#include <KToggleAction>
#endif
#include <QObject>
#include <kaction.h>

enum eStatusBarId {sbMessage=0,sbFactor=1,sbSelSize=2};

class k9Import;
class k9Main;
class kViewMPEG2;
class k9PlaybackOptions;
class k9LangSelect;
class k9MP4Title;
class k9TitleFactor;
class k9MenuEdit;
class k9NewTitle;
class k9ChapterEdit;
class QDockWidget;
class k9ImportFiles;
class k9Copy : public KXmlGuiWindow {
    Q_OBJECT
friend class k9ImportFiles;
public:
    /**
     * Default Constructor
     */
    k9Copy();

    void clone(QString _input,QString _output);
    void setInput(QString _input);
    void setOutput(QString _output);
    void updateDock(int _output);
    //KDockWidget* getVisibleDock();
    //KMdiToolViewAccessor * setToolWindow(QWidget *_widget,KDockWidget::DockPosition _pos,const QString &tabToolTip,const QString &tabCaption) ;
    //void removeToolWindow(KMdiToolViewAccessor *toolWin);

    /**
     * Default Destructor
     */
    virtual ~k9Copy();

protected:
    bool queryClose( );
    void closeEvent( QCloseEvent* ce );

private slots:
    void ActionCopy();
    void ActionMP4();
    void ActionMPEG();
    void ActionEject();
    void ActionBackup();
    void ActionAuthor();
    void ActionPlayTitle();
    void ActionCreate();
    void ActionWizard();
#ifdef BACKLITE
    void About();
    void styleSelected(QAction *);
#endif
    void changeStatusbar(const QString& text,int id);
    void changeCaption(const QString& text);
    void optionsConfigureKeys();
    void optionsConfigureToolbars();
    void newToolbarConfig();
    void setGtkStyle(bool);
public slots:
    void fileOpen();
    void preferences();
    void quit();
    void setActions(bool enabled);
    void setDvdOpened(bool _state);
private:
    void setupAccel();
    void setupActions();
    void initCodecs();
    void createImportWindows();
    void createCopyWindows();
    void removeToolWindows();
    void saveDock();
    bool foundGtkStyle();
    KAction *CopyAction;
    KAction *PlayTitleAction;
    KAction *ejectAction;
    KAction *backupAction;
    KAction *authorAction;
    KAction *wizardAction;
private:
    k9Main  *m_k9Main;
//    KMdiChildView *m_k9MainView,*m_k9ImportView;
 //   KMdiToolViewAccessor *m_previewAcc;
    k9PlaybackOptions *m_options;
    k9TitleFactor *m_factors;
    k9LangSelect *m_lang;
    k9MP4Title *m_mp4;
    QWidget *m_mp2;
    k9MenuEdit *m_menuEdit;
    k9NewTitle *m_newTitle;
    k9ChapterEdit *m_chapterEdit;
    k9ImportFiles *m_import;
    bool m_useXine;
    bool m_usePhonon;
    bool m_useMplayer;
    bool m_useDvdAuthor;

    QMap  <QString,KAction*> m_actions;
    //QPtrList <KMdiToolViewAccessor> m_ToolViews;
    k9CdDrives *m_drives;
    QDockWidget *m_dockPreview;
    QDockWidget *m_dockMP4;
    QDockWidget *m_dockFactor;
    QDockWidget *m_dockPlayback;
    QString m_style;
#ifdef BACKLITE
    QLabel *m_lFactor,*m_lSize;
#endif
};



#endif
