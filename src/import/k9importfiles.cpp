//
// C++ Implementation: k9copy
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2005
//
// Copyright: See COPYING file that comes nth this distribution
//
//
#include "k9importfiles.h"
#include "k9settings.h"

#include <kaction.h>
#include <qframe.h>
#include "k9tools.h"
#include <QDockWidget>
#include "images.h"
#include "dvdread.h"
#include <QMenu>
#include <QCloseEvent>
#ifndef BACKLITE
#include <kdeversion.h>
#include <KStandardAction>
#include <kstatusbar.h>
#include <kstdaccel.h>
#include <KActionCollection>
#include <KMenuBar>
#include <KShortcutsDialog>
#include <kedittoolbar.h>
#else
#include <QMenuBar>
#include <QToolBar>
#endif

#include "k9import.h"
#include "k9menuedit.h"
#include "k9chapteredit.h"
#include "k9newtitle.h"
#include "k9menu.h"
#include "k9dialogs.h"
#include "kdialog.h"
k9ImportFiles::k9ImportFiles(k9Copy *_parent)
        : KXmlGuiWindow(_parent,0 ) {
   // accept dnd
 
    m_parent=_parent;
    setAcceptDrops(true);
    setDockOptions(QMainWindow::ForceTabbedDocks |QMainWindow::AnimatedDocks);
    setCorner(Qt::BottomRightCorner,Qt::RightDockWidgetArea);
    setCorner(Qt::TopRightCorner,Qt::RightDockWidgetArea);

    setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);

    // then, setup our actions
    setupActions();

    // and a status bar
    m_drives=new k9CdDrives(this);
    createWindows();
#ifndef BACKLITE
    statusBar()->show();
    statusBar()->insertFixedItem("",sbFactor);
    statusBar()->setItemFixed(sbFactor,100);
    statusBar()->insertItem("",sbMessage,1);
    statusBar()->setItemAlignment(sbMessage,Qt::AlignLeft | Qt::AlignVCenter);
    setupGUI();
#endif

   // backupAction->setChecked(true);
#ifdef BACKLITE
    k9Config config;
    restoreGeometry(config.getPrefGeometry("import"));
#endif
    changeCaption(i18n("DVD Author"));
}


void k9ImportFiles::createWindows() {

    m_actions["quit"]->setEnabled(true);
    m_actions["preferences"]->setEnabled(true);
    m_actions["create"]->setEnabled(false);

    m_k9Import=new k9Import (this,0,m_drives);
    setCentralWidget(m_k9Import);

    m_newTitle =new k9NewTitle( this);
    m_newTitle->setK9Import(m_k9Import);
    QDockWidget *doc=new QDockWidget(this);//i18n("Shrink Factor"),this,Qt::Tool);
    doc->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |QDockWidget::DockWidgetClosable );
    doc->setObjectName("addTitle");
    QDockWidget *firstDock=doc;
    m_firstBottom=doc;

    doc->setWidget(m_newTitle);
    doc->setWindowTitle(i18n("Add Title"));
    addDockWidget(Qt::BottomDockWidgetArea, doc);
 

    m_chapterEdit=new k9ChapterEdit(this);
    doc=new QDockWidget(this);//i18n("MPEG4 Encoding Options"),this,Qt::Tool);
    doc->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |QDockWidget::DockWidgetClosable);
    doc->setWidget(m_chapterEdit);
    doc->setWindowTitle(i18n("Chapter properties"));
    doc->setObjectName("chapterEdit");
    addDockWidget(Qt::BottomDockWidgetArea, doc);
    tabifyDockWidget(doc,firstDock);

    connect(m_k9Import,SIGNAL(aviFileSelected(k9AviFile*)),m_chapterEdit,SLOT(setAviFile(k9AviFile*)));

    doc=new QDockWidget(this);//
    firstDock=doc;
    doc->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |QDockWidget::DockWidgetClosable);
    m_menuEdit=new k9MenuEdit(this,0,m_k9Import->getNewDVD()->getRootMenu()->getScene());
    doc->setWidget(m_menuEdit);
    doc->setObjectName("menuEdit");
    doc->setWindowTitle(i18n("Edit menu"));
    addDockWidget(Qt::RightDockWidgetArea, doc);
    connect (m_k9Import,SIGNAL(titleSelected(k9Title*)),m_menuEdit,SLOT(titleSelected(k9Title*)));
    connect (m_k9Import,SIGNAL(rootSelected(k9NewDVD*)),m_menuEdit,SLOT(rootSelected(k9NewDVD*)));
    m_k9Import->setMenuEdit(m_menuEdit);

    m_k9Import->init();

    setActions(true);
#ifndef BACKLITE
    setAutoSaveSettings();
#endif
}

void k9ImportFiles::removeToolWindows() {
/*    for (KMdiToolViewAccessor *acc=m_ToolViews.first();acc;acc=m_ToolViews.next()) {
        deleteToolWindow(acc);
    }
    m_ToolViews.clear();
*/
}


k9ImportFiles::~k9ImportFiles() {
  delete m_newTitle;
  delete m_menuEdit;
  delete m_chapterEdit;
  m_parent->m_import=NULL;
  m_k9Import->hide();
  m_k9Import->clear();
  delete m_k9Import;
  delete m_drives;

}

bool k9ImportFiles::queryClose   (    ) {

    return true;
}

void k9ImportFiles::setupActions() {
    m_actions["create"] = new KAction(KIcon("author"),i18n("Create DVD"),this);
    backupAction = new KAction(KIcon("dvdcopy"),i18n("DVD Backup"),this );
    m_actions["backup"]=backupAction;

#ifdef BACKLITE
    QMenu *fileMenu=menuBar()->addMenu(i18n("&File"));
    QToolBar *toolbar=addToolBar("Actions");
#if QT_VERSION >= 0x040600
    toolbar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
#else
    toolbar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon);
#endif
    QMenu *actionMenu=menuBar()->addMenu(i18n("A&ctions"));
    QMenu *configureMenu=menuBar()->addMenu(i18n("&Options"));

    m_actions["quit"]=new KAction(style()->standardIcon(QStyle::SP_DialogCloseButton),i18n("&Quit"),this);
    connect(m_actions["quit"],SIGNAL(triggered(bool)),SLOT(quit()));

    fileMenu->addAction(m_actions["quit"]);


    m_actions["preferences"]=new KAction(SmallIcon("configure"),i18n("Settings"),this);
    connect(m_actions["preferences"],SIGNAL(triggered(bool)),SLOT(preferences()));
    configureMenu->addAction(m_actions["preferences"]);

    fileMenu->addSeparator();
    actionMenu->addAction(m_actions["create"]);
    actionMenu->addAction(backupAction);
    toolbar->addAction(m_actions["create"]);
    toolbar->addSeparator();
    toolbar->addAction(m_actions["preferences"]);
    toolbar->addSeparator();
    toolbar->addAction(m_actions["quit"]);

#else
    m_actions["quit"]=KStandardAction::quit(this, SLOT(quit()),(QObject*) actionCollection());
    m_actions["preferences"]=KStandardAction::preferences(this,SLOT(preferences()),(QObject*)actionCollection());
    KStandardAction::keyBindings(this, SLOT(optionsConfigureKeys()), (QObject*)actionCollection());
    KStandardAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), (QObject*)actionCollection());


    setStandardToolBarMenuEnabled(TRUE);
    createStandardStatusBarAction();

    actionCollection()->addAction("Create",m_actions["create"]);

    actionCollection()->addAction("backup",backupAction);
#endif

    connect(m_actions["create"], SIGNAL(triggered(bool)), SLOT(ActionCreate()));
    connect(backupAction, SIGNAL(triggered(bool)), SLOT(ActionBackup()));
}

void k9ImportFiles::setActions( bool enabled) {
        m_actions["quit"]->setEnabled(enabled);
        m_actions["preferences"]->setEnabled(enabled);
        m_actions["create"]->setEnabled(enabled);
        m_menuEdit->setEnabled(enabled);
        m_newTitle->setEnabled(enabled);
        m_chapterEdit->setEnabled(enabled);
        m_k9Import->setEnabled(enabled);

}

void k9ImportFiles::optionsConfigureKeys() {
#ifndef BACKLITE
    KShortcutsDialog::configure(actionCollection());
#endif
}

void k9ImportFiles::optionsConfigureToolbars() {
#ifndef BACKLITE
    KEditToolBar dlg(factory());
    connect(&dlg,SIGNAL(newToolbarConfig()),this,SLOT(newToolbarConfig()));
    dlg.exec();
#endif
}

void k9ImportFiles::newToolbarConfig() {
    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
#ifndef BACKLITE
    createGUI(0);
#endif

//    applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
}

void k9ImportFiles::preferences() {
    k9Settings settings(k9Dialogs::getMainWidget(),i18n("Settings"));
    settings.exec();
}



void k9ImportFiles::ActionCreate() {
    m_k9Import->execute();
}

void k9ImportFiles::ActionBackup() {
    m_parent->show();
    k9Dialogs::setMainWidget(m_parent);
    hide();
/*   
*/ 

}

void k9ImportFiles::changeStatusbar(const QString& text,int id) {
    // display the text on the statusbar
#ifndef BACKLITE
    statusBar()->changeItem(text,id);
#endif
}

void k9ImportFiles::changeCaption(const QString& text) {
    // display the text on the caption
    setWindowTitle(KDialog::makeStandardCaption(text));
}


void k9ImportFiles::quit() {
    close( );
    qApp->quit();
}


void k9ImportFiles::closeEvent( QCloseEvent* ce ) {
    k9Config config;
    if (config.getPrefDelTmpFiles())
        k9Tools::clearOutput(config.getPrefOutput()+"/dvd");
#ifdef BACKLITE
    config.setPrefGeometry(saveGeometry(),"import");
    config.save();
#endif
    ce->accept();    
    m_parent->show();
}

void k9ImportFiles::setEnabledCreateDVD(bool _state) {
	m_actions["create"]->setEnabled(_state);
}

