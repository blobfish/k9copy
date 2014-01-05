//
// C++ Implementation:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9import.h"
#include "k9title.h"
#include "k9avifile.h"
#include "k9lvitemimport.h"
#include "k9menuedit.h"
#include "k9burndvd.h"
#include <qdir.h>
#include <QComboBox>
#ifndef BACKLITE
#include <kiconloader.h>
#endif

#include "k9processlist.h"
#include "k9importfiles.h"
#include "k9dialogs.h"
#include <QDockWidget>
#include <QAction>
#include <QMenu>
#include <kdialog.h>
#include "k9menu.h"
k9Import::k9Import(QWidget* parent, const char* ,k9CdDrives *_drives)
        : QWidget(parent) {
    Ui_import.setupUi(this);
    Ui_import.lvDVD->setIconSize(QSize(48,48));
    setWindowTitle(KDialog::makeStandardCaption(i18n("import"),this));
    m_parent=(k9ImportFiles*) parent;
    drives=_drives;
    m_newDVD=new k9NewDVD();
    connect(m_newDVD,SIGNAL(sigAddTitle()),this,SLOT(titleAdded()));
    lvDVD=Ui_import.lvDVD;
    connect(lvDVD,SIGNAL(currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem *)),this,SLOT(lvDVDCurrentChanged( QTreeWidgetItem *,QTreeWidgetItem *) ));
    connect (lvDVD,SIGNAL(customContextMenuRequested( const QPoint& )),this,SLOT(showPopup(const QPoint&)));
}

k9Import::~k9Import() {
   disconnect(this);
   if (m_newDVD)
    delete m_newDVD;	

}

void k9Import::clear() {
   
   for (int i=m_root->childCount()-1;i>=0;i--){
	m_selected=(k9LvItemImport*)m_root->child(i);
	removeTitle();
   }
  lvDVD->clear();
  delete m_newDVD;
  m_newDVD=NULL;
}

/*$SPECIALIZATION$*/

void k9Import::init() {
    m_root=new QTreeWidgetItem(lvDVD);
    m_root->setText(0,i18n("Root Menu"));
    m_root->setExpanded(true);
    m_root->setIcon(0,SmallIcon("media-optical-recordable"));
    emit aviFileSelected(NULL);

    connect(drives,SIGNAL(deviceAdded( k9CdDrive*)),this,SLOT(deviceAdded( k9CdDrive* )));
    connect(drives,SIGNAL(deviceRemoved( k9CdDrive*)),this,SLOT(deviceRemoved( k9CdDrive* )));

    readDrives();
    m_root->setSelected(true);
    emit rootSelected(m_newDVD);
    emit aviFileSelected(NULL);

}

void k9Import::readDrives() {
    Ui_import.cbOutputDev->clear();
    recorderList.clear();
    Ui_import.cbOutputDev->addItem(SmallIcon("document-open"), i18n("ISO Image"));
    Ui_import.cbOutputDev->addItem(SmallIcon("folder"),i18n("Folder"));

    drives->scanDrives();

}

void k9Import::deviceAdded(k9CdDrive *_drive) {
    addDrive(_drive);
}

void k9Import::deviceRemoved(k9CdDrive *_drive) {

    int i=recorderList.indexOf(_drive);
    recorderList.removeAll( _drive);
    Ui_import.cbOutputDev->removeItem(i+2);

}

void k9Import::addDrive(k9CdDrive *drive) {
#ifdef HAVE_HAL
    if (drive->getDevice()!=NULL) {
        k9HalDevice *dev=drive->getDevice();
        connect(dev,SIGNAL(volumeChanged( const QString &,const QString &)),this,SLOT(volumeChanged(const QString&,const QString&)));
    }
#endif
    if (drive->canWriteDVD) {
        Ui_import.cbOutputDev->addItem(SmallIcon("media-optical-recordable"), drive->name+ "  ("+drive->device+")",-1);
        recorderList.append(drive);
    }

}

void k9Import::volumeChanged(const QString &device,const QString &volumeName) { Q_UNUSED(device); Q_UNUSED(volumeName);}



QTreeWidgetItem * k9Import::getRoot() const {
    return m_root;
}


void k9Import::aviFileUpdated(k9AviFile *_aviFile) {
    for (int i=0;i< m_root->childCount();i++) {
        k9LvItemImport *itemTitle = (k9LvItemImport*)m_root->child(i);
        for (int j=0;j<itemTitle->childCount();j++) {
            k9LvItemImport *itemChapter = (k9LvItemImport*)itemTitle->child(j);
            if (itemChapter->getAviFile()==_aviFile) {
                itemChapter->setText(1,_aviFile->getStart().toString("hh:mm:ss") +" - "+_aviFile->getEnd().toString("hh:mm:ss"));
                itemChapter->setIcon(0,QIcon(QPixmap::fromImage(_aviFile->getImage().scaled(50,40))));
            }
        }
    }
    updateTotalTime();
}

void k9Import::buttonUpdated(k9MenuButton *_button, const QImage &_image) {
    for (int i=0;i< m_root->childCount();i++) {
        k9LvItemImport *itemTitle = (k9LvItemImport*)m_root->child(i);
        if (itemTitle->getTitle()->getButton()==_button) {
            QImage img=_image.scaled(50,40,Qt::IgnoreAspectRatio);
            QPixmap pix=QPixmap::fromImage(img);
            itemTitle->setIcon(0,QIcon(pix));
        }
    }
}

void k9Import::removeTitle(k9LvItemImport *_item) {
    m_selected=_item;
    removeTitle();
}

void k9Import::removeTitle() {
    m_newDVD->getRootMenu()->removeButton(m_selected->getTitle()->getButton());
    m_selected->getTitle()->getButton()->remove();
    m_newDVD->getTitles()->removeAll( m_selected->getTitle());
    m_selected->parent()->removeChild(m_selected);
}

void k9Import::addChapter() {
   k9LvItemImport *title=(k9LvItemImport*) m_selected;
   k9AviFile *file=new k9AviFile(title->getTitle(),0);

   if (title->childCount() >0) {
	k9AviFile *f2 = title->getTitle()->getFiles()->at(0);
	file->setReencode(f2->getReencode());
   }

   k9Config config;   


   k9LvItemImport * itemch=new k9LvItemImport(title,k9LvItemImport::CHAPTER);
   itemch->setText(0,i18n("chapter %1").arg(title->childCount()));
   itemch->setAviFile(file);
   itemch->setTitle(title->getTitle());
   connect(file,SIGNAL(aviFileUpdated(k9AviFile*)),this,SLOT(aviFileUpdated(k9AviFile*)));
   k9MenuButton *btn=title->getTitle()->getMenu()->addButton();
   btn->setNum(title->childCount());
        //_title->setButton(btn);
   QImage img;
   btn->setImage(img);
   btn->setTop(0);
   btn->setLeft(0);
   btn->setWidth(config.getPrefButtonWidth());
   btn->setHeight(config.getPrefButtonHeight());
   btn->setScript(QString("jump title 1 chapter %1 ;").arg(title->childCount()));
   btn->setTextPosition(BOTTOM);
   btn->setText(i18n("chapter %1").arg(title->childCount()));
   btn->setColor(config.getPrefButtonTextColor());
   btn->setFont(config.getPrefButtonFont());
   file->setButton(btn);
   connect(file,SIGNAL(imageChanged(const QImage&)),btn,SLOT(setImage(const QImage&)));
}

void k9Import::removeChapter() {

   k9LvItemImport *title=(k9LvItemImport*) m_selected->parent();
   title->getTitle()->getFiles()->removeAll(m_selected->getAviFile());

   //if trying to remove the last chapter, remove the entire title
   if (title->childCount()==1) {
	m_selected=title;
	removeTitle();
	return;
   }
	
   k9AviFile *previous=m_selected->getAviFile()->getPrevious();
   k9AviFile *next=m_selected->getAviFile()->getNext();
   if (previous) {
	previous->setNext(next);
	previous->setBreakNext(true);
   }
   if (next) {
	next->setPrevious(previous);
	next->setBreakPrevious(true);
   }

   m_selected->getAviFile()->getButton()->remove();

   title->getTitle()->getMenu()->removeButton(m_selected->getAviFile()->getButton());
   m_selected->parent()->removeChild(m_selected);
   emit titleSelected(title->getTitle());
   emit aviFileSelected(NULL);
}

void k9Import::showPopup(const QPoint & iPosition) {
    QTreeWidgetItem * item = 0 ;
    item = lvDVD->itemAt(iPosition) ;
    switch (item->type()) {
	case  k9LvItemImport::TITLE : {
		m_selected=(k9LvItemImport*) item;
		QMenu menu(lvDVD);
		QAction *m=new QAction(SmallIcon("list-remove"),i18n("remove"),this);
		connect(m, SIGNAL(triggered()), this, SLOT(removeTitle()));
		menu.addAction(m);
		m=new QAction(SmallIcon("list-add"),i18n("add chapter"),this);
		connect(m, SIGNAL(triggered()), this, SLOT(addChapter()));
		menu.addAction(m);


		menu.exec(lvDVD->mapToGlobal(iPosition));
	
	}
	break;
	case k9LvItemImport::CHAPTER: {
		m_selected=(k9LvItemImport*)item;
		QMenu menu(lvDVD);
		QAction *m=new QAction(SmallIcon("list-remove"),i18n("remove"),this);
		connect(m, SIGNAL(triggered()), this, SLOT(removeChapter()));
		menu.addAction(m);
	
		menu.exec(lvDVD->mapToGlobal(iPosition));
	}
	break;
	default:
		m_selected=NULL;
    }

}


void k9Import::lvDVDCurrentChanged( QTreeWidgetItem *_item,QTreeWidgetItem *)  {
    if (_item==NULL)
	return;
    k9LvItemImport *item=(k9LvItemImport*)_item;

    switch (_item->type()) {
    case 1002:
        emit aviFileSelected(item->getAviFile());
        emit titleSelected(item->getTitle());
        break;
    case 1001:
        emit titleSelected(item->getTitle());
        emit aviFileSelected(NULL);
        break;
    default:
        emit rootSelected(m_newDVD);
        emit aviFileSelected(NULL);
        break;
    }
}


void k9Import::execute() {
    QString filename;
    k9Config config;
    QString outputDir=config.getPrefOutput();
    if (Ui_import.cbOutputDev->currentIndex() ==0) {
//      changed QDir::homePath() to outputDir (bkp)
        filename=k9Dialogs::getSaveFileName (outputDir,"*.iso", 0,i18n("Save image to disk"));
        if (filename =="")
            return;
    } else if (Ui_import.cbOutputDev->currentIndex()==1) {
//      changed QDir::homePath() to outputDir (bkp)
        outputDir= k9Dialogs::getExistingDirectory(outputDir);
        if (outputDir=="")
            return;
    }
    k9ProcessList *p=new k9ProcessList(k9Dialogs::getMainWidget());

    p->setModal(true);
    p->show();
    m_newDVD->setProcessList(p);
    m_newDVD->setWorkDir(outputDir);
    m_newDVD->execute();
    p->close();
    bool burn=(m_newDVD->getError()=="") ;
    if (burn && Ui_import.cbOutputDev->currentIndex()!=1) {
        //changeStatusbar(i18n("Burning DVD"),sbMessage);

        k9BurnDVD b;

        b.setworkDir(config.getPrefOutput() +"/dvd");
        b.setUseK3b(config.getPrefK3b() );
        b.setAutoBurn(config.getPrefAutoBurn());
        b.setvolId("DVD");
        b.setSpeed( i18n("default"));
        if (Ui_import.cbOutputDev->currentIndex() !=0) {
            k9CdDrive * drive=(k9CdDrive*)recorderList.at(Ui_import.cbOutputDev->currentIndex()-2);
            b.setburnDevice(drive->device);
        } else
            b.makeIso(filename);
        b.burn();
    }
    //  m_parent->removeDockWidget(doc);
}


void k9Import::cbFormatActivated(const QString &_format) {
    if (_format == "PAL") {
        m_newDVD->setFormat(k9NewDVD::PAL);
        m_menuEdit->setFormat(k9MenuEdit::PAL);
    } else {
        m_newDVD->setFormat(k9NewDVD::NTSC);
        m_menuEdit->setFormat(k9MenuEdit::NTSC);
    }

}

void k9Import::setMenuEdit(k9MenuEdit* _value) {
    m_menuEdit = _value;
}

void k9Import::setEnableCreate(bool _state) {
//    m_parent->setEnabledCreateDVD(true);
}

void k9Import::titleAdded() {
    emit rootSelected(m_newDVD);
}

void k9Import::updateTotalTime() {
    int total=m_newDVD->getTotalTime();
    //Ui_import.gsTotal->setValue(total/60);
    QTime t(0,0,0);
    t=t.addSecs(total);
    Ui_import.lTotal->setText(t.toString("hh:mm:ss"));
}
