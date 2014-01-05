//
// C++ Implementation: k9copy
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9main.h"
#include "k9copy.h"
#include "k9settings.h"
#include "k9playbackoptions.h"
#include "k9langselect.h"
#ifdef BACKLITE
#include "aboutbox.h"
#include <QActionGroup>
#include <QStatusBar>
#include <QStyleFactory>
#include <QMenuBar>
#include <QToolBar>
#else
#include <kdeversion.h>
#include <kstatusbar.h>
#include <kstdaccel.h>

#include <KStandardAction>
#include <kedittoolbar.h>
#include <KShortcutsDialog>
#include <KActionCollection>
#include "k9importfiles.h"
#include <QStyleFactory>
#endif
#include <kaction.h>
#include "kviewmpeg2.h"
#include "k9titlefactor.h"
#include "k9mp4title.h"
#include "k9tools.h"
#include <QDockWidget>
#include "images.h"
#include "dvdread.h"
#include <QMenu>
#include "k9dialogs.h"
#include "k9assistant.h"
#include "k9videocodecs.h"
#include "k9audiocodecs.h"
#include "k9importfiles.h"

k9Copy::k9Copy()
        : KXmlGuiWindow( ) {
   // accept dnd
    m_import=NULL;
    m_mp4=NULL;
    setAcceptDrops(true);
    m_style=qApp->style()->objectName();

    setDockOptions(QMainWindow::AllowTabbedDocks |QMainWindow::AnimatedDocks);
    setCorner(Qt::BottomRightCorner,Qt::RightDockWidgetArea);
    setCorner(Qt::TopRightCorner,Qt::RightDockWidgetArea);

    setCorner(Qt::BottomLeftCorner,Qt::LeftDockWidgetArea);

    // then, setup our actions
    setupActions();

    // and a status bar
    statusBar()->show();
    m_drives=new k9CdDrives(this);
    k9Config config;
#ifdef BACKLITE

    if (config.getPrefStyle()!="")
        qApp->setStyle(config.getPrefStyle());
    createCopyWindows();
    restoreGeometry(config.getPrefGeometry("mainwindow"));

    m_lSize=new QLabel(this);
    statusBar()->addPermanentWidget(m_lSize,0);
    m_lFactor=new QLabel(this);
    statusBar()->addPermanentWidget(m_lFactor,0);

#else
    createCopyWindows();

    statusBar()->insertItem(i18n("Ready"),sbMessage,1);
    statusBar()->setItemAlignment(sbMessage,Qt::AlignLeft | Qt::AlignVCenter);

    statusBar()->insertFixedItem("",sbFactor);
    statusBar()->setItemFixed(sbFactor,100);

    statusBar()->insertFixedItem("",sbSelSize);
    statusBar()->setItemFixed(sbSelSize,100);
    setupGUI();
#endif

   // backupAction->setChecked(true);

    
    int output=config.getOutputDev();
    updateDock(output<100?0:output-100);
    k9Dialogs::setMainWidget(this);
    setDvdOpened(false);

}


bool k9Copy::foundGtkStyle() {
    foreach (QString s , QStyleFactory::keys()) {
        if (s.toLower()=="gtk+") {
             return true;
        }
    }
    return false;
}

void k9Copy::setGtkStyle(bool _checked) {
#ifndef BACKLITE
    if (_checked) {
        if (foundGtkStyle())
         qApp->setStyle("gtk+");
    } else
         qApp->setStyle(m_style);
    k9Config config;
    config.setGtkStyle(_checked);
    config.save();
#endif
}

void k9Copy::createCopyWindows() {
    m_dockMP4=new QDockWidget(this);
    m_dockFactor=new QDockWidget(this);
    m_dockPlayback=new QDockWidget(this);
    m_actions["open"]->setEnabled(true);
    m_actions["quit"]->setEnabled(true);
    m_actions["preferences"]->setEnabled(true);
    m_actions["copy"]->setEnabled(true);
    m_actions["eject"]->setEnabled(true);
    m_actions["wizard"]->setEnabled(true);


    k9Config config;
    m_useXine=config.getUseXine();
    m_usePhonon=config.getUsePhonon();
    m_useMplayer=config.getUseMplayer();

    m_useDvdAuthor=config.getUseDvdAuthor();

    m_k9Main=new k9Main(this,0,m_drives);
    setCentralWidget(m_k9Main);

    m_factors =new k9TitleFactor( this);
    m_factors->setUseDvdAuthor(m_useDvdAuthor);
    QDockWidget *doc=m_dockFactor;//i18n("Shrink Factor"),this,Qt::Tool);
    doc->setObjectName("factors");
    doc->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |QDockWidget::DockWidgetClosable );
    QDockWidget *firstDock=doc;
    doc->setWidget(m_factors);
    doc->setWindowTitle(i18n("Shrink Factor"));
    addDockWidget(Qt::BottomDockWidgetArea, doc);
 
    connect(m_k9Main,SIGNAL(SelectionChanged( k9DVD*,bool )),m_factors,SLOT(SelectionChanged( k9DVD*,bool )));
    connect(m_k9Main,SIGNAL(changedTitle( k9DVDTitle* )),m_factors,SLOT(changedTitle( k9DVDTitle* )));

    m_mp4=new k9MP4Title(this);
    doc=m_dockMP4;//i18n("MPEG4 Encoding Options"),this,Qt::Tool);
    doc->setObjectName("mp4title");
    doc->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |QDockWidget::DockWidgetClosable);
    doc->setWidget(m_mp4);
    doc->setWindowTitle(i18n("MPEG4 Encoding Options"));
    addDockWidget(Qt::BottomDockWidgetArea, doc);
    tabifyDockWidget(firstDock,doc);

    connect(m_k9Main,SIGNAL(changedTitle( k9DVDTitle* )),m_mp4,SLOT(titleChanged( k9DVDTitle* )));
    connect(m_k9Main,SIGNAL(SelectionChanged( k9DVD*,bool )),m_mp4,SLOT(selectionChanged( k9DVD*,bool )));
    connect(m_mp4,SIGNAL(updateSelection()),m_k9Main,SLOT(updateSelection()));
    
    // allow the view to change the statusbar and caption

    connect(m_k9Main, SIGNAL(changeStatusbar(const QString&,int )),
            this,   SLOT(changeStatusbar(const QString&,int)));
    connect(m_k9Main, SIGNAL(changeCaption(const QString&)),
            this,   SLOT(changeCaption(const QString&)));



    doc=m_dockPlayback;
    doc->setObjectName("playbackoptions");
    firstDock=doc;
    doc->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |QDockWidget::DockWidgetClosable);
    m_options=new k9PlaybackOptions(m_k9Main,this);
    doc->setWidget(m_options);
    doc->setWindowTitle(i18n("DVD playback options"));
    addDockWidget(Qt::RightDockWidgetArea, doc);


    m_lang=new k9LangSelect(m_k9Main,this);
    doc=new QDockWidget(this);//,this,Qt::Tool);
    doc->setObjectName("langselect");
    doc->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |QDockWidget::DockWidgetClosable);
    doc->setWidget(m_lang);
    doc->setWindowTitle(i18n("Selection"));
    addDockWidget(Qt::RightDockWidgetArea, doc);
    tabifyDockWidget(firstDock,doc);
    if (m_useXine) {
#ifndef BACKLITE
  	m_mp2=new k9xine(this);
#endif
    }
    else if (m_usePhonon) {
	m_mp2=new k9Phonon(this);
    }else if (m_useMplayer)
	m_mp2=new K9Mplayer(this);
    else
        m_mp2=new kViewMPEG2(this);

    doc=new QDockWidget(this);//i18n("Preview"),this,Qt::Tool);
    doc->setObjectName("preview");
    m_dockPreview=doc;
    doc->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable |QDockWidget::DockWidgetClosable );
    doc->setWidget(m_mp2);
    doc->setWindowTitle(i18n("Preview"));
    addDockWidget(Qt::RightDockWidgetArea, doc);
    tabifyDockWidget(firstDock,doc);

    connect(m_k9Main,SIGNAL(showPreview( k9DVD*, k9DVDTitle*,int )),m_mp2,SLOT(open( k9DVD*, k9DVDTitle*,int )));
    connect(m_k9Main,SIGNAL(stopPreview()),m_mp2,SLOT(bStopClick()));

#ifndef BACKLITE
    setAutoSaveSettings();
#endif


}

void k9Copy::removeToolWindows() {
/*    for (KMdiToolViewAccessor *acc=m_ToolViews.first();acc;acc=m_ToolViews.next()) {
        deleteToolWindow(acc);
    }
    m_ToolViews.clear();
*/
}

void k9Copy::createImportWindows() {
    if (!m_import)
       m_import=new k9ImportFiles(this);    
    m_import->show();
    k9Dialogs::setMainWidget(m_import);
    hide();
}

k9Copy::~k9Copy() {
}

void k9Copy::initCodecs() {
    k9VideoCodecs v(NULL);
    v.reset();
    k9AudioCodecs a(NULL);
    a.reset();
}

bool k9Copy::queryClose   (    ) {
    return true;
}

void k9Copy::setupActions() {
    CopyAction = new KAction(KIcon("dvdcopy"),i18n("Copy"),this);
    connect(CopyAction, SIGNAL(triggered(bool)), SLOT(ActionCopy()));
    m_actions["copy"]=CopyAction;

    ejectAction = new KAction(KIcon("media-eject"),i18n("Eject"),this);
    connect(ejectAction, SIGNAL(triggered(bool)), SLOT(ActionEject()));
    m_actions["eject"]=ejectAction;

    wizardAction = new KAction(KIcon("tools-wizard"),i18n("Wizard"),this);
    connect(wizardAction, SIGNAL(triggered(bool)), SLOT(ActionWizard()));
    m_actions["wizard"]=wizardAction;

    authorAction = new KAction(KIcon("author"),i18n("DVD Author"),this );
    connect(authorAction, SIGNAL(triggered(bool)), SLOT(ActionAuthor()));
    m_actions["author"]=authorAction;

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
    QMenu *helpMenu=menuBar()->addMenu(i18n("&Help"));
    m_actions["open"]=new KAction(style()->standardIcon(QStyle::SP_DriveDVDIcon),i18n("&Open"),this);
    connect(m_actions["open"],SIGNAL(triggered(bool)),SLOT(fileOpen()));
    fileMenu->addAction(m_actions["open"]);
    fileMenu->addSeparator();

    m_actions["quit"]=new KAction(style()->standardIcon(QStyle::SP_DialogCloseButton),i18n("&Quit"),this);
    connect(m_actions["quit"],SIGNAL(triggered(bool)),SLOT(quit()));
    fileMenu->addAction(m_actions["quit"]);


    m_actions["preferences"]=new KAction(SmallIcon("configure"),i18n("Settings"),this);
    connect(m_actions["preferences"],SIGNAL(triggered(bool)),SLOT(preferences()));
    configureMenu->addAction(m_actions["preferences"]);

    k9Config config;

    QActionGroup *grp=new QActionGroup(this);
    QAction *styles=new QAction("Style",this);
    configureMenu->addAction(styles);
    QMenu *styleMenu=new QMenu(i18n("Style"),this);
    styles->setMenu(styleMenu);
    grp->setExclusive(true);
    QAction *defStyle=grp->addAction(i18n("default"));
    defStyle->setData("");
    defStyle->setCheckable(true);
    defStyle->setChecked(config.getPrefStyle()=="");
    styleMenu->addAction(defStyle);
    foreach (QString s , QStyleFactory::keys()) {
        QAction *st=grp->addAction(s);
        styleMenu->addAction(st);
        st->setData(s.toLower());
        st->setCheckable(true);
        st->setChecked(s.toLower()==config.getPrefStyle());
    }
    connect(grp,SIGNAL(triggered(QAction*)),this,SLOT(styleSelected(QAction*)));


    m_actions["about"]=new KAction(QIcon(),i18n("About backlite"),this);
    connect(m_actions["about"],SIGNAL(triggered(bool)),SLOT(About()));
    helpMenu->addAction(m_actions["about"]);

    actionMenu->addAction(CopyAction);
    actionMenu->addSeparator();
    actionMenu->addAction(ejectAction);
    actionMenu->addSeparator();
    actionMenu->addAction(wizardAction);
    actionMenu->addSeparator();
    actionMenu->addAction(authorAction);
    toolbar->addAction(m_actions["open"]);
    toolbar->addAction(CopyAction);
    toolbar->addSeparator();

    toolbar->addAction(wizardAction);
    toolbar->addSeparator();
    toolbar->addAction(m_actions["preferences"]);
    toolbar->addAction(ejectAction);
    toolbar->addAction(m_actions["quit"]);

#else 
    m_actions["open"]=KStandardAction::open(this, SLOT(fileOpen()), (QObject*)actionCollection());
    m_actions["quit"]=KStandardAction::quit(this, SLOT(quit()),(QObject*) actionCollection());
    m_actions["preferences"]=KStandardAction::preferences(this,SLOT(preferences()),(QObject*)actionCollection());
    KStandardAction::keyBindings(this, SLOT(optionsConfigureKeys()), (QObject*)actionCollection());
    KStandardAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), (QObject*)actionCollection());

    setStandardToolBarMenuEnabled(TRUE);
    createStandardStatusBarAction();

    QStringList slEnv=QProcess::systemEnvironment();
    bool bGtk=true;
    foreach (QString env,slEnv) {
        if (env.startsWith("KDE_FULL_SESSION=true"))
            bGtk=false;
    }
    if (bGtk && foundGtkStyle()) {
        m_actions["gtk-style"]=new KAction(KIcon("configure"),i18n("Use Gtk Style"),this);
        m_actions["gtk-style"]->setCheckable(true);
        connect(m_actions["gtk-style"],SIGNAL(triggered(bool)),this,SLOT(setGtkStyle(bool)));
        k9Config config;
        if (config.getGtkStyle()) {
            m_actions["gtk-style"]->setChecked(true);
            qApp->setStyle("gtk+");
        }

    }

    actionCollection()->addAction("Copy",CopyAction);
    actionCollection()->addAction("Eject",ejectAction);
    actionCollection()->addAction("Wizard",wizardAction);
    actionCollection()->addAction("author",authorAction);
    actionCollection()->addAction("gtk-style",m_actions["gtk-style"]);
#endif

}

#ifdef BACKLITE
void k9Copy::styleSelected(QAction *act) {
    QString st=act->data().toString();
    qApp->setStyle(st!="" ? st: m_style);
    k9Config config;
    config.setPrefStyle(st);
    config.save();
}
#endif

void k9Copy::setActions( bool enabled) {
        m_actions["open"]->setEnabled(enabled);
        m_actions["quit"]->setEnabled(enabled);
        m_actions["preferences"]->setEnabled(enabled);
        m_actions["copy"]->setEnabled(enabled);
	m_actions["wizard"]->setEnabled(enabled);
        m_actions["eject"]->setEnabled(enabled);
        m_mp2->setEnabled(enabled);
        m_mp4->setEnabled(enabled);
        m_factors->setEnabled(enabled);
        m_lang->setEnabled(enabled);
        m_options->setEnabled(enabled);

}

void k9Copy::optionsConfigureKeys() {
#ifndef BACKLITE
    KShortcutsDialog::configure(actionCollection());
#endif
}

void k9Copy::optionsConfigureToolbars() {
#ifndef BACKLITE
    KEditToolBar dlg(factory());
    connect(&dlg,SIGNAL(newToolbarConfig()),this,SLOT(newToolbarConfig()));
    dlg.exec();
#endif
}

void k9Copy::newToolbarConfig() {
    // this slot is called when user clicks "Ok" or "Apply" in the toolbar editor.
    // recreate our GUI, and re-apply the settings (e.g. "text under icons", etc.)
#ifndef BACKLITE
    createGUI(0);
#endif
}

void k9Copy::fileOpen() {
    m_k9Main->Open();

}



void k9Copy::preferences() {
    k9Settings settings(this,i18n("Settings"));
    settings.exec();
    k9Config config;
    m_useXine=config.getUseXine();
    m_usePhonon=config.getUsePhonon();
    m_useMplayer=config.getUseMplayer();

//    if (m_k9MainView) {
        m_k9Main->readSettings();
        m_mp2->close();
        if (m_useMplayer)
            m_mp2=new K9Mplayer(this);
#ifndef BACKLITE
	else if (m_useXine)
	    m_mp2=new k9xine(this);
#endif
 	else if (m_usePhonon)
	    m_mp2=new k9Phonon(this);
        else
             m_mp2=new kViewMPEG2(this);
	m_dockPreview->setWidget(m_mp2);

        connect(m_k9Main,SIGNAL(showPreview( k9DVD*, k9DVDTitle*,int )),m_mp2,SLOT(open( k9DVD*, k9DVDTitle*,int )));
        connect(m_k9Main,SIGNAL(stopPreview()),m_mp2,SLOT(bStopClick()));
        m_mp4->load();
  //  }
}


void k9Copy::ActionCopy() {
    if (m_useMplayer)
        ((K9Mplayer*)m_mp2)->bStopClick();
#ifndef BACKLITE
    else if (m_useXine)
	((k9xine*)m_mp2)->bStopClick();
#endif
    else if (m_usePhonon)
	((k9Phonon*)m_mp2)->bStopClick();
    else
        ((kViewMPEG2*)m_mp2)->bStopClick();
    m_k9Main->Copy();
}

void k9Copy::ActionPlayTitle() {
//    m_previewAcc->show();
    m_k9Main->PreviewTitle();

}

void k9Copy::ActionMP4() {
    m_k9Main->CreateMP4();
}

void k9Copy::ActionMPEG() {
    m_k9Main->extractMPEG2();
}


void k9Copy::ActionEject() {
    m_k9Main->eject();
}

void k9Copy::ActionBackup() {
   createCopyWindows();
}

void k9Copy::ActionAuthor() {
    createImportWindows();
}
#ifdef BACKLITE
void k9Copy::About() {
    AboutBox ab(this);
    ab.exec();
}
#endif

void k9Copy::ActionCreate() {
//    m_k9Import->execute();
}

void k9Copy::ActionWizard() {
	hide();
	k9Assistant *ast=k9Assistant::createAssistant();
	k9Dialogs::setMainWidget(ast);
	ast->run();
#ifdef BACKLITE
        ast->exec();
#endif
        show();
}

/*KMdiToolViewAccessor * k9Copy::setToolWindow(QWidget *_widget,KDockWidget::DockPosition _pos,const QString &tabToolTip,const QString &tabCaption) {
    KMdiToolViewAccessor *acc=addToolWindow(_widget, _pos, getMainDockWidget(),0,tabToolTip,tabCaption);
    m_ToolViews.append(acc);
    return acc;
}

void k9Copy::removeToolWindow(KMdiToolViewAccessor *toolWin) {
    deleteToolWindow(toolWin);
    m_ToolViews.removeRef(toolWin);
}

*/
void k9Copy::changeStatusbar(const QString& text,int id) {
    // display the text on the statusbar
#ifndef BACKLITE
    statusBar()->changeItem(text,id);
#else
    switch(id) {
        case sbMessage:
            statusBar()->showMessage(text,0);
            break;
        case sbFactor:
            m_lFactor->setText(text);
            break;
        case sbSelSize:
            m_lSize->setText(text);
            break;
    }
#endif
}

void k9Copy::changeCaption(const QString& text) {
    // display the text on the caption
#ifdef BACKLITE
    setWindowTitle(text);
#else
    setCaption(text);
#endif
}

void k9Copy::clone( QString _input,QString _output) {
    show();
    m_k9Main->Clone( _input,_output);
}

void k9Copy::setInput(QString _input) {
    m_k9Main->setInput(_input);
}

void k9Copy::setOutput(QString _output) {
    m_k9Main->setOutput(_output);
}


void k9Copy::quit() {
    close( );
}

void k9Copy::saveDock() {
//    KConfig config("K9Copy");
//    if (m_k9MainView) {
//        dockManager->writeConfig(&config,"dock");
//        m_k9Main->saveSettings();
//    } else if (m_k9ImportView) {
//        dockManager->writeConfig(&config,"dockImport");
//    }
}

void k9Copy::closeEvent( QCloseEvent* ce ) {
    m_k9Main->saveSettings();
#ifndef BACKLITE
    saveAutoSaveSettings();
#else
    k9Config config;
    config.setPrefGeometry(saveGeometry(),"mainwindow");
    config.save();
#endif
    ce->accept();
    qApp->quit();
}

void k9Copy::setDvdOpened(bool _state) {
    m_actions["eject"]->setEnabled(_state);
    m_actions["copy"]->setEnabled(_state);
}

void k9Copy::updateDock(int _output) {
  switch ((k9Main::eOutput)_output) {
    case k9Main::oDVD:
    case k9Main::oISO:
    case k9Main::oFolder:
    case k9Main::oMPEG2:
      removeDockWidget(m_dockMP4);
      restoreDockWidget(m_dockFactor);
      restoreDockWidget(m_dockPlayback);

      break;
    case k9Main::oMP4 :
      restoreDockWidget(m_dockMP4);
      removeDockWidget(m_dockFactor);
      removeDockWidget(m_dockPlayback);
      if (m_mp4)
	m_mp4->setOptions(k9MP4Title::optAll);
      break;
    case k9Main::oAudio:
      restoreDockWidget(m_dockMP4);
      removeDockWidget(m_dockFactor);
      removeDockWidget(m_dockPlayback);
      if (m_mp4)
	m_mp4->setOptions(k9MP4Title::optAudio);
      break;  
  }
  
}

