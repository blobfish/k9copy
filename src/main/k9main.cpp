/***************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   k9copy@free.fr                                                            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "k9main.h"
#include "k9common.h"
#include "k9dvdauthor.h"
#include "k9burndvd.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include "k9dvdtitleset.h"
#include "k9dvdbackup.h"
#include "k9mp4enc.h"
#include "k9settings.h"
#include "k9langselect.h"
#include "k9updatefactor.h"
#include "k9titleencopt.h"
#include "k9tools.h"
#include <qapplication.h>
#include <kurlrequester.h>
#include <qdir.h>
#include <qstatusbar.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qspinbox.h>
#include <qpainter.h>
#ifndef BACKLITE
#include <kdirselectdialog.h>
#include <kcombobox.h>
#include <kstandarddirs.h>
#include <KDiskFreeSpace>
#include <kmountpoint.h>
#include <kiconloader.h>
#include <kdeversion.h>
#endif
#include <qbitmap.h>
#include "images.h"
#include "k9execcopy.h"
#include <QMenu>
#include "k9dialogs.h"

ckLvItem::ckLvItem ( QTreeWidgetItem *parent,k9Main *dlg,eObjectType _objectType )
        : QTreeWidgetItem ( parent,1001 ) {
    m_checked=false;
    mainDlg=dlg;
    obj=NULL;
    stream=NULL;
    streamType=NONE;
    language="";
    objectType=_objectType;
    setFlags ( Qt::ItemIsUserCheckable |Qt::ItemIsEnabled  );
    setOn ( false );
}
ckLvItem::ckLvItem ( QTreeWidget *parent,k9Main *dlg,eObjectType _objectType )
        : QTreeWidgetItem ( parent,1001 ) {
    m_checked=false;
    mainDlg=dlg;
    obj=NULL;
    stream=NULL;
    streamType=NONE;
    language="";
    objectType=_objectType;
    setFlags ( Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
    setOn ( false );
    //QCheckBox *bx=new QCheckBox(NULL);
    //dlg->getTreeView()->setItemWidget ( this, 0, bx );

}


k9DVDListItem::k9DVDListItem ( void *DVD,ckLvItem *List,eStreamType type ) {
    switch ( type ) {
    case AUD :
        audioStream= ( k9DVDAudioStream* ) DVD;
        subtitle=NULL;
        title=NULL;
        break;
    case SUB :
        subtitle = ( k9DVDSubtitle* ) DVD;
        audioStream=NULL;
        title=NULL;
        break;
    case VID :
        subtitle=NULL;
        audioStream=NULL;
        title = ( k9DVDTitle* ) DVD;
        break;
    default :
        break;
    }
    listItem= List;
    streamType=type;
}

void k9Main::clearItems() {
    while ( !items.isEmpty() ) delete items.takeFirst();
}

k9Main::k9Main ( QWidget* parent, const char* , k9CdDrives *_drives )
        : QWidget ( parent ) {
    m_ready=false;
    Ui_MainDlg.setupUi ( this );
    listView1=Ui_MainDlg.listView1;
    connect ( Ui_MainDlg.cbOutputDev,SIGNAL ( currentIndexChanged ( int ) ),this,SLOT ( cbOutputDevActivated ( int ) ) );
    connect ( listView1,SIGNAL ( itemExpanded ( QTreeWidgetItem* ) ),this,SLOT ( expanded ( QTreeWidgetItem* ) ) );
    connect ( listView1,SIGNAL ( itemCollapsed ( QTreeWidgetItem* ) ),this,SLOT ( collapsed ( QTreeWidgetItem* ) ) );
    connect (listView1->header(),SIGNAL(customContextMenuRequested( const QPoint& )),this,SLOT(showPopup(const QPoint&)));

    listView1->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    drives=_drives;
    m_parent= ( k9Copy* ) parent;
    QImage img;
    img.loadFromData ( img_chapter, sizeof ( img_chapter ), "PNG" );
    pxChapter.fromImage ( img );
    /*    m_factory = KLibLoader::self()->factory("libk9copy");
    if (m_factory)      {
    dvd=static_cast<k9DVD  *>(m_factory->create(this,"dvd", "k9DVD"));
    }
    */

    dvd=new k9DVD ( this );

    updating=false;

//  items.setAutoDelete(true);
//  listView1->setDefaultRenameAction(QListView::Accept);
    //    KStandardDirs kd;
    m_prefOutput=k9Tools::getTempPath();
    connect ( drives,SIGNAL ( deviceAdded ( k9CdDrive* ) ),this,SLOT ( deviceAdded ( k9CdDrive* ) ) );
    connect ( drives,SIGNAL ( deviceRemoved ( k9CdDrive* ) ),this,SLOT ( deviceRemoved ( k9CdDrive* ) ) );

    readSettings();

    emit SelectionChanged ( NULL,true );
    m_update=new k9UpdateFactor ( this,"" );
    connect ( m_update,SIGNAL ( updateFactor_internal() ),this,SLOT ( updateFactor_internal() ) );
    cbOutputChanged(Ui_MainDlg.cbOutput->currentIndex());
    cbInputChanged(Ui_MainDlg.cbInput->currentIndex());
    
    Ui_MainDlg.cbInput->setItemIcon(iDVD,SmallIcon("media-optical"));
    Ui_MainDlg.cbInput->setItemIcon(iISO,SmallIcon("document-open"));
    Ui_MainDlg.cbInput->setItemIcon(iFolder,SmallIcon("document-open-folder"));

    Ui_MainDlg.cbOutput->setItemIcon(oDVD,SmallIcon("media-optical-recordable"));
    Ui_MainDlg.cbOutput->setItemIcon(oISO,SmallIcon("document-open"));
    Ui_MainDlg.cbOutput->setItemIcon(oMP4,SmallIcon("video-mp4"));
    Ui_MainDlg.cbOutput->setItemIcon(oMPEG2,SmallIcon("video-mpeg"));
    Ui_MainDlg.cbOutput->setItemIcon(oAudio,SmallIcon("audio-basic"));
    Ui_MainDlg.cbOutput->setItemIcon(oFolder,SmallIcon("document-open-folder"));

    m_ready=true;
}


k9DVDListItem *k9Main::addListItem ( void *DVD,ckLvItem *List,eStreamType type ) {
    k9DVDListItem *litem;
    litem=new k9DVDListItem ( DVD,List,type );
    items.append ( litem );
    return litem;
}
/*
int LvItem::() const
{
  return(1000);
}


int ckLvItem::rtti() const
{
  return(1001);
}
*/
bool ckLvItem::operator< ( const QTreeWidgetItem & other ) const {
    int col=treeWidget()->sortColumn();

    if ( other.type() !=1001 )
        return true;
    ckLvItem *litem = ( ckLvItem* ) &other;
    k9DVDTitleset *titleset1,*titleset2;
    ckLvItem *l;
    uint id,id2;
    if ( col==0 ) {
        switch ( this->objectType ) {
        case TITLESET:
            titleset1= ( k9DVDTitleset* ) obj;
            titleset2= ( k9DVDTitleset* ) litem->obj;
            return titleset1->getnum() >titleset2->getnum();
        case STREAM:
            l= ( ckLvItem* ) this;
            id=streamType*100;
            id2=litem->streamType*100;

            if ( streamType==AUD ) {
                id+= ( ( k9DVDAudioStream* ) l->stream )->getID();
            }
            if ( streamType==SUB ) {
                id+= ( ( k9DVDSubtitle* ) l->stream )->getID().first();
            }

            if ( litem->streamType==AUD ) {
                id2+= ( ( k9DVDAudioStream* ) litem->stream )->getID();
            }
            if ( litem->streamType==SUB ) {
                id2+= ( ( k9DVDSubtitle* ) litem->stream )->getID().first();
            }


            return k9Main::compare ( id,id2 );
            break;
        case CHAPTER: {
            l= ( ckLvItem* ) this;
            k9DVDChapter *ch1= ( k9DVDChapter* ) l->obj;
            k9DVDChapter *ch2= ( k9DVDChapter* ) litem->obj;
            return k9Main::compare ( ch1->getnum() +ch1->getTitle()->getId() *1000 ,ch2->getnum() +ch2->getTitle()->getId() *1000 );
        }
        break;
        default:
            return true;
        }
    }

    if ( col ==1 || col==2) {
        switch ( this->objectType ) {
        case TITLESET:
            titleset1= ( k9DVDTitleset* ) obj;
            titleset2= ( k9DVDTitleset* ) litem->obj;
            return k9Main::compare ( titleset1->getsize(),titleset2->getsize() );
            break;
        case STREAM:
            double size1,size2;
            l = ( ckLvItem* ) this;
            size1=l->getstreamSize();
            size2=litem->getstreamSize();
            return k9Main::compare ( size1,size2 );
            break;
        case CHAPTER: {
            l= ( ckLvItem* ) this;
            k9DVDChapter *ch1= ( k9DVDChapter* ) l->obj;
            k9DVDChapter *ch2= ( k9DVDChapter* ) litem->obj;
            return k9Main::compare ( ch1->getsectors(),ch2->getsectors() );
	    }
	    break;
	case TITLE : {
	      k9DVDTitle* title1= ( k9DVDTitle* ) obj;
	      k9DVDTitle* title2= ( k9DVDTitle* ) litem->obj;
	      return k9Main::compare(title1->gettotalsize_mb() , title2->gettotalsize_mb() );
	    }
	    break;
        default:
            return true;
        }
    }
    return  false;



}

double ckLvItem::getstreamSize() {
    double size;
    k9DVDTitle *title;
    switch ( streamType ) {
    case SUB: {
        k9DVDSubtitle *sub= ( k9DVDSubtitle* ) stream;
        size=sub->getsize_mb();
        break;
    }
    case AUD: {
        k9DVDAudioStream *aud= ( k9DVDAudioStream* ) stream;
        size=aud->getsize_mb();
        break;
    }
    case VID: {
        title= ( k9DVDTitle* ) obj;
        size=title->gettotalvideosize_mb();
        break;
    }
    default:
        size=0;
        break;
    }
    return size;
}


bool LvItem::operator< ( const QTreeWidgetItem & other ) const {
    int col=treeWidget()->sortColumn();

    if ( other.type() ==1001 )
        return false;
    LvItem *litem = ( LvItem* ) &other;

    k9DVDTitle *title1,*title2;
    if ( (col ==1 || col==2) && objectType==TITLE ) {
        title1= ( k9DVDTitle* ) obj;
        title2= ( k9DVDTitle* ) litem->obj;
        return k9Main::compare ( title1->gettotalsize_mb(),title2->gettotalsize_mb() );
    }
    if ( col==0 && objectType==TITLE ) {
        title1= ( k9DVDTitle* ) obj;
        title2= ( k9DVDTitle* ) litem->obj;
        return k9Main::compare ( title1->getpgc(),title2->getpgc() );
    }
  
    return false;
}


void k9Main::listView1ItemChanged ( QTreeWidgetItem *_item,int _col ) {
    if ( !dvd->getopened() )
        return;
    if ( _item->type() ==1001 && _col==0 ) {
        ckLvItem *it= ( ckLvItem* ) _item;
        it->stateChange ( it->isOn() );
    }

    if ( ( _item->flags() & Qt::ItemIsEditable ) ==Qt::ItemIsEditable )
        itemRenamed ( _item,_col );
}


void ckLvItem::stateChange ( bool state ) {
    if ( m_checked ==state )
        return;
    m_checked=state;

    if ( mainDlg !=NULL ) {
        if ( mainDlg->getupdating() )
            return;
        switch ( objectType ) {
        case ROOT :
            mainDlg->checkAll ( state );
            break;
        case TITLESET:
            mainDlg->checkTS ( state,this );
            break;
        default :
            mainDlg->checkTitle ( state,this );
            break;
        }
        //        k9DVD *d = mainDlg->dvd;
        mainDlg->setDVDSize();
        mainDlg->updateFactor();
        //(mainDlg->factor)->setValue((int) (d->getfactor(mainDlg->withMenus(),true )*100)-100 );
    }
}


bool k9Main::getupdating() {
    return updating;
}


void k9Main::Copy() {
    switch(Ui_MainDlg.cbOutput->currentIndex()) {
      case oMP4:
	CreateMP4();
	break;
      case oMPEG2:
	 extractMPEG2();
	 break;

      case oAudio:
	 extractAudio();
	 break;
      default:
	  k9ExecCopy execCopy;
	  execCopy.setDvd ( dvd );
	  QString path;
	  execCopy.setCopyMenus ( withMenus() );
	  execCopy.setSpeed ( Ui_MainDlg.cbBurnSpeed->currentText() );
	  if ( m_useDvdAuthor || !withMenus() )
	      m_playbackOptions->setSequence();

	  if ( Ui_MainDlg.cbOutput->currentIndex() ==oFolder) {
	      execCopy.setOutput ( k9ExecCopy::oFolder );
	      // changed from home to preferred output directory (bkp)
              path= Ui_MainDlg.urOutput->url().path();
	      if ( path=="" )
		  return;
	  } else if ( Ui_MainDlg.cbOutput->currentIndex() ==oISO ) {
	      // changed QDir::homePath() to m_prefOutput (bkp)
              path= Ui_MainDlg.urOutput->url().path();
	      if ( path =="" )
		  return;
	      execCopy.setOutput ( k9ExecCopy::oISO );
	  } else {
	      k9CdDrive * drive= ( k9CdDrive* ) recorderList.at ( Ui_MainDlg.cbOutputDev->currentIndex());
	      path=drive->device;
	      execCopy.setOutput ( k9ExecCopy::oDVD );

	  }
	  execCopy.setPath ( path );

	  execCopy.copyDVD();
	  break;
    }
    changeStatusbar ( i18n ( "Ready" ) ,sbMessage );

}



void k9Main::eject() {
    closeDVD();
    drives->eject ( dvd->getDevice() );
}



QString  k9Main::getDevice ( QComboBox *_combo ) {
    int index=-1;
    for ( int i=0; i<_combo->count();i++ ) {
        QString t =_combo->itemText ( i );
        if ( _combo->itemText ( i ) == _combo->currentText() )
            index=i;
    }
    QString res="";
    if ( ( index==-1 ) || ( _combo->currentText() ==i18n ( "ISO Image" ) ) ) {
        res=_combo->currentText();
    } else {
        k9CdDrive * drive= ( k9CdDrive* ) driveList.at ( index );
        res=drive->device;
    }
    return res;
}

void k9Main::Open() {
    listView1->setSortingEnabled ( false );
    
    int i;
    k9DVDTitle * l_track;
    QString t;
    //QTime h;

    listView1->disconnect ( SIGNAL ( itemChanged ( QTreeWidgetItem*,int ) ) );
    listView1->disconnect ( SIGNAL ( currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem * ) ) );

    disconnect ( m_langSelect->listView(),0,0,0 );


//  connect(listView1,SIGNAL(itemRenamed(QListViewItem*,int)),this,SLOT(itemRenamed(QListViewItem *,int)));

    closeDVD();
    listView1->clear();
    clearItems();
    tsItems.clear();
    chItems.clear();

    QString sDevice=Ui_MainDlg.cbInput->currentIndex()==iDVD ? getDevice ( Ui_MainDlg.cbInputDev ): Ui_MainDlg.urInput->url().path();

    QString sVolName="";
    //if no dvd label, try to get it from hal
#ifdef HAVE_HAL_H
    foreach ( k9CdDrive *drive,driveList ) {

        if ( drive->getDevice() != NULL ) {
            if ( drive->getDevice()->mountPoint() ==sDevice ) {
                sVolName=drive->getDevice()->getVolumeName();
                sDevice=drive->device;
                break;
            }
        }
    }

#endif
    k9Config config;
#ifndef BACKLITE
    if ( sDevice.startsWith ( "/dev/" ) && config.getPrefReadAhead() !=-1 )
        k9CdDrive::setReadAhead ( sDevice,config.getPrefReadAhead() );
#endif
    dvd->scandvd ( sDevice,m_quickScan );
    if ( dvd->geterror() ) {
        k9Dialogs::error ( dvd->geterrMsg(), i18n ( "Open DVD" ) );
        return;
    }
    
    m_parent->setDvdOpened(true);
    if ( dvd->getDVDTitle() ==i18n ( "unknown" ) && sVolName!="" ) {
        dvd->setDVDTitle ( sVolName );
    }

    root = new ckLvItem ( listView1,this,ROOT );
    root->setExpanded ( TRUE );

    root->setText ( 0, dvd->getDVDTitle() );
    root->setFlags ( root->flags() | Qt::ItemIsEditable );
    root->obj=NULL;
    root->streamType=NONE;

    root->setIcon ( 0,SmallIcon ( "media-optical" ) );

    for ( i=0;i<dvd->gettitlesetCount();i++ ) {
        ckLvItem *tsItem = new ckLvItem ( root,this,TITLESET );
        tsItem->setExpanded ( TRUE );
        QString c;
        c=i18n ( "Titleset %1").arg(i+1 );
        tsItem->setText ( 0,c );
        QFont font=tsItem->font ( 1 );
        font.setWeight ( QFont::Bold );
        tsItem->setFont ( 1,font );


        tsItem->setText ( 1,"   "+dvd->gettitleset ( i )->getsize_mb() +" " +i18n ( "MB" ) );
        tsItem->setTextAlignment ( 1,Qt::AlignRight );
        tsItem->obj=dvd->gettitleset ( i ) ;
        tsItem->streamType=NONE;
        // tsItem->setRenameEnabled(0,false);
        tsItems.append ( tsItem );

        tsItem->setIcon ( 0,SmallIcon ( "folder-open" ) );
    }

    for ( i=0;i<dvd->gettitleCount();i++ ) {
        l_track=dvd->gettitle ( i );
        if ( ( l_track->getIndexed() ) )
            addTitle ( l_track );
    }

    m_playbackOptions->fillTitleList();
    m_langSelect->fillLvLanguages();
    listView1->setSortingEnabled ( true );
    changeStatusbar ( i18n ( "Ready" ),sbMessage );
    emit SelectionChanged ( dvd,withMenus() );
    this->setEnabled ( true );

    connect ( listView1,SIGNAL ( itemChanged ( QTreeWidgetItem*,int ) ),this,SLOT ( listView1ItemChanged ( QTreeWidgetItem*,int ) ) );
    connect ( listView1,SIGNAL ( currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem * ) ),this,SLOT ( listView1CurrentChanged ( QTreeWidgetItem *,QTreeWidgetItem * ) ) );

    resizeColumns();
    //call to cbOutputChanged to update the filename
    cbOutputChanged(Ui_MainDlg.cbOutput->currentIndex());
}

void k9Main::resizeEvent ( QResizeEvent * ) {
    resizeColumns();
}

void k9Main::showEvent ( QShowEvent * )   {
    resizeColumns();
  
}

void k9Main::resizeColumns() {
    listView1->setColumnWidth ( 0,listView1->viewport()->width() );
    listView1->resizeColumnToContents ( 1 );
    listView1->resizeColumnToContents ( 2 );
    listView1->resizeColumnToContents ( 3 );
    listView1->setColumnWidth ( 0,listView1->viewport()->width()-listView1->columnWidth ( 1 )-listView1->columnWidth ( 2 ) -listView1->columnWidth ( 3 ));
}

void k9Main::setDVDSize() {
    //k9DVDSize *dvdsize=static_cast<k9DVDSize  *>(m_factory->create(this,"dvd", "k9DVDSize"));
    k9DVDSize *dvdsize=new k9DVDSize ( this );
    dvdsize->setMaxSizeDyn ( m_prefSize );
    delete dvdsize;
}


k9Main::~k9Main() {
    clearItems();
    delete dvd;
}


void k9Main::addChapters ( QTreeWidgetItem *_parent,k9DVDTitle *_title ) {
    LvItem *chapter = new LvItem ( _parent,CHAPTER );
    chapter->setText ( 0, i18n ( "chapters" ) );
    chapter->setExpanded ( false );
    chapter->setIcon ( 0,pxChapter );

    int ch=0;
    for ( int i=0;i< _title->getchapterCount();i++ ) {
        ckLvItem *it =new ckLvItem ( chapter,this,CHAPTER );
        it->setText ( 0,i18n ( "chapter %1").arg(++ch ) );
        QString s;

        s=QString ( "%1" ).arg ( ( double ) ( _title->getChapter ( i )->getsectors() ) /512,0,'f',2 );
        it->setText ( 1,i18n ( "%1 MB").arg(s ) );
        it->setTextAlignment ( 1,Qt::AlignRight | Qt::AlignVCenter);
        it->setTextAlignment ( 2,Qt::AlignRight | Qt::AlignVCenter);

	it->setText(2,_title->getChapter(i)->getLength().toString("h:mm:ss"));
        it->streamType=CHAP;
        it->obj=_title->getChapter ( i );
        chItems.append ( it );
        it->mainTitle=_title;
    }
    for ( int j=0;j <_title->getTitles().count();j++ ) {
        k9DVDTitle *title2=_title->getTitles().at ( j );
        for ( int i=0;i< title2->getchapterCount();i++ ) {
            ckLvItem *it =new ckLvItem ( chapter,this,CHAPTER );
            it->setText ( 0,i18n ( "chapter %1").arg(++ch ) );
            it->streamType=CHAP;
            it->obj=title2->getChapter ( i );
            QString s;
            s=QString ( "%1" ).arg ( ( double ) ( title2->getChapter ( i )->getsectors() ) /512,0,'f',2 );

            it->setText ( 1,i18n ( "%1 MB").arg(s ) );
            it->setTextAlignment ( 1,Qt::AlignRight | Qt::AlignVCenter );
            it->setTextAlignment ( 2,Qt::AlignRight | Qt::AlignVCenter );

	    it->setText(2,title2->getChapter(i)->getLength().toString("h:mm:ss"));

            chItems.append ( it );
            it->mainTitle=_title;
        }

    }

}
/** No descriptions */
void k9Main::addTitle ( k9DVDTitle *track ) {
    const int col1 =0;
    const int col2 =1;
    const int col3 =3;
    const int col4= 2;
    k9DVDAudioStream *l_auds;
    k9DVDSubtitle *l_sub;
    int i;
    QString c,ch;


    listView1->setRootIsDecorated ( true );

    LvItem * itemTrack = new LvItem ( tsItems.at ( track->getVTS()-1 ),TITLE );
    itemTrack->setExpanded ( false );
    itemTrack->setText ( col1,track->getname() );
    itemTrack->setFlags ( itemTrack->flags() | Qt::ItemIsEditable );

    itemTrack->setIcon ( col1,SmallIcon ( "title" ) );
    c.sprintf ( "%.2f ", track->gettotalsize_mb() );

    itemTrack->setText ( col2,c+i18n ( "MB" ) );
    itemTrack->setText (col4,track->gettotallength().toString ( "h:mm:ss" ));
    itemTrack->setTextAlignment ( col2,Qt::AlignRight|Qt::AlignVCenter );
    itemTrack->setTextAlignment ( col4,Qt::AlignRight|Qt::AlignVCenter );

    itemTrack->obj=track;

   

    addChapters ( itemTrack,track );

    ckLvItem *video;
    video=new ckLvItem ( itemTrack,this,STREAM );
    video->streamType=VID;

    video->setIcon ( 0,SmallIcon ( "video" ) );
    addListItem ( track,video,VID );
    video->setExpanded ( false );
    c=i18n ( "video %1 ").arg(track->getformat() );
    c.append ( " - " + track->getaspectRatio() );
     
    video->setText ( col1, c );
    if ( track->gettotalvideosize_mb() ) {
        c.sprintf ( "%.2f ",  track->gettotalvideosize_mb() );
        video->setText ( col2,c +i18n ( "MB" ) );
        video->setTextAlignment ( col2,Qt::AlignRight );
    }
    video->obj=track;

    for ( i=0;i< track->getaudioStreamCount();i++ ) {
        l_auds=track->getaudioStream ( i );
        c=i18n ( "audio %1 ").arg(i+1 );
        c.append ( l_auds->getlanguage() + " " +l_auds->getformat() +" " );
        ch.sprintf ( "%dch ",l_auds->getchannels() );
        c.append ( ch+l_auds->getfrequency() +" "+l_auds->getquantization() );
        ckLvItem * item = new ckLvItem ( itemTrack,this,STREAM );

        item->setIcon ( 0,SmallIcon ( "sound" ) );

        item->streamType=AUD;
        item->language=l_auds->getlanguage();
        addListItem ( l_auds,item,AUD );
        item->setText ( col1,  c );
        if ( l_auds->getsize_mb() ) {
            c.sprintf ( "%.2f ",  l_auds->getsize_mb());
            item->setText ( col2,c +i18n ( "MB" ) );
            item->setTextAlignment ( col2,Qt::AlignRight );
        }
        item->setText ( col3,l_auds->getcontent() );

        item->obj=track;
        item->stream=l_auds;
    }
    for ( i=0;i< track->getsubPictureCount();i++ ) {
        l_sub=track->getsubtitle ( i );
        c=i18n ( "subpicture %1 ").arg(i+1 );
        c.append ( l_sub->getlanguage() );
        ckLvItem * item = new ckLvItem ( itemTrack,this,STREAM );

        item->setIcon ( 0,SmallIcon ( "subtitle" ) );
        item->streamType=SUB;
        item->language=l_sub->getlanguage();
        addListItem ( l_sub,item,SUB );
        item->setText ( col1,  c );
        if ( l_sub->getsize_mb() ) {
            c.sprintf ( "%.2f ",  l_sub->getsize_mb());
            item->setText ( col2,c +i18n ( "MB" ) );
            item->setTextAlignment ( col2,Qt::AlignRight );
        }
        item->setText ( col3, l_sub->getcontent() );

        item->obj=track;
        item->stream=l_sub;
    }
}


/** No descriptions */
void k9Main::updateSelection() {
    QMap <int,int> mAudio;
    QMap <int,int> mSubp;

    k9DVDListItem *litem;
    for ( int i=0;i<items.count();i++ ) {
        litem= ( k9DVDListItem* ) items.at ( i );

	k9DVDTitle *title;
	switch (litem->streamType) {
	  case AUD:
	      title=litem->audioStream->gettitle();
	      break;
	  case SUB:
	      title=litem->subtitle->gettitle();
	      break;
	  case VID:
	      title=litem->title;
	      break;
          default:
              title=NULL;
              break;
	}
	bool singleAudio=(Ui_MainDlg.cbOutput->currentIndex()==oMP4) && (title->getEncOpt()->getEncoder()=="mencoder");
	bool singleSubp=(Ui_MainDlg.cbOutput->currentIndex()==oMP4) && (title->getEncOpt()->getEncoder()=="mencoder");
        bool noSubp=(Ui_MainDlg.cbOutput->currentIndex()==oMP4) && (title->getEncOpt()->getEncoder()=="ffmpeg") && (!Ui_MainDlg.urOutput->url().path().endsWith(".mkv"));
	
        switch ( litem->streamType ) {
        case SUB:
	    
            k9DVDSubtitle *l_sub;
            l_sub=litem->subtitle;
	    if (singleSubp && mSubp.contains(title->getnumTitle()))
	          litem->listItem->setOn(false);
	    if (noSubp) 
		  litem->listItem->setOn(false);
	    if (litem->listItem->isOn())
	      mSubp[title->getnumTitle()]=1;

	    l_sub->setselected ( litem->listItem->isOn() );
            break;
        case AUD:
            k9DVDAudioStream *l_auds;
            l_auds=litem->audioStream;
	    if (singleAudio && mAudio.contains(title->getnumTitle()))
	      litem->listItem->setOn(false);
	    if (litem->listItem->isOn())
	      mAudio[title->getnumTitle()]=1;
            l_auds->setselected ( litem->listItem->isOn() );
            break;
        case VID :
            k9DVDTitle *l_title;
            l_title=litem->title;

            l_title->setvideoSelected ( litem->listItem->isOn() );
            //if title is not selected => shrink factor is not forced
            if ( !l_title->isSelected() )
                l_title->setforceFactor ( false );

            break;
        default :
            break;
        }
    }
    bool supdating=updating;
    updating=true;
    for ( int i=0;i<chItems.count();i++ ) {
        ckLvItem *it=chItems.at ( i );
        k9DVDChapter *c= ( k9DVDChapter* ) it->obj;
        it->setOn ( c->getSelected() );
    }
    updating=supdating;
}

void k9Main::checkLang ( QString lang,eStreamType streamType,bool state ) {
    updating =true;
    for ( int i=0;i<items.count();i++ ) {
        bool checked=false;
        k9DVDListItem *litem= ( k9DVDListItem* ) items.at ( i );
        if ( litem->streamType == streamType ) {
            switch ( litem->streamType ) {
            case SUB:
                k9DVDSubtitle *l_sub;
                l_sub=litem->subtitle;
                if ( l_sub->getlanguage() == lang ) {
                    l_sub->setselected ( state );
                    litem->listItem->setOn ( state );
                    checked=state;

                    k9DVDTitle *title= ( k9DVDTitle* ) litem->listItem->obj;
                    ckLvItem * itemtitleset= ( ckLvItem* ) litem -> listItem->parent()->parent();
                    itemtitleset->setOn ( title->gettitleset()->getselected() );
                }
                break;
            case AUD:
                k9DVDAudioStream *l_auds;
                l_auds=litem->audioStream;
                if ( l_auds->getlanguage() == lang ) {
                    l_auds->setselected ( state );
                    litem->listItem->setOn ( state );
                    checked=state;

                    k9DVDTitle *title= ( k9DVDTitle* ) litem->listItem->obj;
                    ckLvItem * itemtitleset= ( ckLvItem* ) litem -> listItem->parent()->parent();
                    itemtitleset->setOn ( title->gettitleset()->getselected() );
                }
                break;
            default:
                break;
            }
            //we must check the video item
            if ( checked ) {
                for ( int j=0; j<items.count();j++ ) {
                    k9DVDListItem *litem2= ( k9DVDListItem* ) items.at ( j );
                    if ( litem2->listItem->parent() ==litem->listItem->parent() && litem2->streamType==VID ) {
                        litem2->title->setvideoSelected ( state );
                        litem2->listItem->setOn ( state );
                    }
                }
            }
        }
    }
    m_playbackOptions->fillTitleList();
    updating=false;
    emit SelectionChanged ( dvd,withMenus() );

}

void k9Main::checkTitle ( bool state, ckLvItem *_item ) {
    m_update->wait();

    k9DVDListItem *litem;
    updating=true;

    k9DVDTitle *title;
    if ( _item->streamType==CHAP ) {
        k9DVDChapter *c= ( k9DVDChapter* ) _item->obj;
        c->setSelected ( state );
        //title=c->getTitle();
        title=_item->mainTitle;
        for ( int i=0;i<items.count() && state ;i++ ) {
            litem= ( k9DVDListItem* ) items.at ( i );
            if ( title==litem->title && litem->streamType==VID )
                litem->listItem->setOn ( state );

        }
    } else
        title= ( k9DVDTitle* ) _item->obj;
   
    bool singleAudio=(Ui_MainDlg.cbOutput->currentIndex()==oMP4) && (title->getEncOpt()->getEncoder()=="mencoder");
    bool singleSubp=(Ui_MainDlg.cbOutput->currentIndex()==oMP4) && (title->getEncOpt()->getEncoder()=="mencoder");
    //bool noSubp=(Ui_MainDlg.cbOutput->currentIndex()==oMP4) && (title->getEncOpt()->getEncoder()=="ffmpeg") && (!Ui_MainDlg.urOutput->url().fileName().endsWith(".mkv"));

    
    for ( int i=0;i<items.count();i++ ) {
        litem= ( k9DVDListItem* ) items.at ( i );
        if ( litem->listItem->parent() == _item->parent() ) {
            //if current item is video and is unchecked, uncheck audio and subtitles
            if ( !state && _item->streamType==VID ) {
                switch ( litem->streamType ) {
                case SUB:
                    k9DVDSubtitle *l_sub;
                    l_sub=litem->subtitle;
                    litem->listItem->setOn (state);
                    l_sub->setselected ( state);
                    break;

                case AUD:
                    k9DVDAudioStream *l_auds;
                    l_auds=litem->audioStream;
                    litem->listItem->setOn ( state);
                    l_auds->setselected ( state);
                    break;
                default:
                    break;
                }
            } else if ( litem->streamType == VID && state && ( _item->streamType==SUB || _item->streamType==AUD ) ) {
                litem->title->setvideoSelected ( state );
                litem->listItem->setOn ( state );
	    } else if ( litem->streamType == AUD && state && _item->streamType==AUD && litem->listItem !=_item) {
	        if (singleAudio) {
		  k9DVDAudioStream *l_auds;
                  l_auds=litem->audioStream;
                  litem->listItem->setOn ( !state);
                  l_auds->setselected ( !state);
		}
	    } else if ( litem->streamType == SUB && state && _item->streamType==SUB && litem->listItem !=_item) {
	        if (singleSubp) {
		  k9DVDSubtitle *l_subp;
                  l_subp=litem->subtitle;
                  litem->listItem->setOn ( !state);
                  l_subp->setselected ( !state);
		}
	    }
        }
    }


    updateSelection();

    k9DVDTitleset * titleset=NULL;

    titleset=title->gettitleset();

    m_langSelect->update();

    //check the titleset checkbox
    if ( titleset!=NULL ) {
        ckLvItem * itemtitleset;
        if ( _item->streamType==CHAP )
            itemtitleset= ( ckLvItem* ) _item->parent()->parent()->parent();
        else
            itemtitleset= ( ckLvItem* ) _item->parent()->parent();
        itemtitleset->setOn ( titleset->getselected() );
    }

    m_playbackOptions->fillTitleList();
    updating=false;
    emit SelectionChanged ( dvd,withMenus() );
}



void k9Main::checkAll ( bool state ) {
    k9DVDListItem *litem;
    updating=true;
    k9Config config;
    for ( int i=0;i<items.count();i++ ) {
        litem= ( k9DVDListItem* ) items.at ( i );
        switch ( litem->streamType ) {
        case SUB:
            if ( state )
                litem->listItem->setOn (config.getPrefSubtitleLang().contains(litem->subtitle->getlangCod()) || (config.getPrefSubtitleLang().count()==0));
            else
                litem->listItem->setOn ( state );
            break;
        case AUD:
            if ( state )
                litem->listItem->setOn ( config.getPrefAudioLang().contains( litem->audioStream->getlangCod() ) || (config.getPrefAudioLang().count()==0));
            else
                litem->listItem->setOn ( state );
            break;
        case VID: {
            k9DVDTitle * l_title;
            l_title=litem->title;
            //            l_title->setforceSelection(state);
            litem->listItem->setOn ( state );

            //            k9DVDTitle *title=(k9DVDTitle*)litem->listItem->obj;
            ckLvItem * itemtitleset= ( ckLvItem* ) litem -> listItem->parent()->parent();
            l_title->gettitleset()->setselected ( state );
            itemtitleset->setOn ( l_title->gettitleset()->getselected() );
        }
        break;
        default:
            break;
        }
    }

    //m_langSelect->selectAll(state);
    updateSelection();
    m_langSelect->update();

    m_playbackOptions->fillTitleList();
    updating=false;
    emit SelectionChanged ( dvd,withMenus() );

}

void k9Main::checkTS ( bool _state,ckLvItem *_item ) {
    k9DVDTitleset *titleset = ( k9DVDTitleset* ) _item->obj;
    titleset->setselected ( _state );
    k9DVDListItem *litem;
    k9Config config;
    updating=true;
    for ( int i=0;i<items.count();i++ ) {
        litem= ( k9DVDListItem* ) items.at ( i );
        if ( litem->listItem->parent ()->parent() == _item ) {
            switch ( litem->streamType ) {
            case SUB:
                if ( _state )
                    litem->listItem->setOn ( config.getPrefSubtitleLang().contains( litem->subtitle->getlangCod()) ||  (config.getPrefSubtitleLang().count()==0));
                else
                    litem->listItem->setOn ( _state );
                break;
            case AUD:
                if ( _state )
                    litem->listItem->setOn ( config.getPrefAudioLang().contains( litem->audioStream->getlangCod()) || (config.getPrefAudioLang().count()==0));
                else
                    litem->listItem->setOn ( _state );
                break;
            default :
                litem->listItem->setOn ( _state );
                break;
            }
        }
    }
    m_langSelect->update();

    updateSelection();

    m_playbackOptions->fillTitleList();
    updating=false;
    emit SelectionChanged ( dvd,withMenus() );
}

void k9Main::expanded ( QTreeWidgetItem *item ) {
    if ( item->type() ==1001 ) {
        ckLvItem *ckit = ( ckLvItem* ) item;
        if ( ckit->objectType==TITLESET )
            ckit->setIcon ( 0,SmallIcon ( "folder-open" ) );
    }
    //  listView1->resizeColumnToContents(0);
    resizeColumns();

}

void k9Main::collapsed ( QTreeWidgetItem *item ) {
    if ( item->type() ==1001 ) {
        ckLvItem *ckit = ( ckLvItem* ) item;
        if ( ckit->objectType==TITLESET )

            ckit->setIcon ( 0,SmallIcon ( "folder" ) );
    }
    resizeColumns();
}


/** No descriptions */
void k9Main::itemRenamed ( QTreeWidgetItem * item,int ) {
    k9DVDTitle *t=NULL;
    QString newText;
    if ( item->type() ==1000 ) {
        LvItem *it = ( LvItem* ) item;
        t= ( k9DVDTitle* ) it->obj;
        newText=it->text ( 0 );
    } else if ( item->type() ==1001 ) {
        ckLvItem *ckit = ( ckLvItem* ) item;
        t= ( k9DVDTitle* ) ckit->obj;
        newText=ckit->text ( 0 );
        if ( !ckit->renamed ( newText ) )
            return;
    }

    QString c;
    if ( t !=NULL ) {
        //QMessageBox::critical( this, "test", c.sprintf("%d",it->tag));
        t->setname ( newText );
        m_playbackOptions->titleRenamed ( t,newText );
    } else
        dvd->setDVDTitle ( item->text ( 0 ) );
}


/** No descriptions */
void k9Main::readSettings() {
    readDrives();
    k9Config config;
    config.read();

    m_prefOutput=config.getPrefOutput();
    
    if (config.getInputDev() <100) {
      Ui_MainDlg.cbInputDev->setCurrentIndex (config.getInputDev()  );
      Ui_MainDlg.cbInput->setCurrentIndex (iDVD  );
    } else
      Ui_MainDlg.cbInput->setCurrentIndex (config.getInputDev() -100  );
    
    if (config.getOutputDev() <100 ) {
      Ui_MainDlg.cbOutputDev->setCurrentIndex ( config.getOutputDev() );
      Ui_MainDlg.cbOutput->setCurrentIndex ( oDVD );
    } else
      Ui_MainDlg.cbOutput->setCurrentIndex ( config.getOutputDev() - 100);
    m_prefK3b=config.getPrefK3b();

    m_prefAutoBurn=config.getPrefAutoBurn();
    m_quickScan=config.getQuickScan();
    m_prefSize=config.getPrefSize();
    //fill the burn speed combo
    //cbOutputDevActivated( cbOutputDev->currentItem());
    cbOutputDevActivated ( config.getOutputDev() );


    //m_useDvdAuthor=settings.readBoolEntry("/options/useDvdAuthor",true);
    m_useDvdAuthor=config.getUseDvdAuthor();

    for (int i=0; i<config.getPrefColumns().count();i++) {
      listView1->setColumnHidden(i,!config.getPrefColumns().at(i));
    }
}
/** No descriptions */
void k9Main::saveSettings() {
    k9Config config;
    config.read();

    int inputDev=Ui_MainDlg.cbInput->currentIndex() ;
    inputDev= inputDev==iDVD?Ui_MainDlg.cbInput->currentIndex(): inputDev+100;
    int outputDev=Ui_MainDlg.cbOutput->currentIndex() ;
    outputDev= outputDev==oDVD?Ui_MainDlg.cbOutputDev->currentIndex(): outputDev+100;
    
    config.setInputDev ( inputDev);
    config.setOutputDev ( outputDev );
    config.setKeepMenus ( withMenus() );

    QList <bool> cols;
    for (int i=0;i <listView1->columnCount();i++) {
       cols << !listView1->isColumnHidden(i);
    }
    config.setPrefColumns(cols);
    config.save();
}
/** No descriptions */
void k9Main::bSaveClick() {
    saveSettings();
}

/** No descriptions */
void k9Main::PreviewTitle() {
    if ( listView1->currentItem() ==NULL )
        return;
    if ( listView1->currentItem()->type() ==0 )
        return;
    int rtti=listView1->currentItem()->type();
    void *obj;
    if ( rtti==1000 ) {
        LvItem *it= ( LvItem* ) listView1->currentItem();
        obj=it->obj;
    } else {
        ckLvItem *it= ( ckLvItem* ) listView1->currentItem();
        if ( it->objectType ==TITLESET )
            return;
        obj=it->obj;
    }

    if ( obj !=NULL ) {
        k9DVDTitle *t;
        ckLvItem *it= ( ckLvItem* ) listView1->currentItem();
        int chapter=1;
        if ( it->objectType==CHAPTER ) {
            k9DVDChapter *c= ( k9DVDChapter* ) obj;
            t=c->getTitle();
            chapter=c->getnum();
        } else
            t= ( k9DVDTitle* ) obj;
        //viewer->show();
        //viewer->open(dvd,t);


        emit showPreview ( dvd,t,chapter );
    }
}

void k9Main::CreateMP4() {
    k9ExecCopy execCopy;
    execCopy.setDvd ( dvd );
    execCopy.setPath(Ui_MainDlg.urOutput->url().path());
    execCopy.CreateMP4();
    changeStatusbar ( i18n ( "Ready" ) ,sbMessage );

}

void k9Main::extractAudio() {
    k9ExecCopy execCopy;
    execCopy.setDvd ( dvd );
    execCopy.setPath(Ui_MainDlg.urOutput->url().path());
    execCopy.extractAudio();
    changeStatusbar ( i18n ( "Ready" ) ,sbMessage );

}


void k9Main::extractMPEG2() {
    k9ExecCopy execCopy;
    execCopy.setDvd ( dvd );
    execCopy.setPath(Ui_MainDlg.urOutput->url().path());
    execCopy.extractMPEG2();
    changeStatusbar ( i18n ( "Ready" ) ,sbMessage );


}


void k9Main::deviceAdded ( k9CdDrive *_drive ) {
    addDrive ( _drive );
}

void k9Main::deviceRemoved ( k9CdDrive *_drive ) {
    if ( _drive->device == dvd->getDevice() )
        closeDVD();

    int i=driveList.indexOf ( _drive );
    driveList.removeAt ( i );
    Ui_MainDlg.cbInputDev->removeItem ( i );

    i=recorderList.indexOf ( _drive );
    recorderList.removeAt ( i );
    Ui_MainDlg.cbOutputDev->removeItem ( i );

}

void k9Main::addDrive ( k9CdDrive *drive ) {
#ifdef HAVE_HAL_H
    if ( drive->getDevice() !=NULL ) {
        k9HalDevice *dev=drive->getDevice();
        connect ( dev,SIGNAL ( volumeChanged ( const QString &,const QString & ) ),this,SLOT ( volumeChanged ( const QString&,const QString& ) ) );
    }
#else
    connect ( drive,SIGNAL ( volumeChanged ( const QString &,const QString & ) ),this,SLOT ( volumeChanged ( const QString&,const QString& ) ) );

#endif
    if ( drive->canReadDVD ) {
        QString c ( drive->name + "  ("+drive->device+")" );
        Ui_MainDlg.cbInputDev->addItem ( c,drive->device );
        driveList.append ( drive );
    }
    if ( drive->canWriteDVD ) {
        Ui_MainDlg.cbOutputDev->addItem ( drive->name+ "  ("+drive->device+")",drive->device );
        recorderList.append ( drive );
    }

}

/*!
    \fn k9Main::readDrives()
 */
void k9Main::readDrives() {
    bool ready=m_ready;
    m_ready=false;
    Ui_MainDlg.cbInputDev->clear();
    Ui_MainDlg.cbOutputDev->clear();
    driveList.clear();
    recorderList.clear();
    drives->scanDrives();
    m_ready=ready;
}


void k9Main::volumeChanged ( const QString &device,const QString &volumeName ) {
    if (!m_ready)
        return;
    if ( device==dvd->getDevice() && dvd->getopened() && volumeName=="" )
        closeDVD();
    else if ( !dvd->getopened() && volumeName !="" && getDevice ( Ui_MainDlg.cbInputDev ) ==device && Ui_MainDlg.cbInput->currentIndex()==iDVD) {
        Open();
    }
}

/*!
    \fn k9Main::listView1CurrentChanged( QListViewItem *newItem )
 */
void k9Main::listView1CurrentChanged ( QTreeWidgetItem *newItem,QTreeWidgetItem* ) {
    if ( newItem == NULL ) {
        m_playbackOptions->enable ( false );
        return;
    } else {
        if ( !withMenus() ) {
            m_playbackOptions->enable ( true );
        }
    }

    k9DVDTitle *title=NULL;

    if ( newItem->type() ==1000 ) {
        LvItem *it = ( LvItem* ) newItem;
        title= ( k9DVDTitle* ) it->obj;
    } else if ( newItem->type() ==1001 ) {
        ckLvItem *ckit = ( ckLvItem* ) newItem;
        if ( ckit->objectType!=TITLESET ) {
            if ( ( ( ckLvItem* ) newItem )->streamType==CHAP ) {
                LvItem *parent= ( LvItem* ) ( ckit->parent()->parent() );
                title= ( k9DVDTitle* ) parent->obj;
            } else
                title= ( k9DVDTitle* ) ckit->obj;
        }
    }
    if ( title !=NULL ) {
        emit changedTitle ( title );
        PreviewTitle();
    }

}


void k9Main::closeDVD() {
    //viewer->bStopClick();
    emit stopPreview();
    emit changedTitle ( NULL );
    emit SelectionChanged ( NULL,withMenus() );
    changeStatusbar ( "",sbFactor );
    changeStatusbar ("",sbSelSize);
    changeStatusbar ( "",sbMessage );
    listView1->clear();
    clearItems();
    m_langSelect->clear();
    dvd->close();
    m_parent->setDvdOpened(false);
}


bool k9Main::compare ( double v1,double v2 ) {
    if ( v1 >v2 )
        return true;
    if ( v1<v2 )
        return false;
    return true;
}


void k9Main::cbOutputDevActivated ( int _index ) {
    if (_index >= recorderList.count() || _index <0)
        return;
    k9CdDrive * drive= ( k9CdDrive* ) recorderList.at ( _index);
    if ( drive !=NULL ) {
        QList <int>list=drive->getWriteSpeeds();
        Ui_MainDlg.cbBurnSpeed->clear();
        Ui_MainDlg.cbBurnSpeed->addItem ( i18n ( "default" ) );
        foreach ( int speed,list ) {
            Ui_MainDlg.cbBurnSpeed->addItem ( QString::number ( ( double ) ( speed ) /1385 ) );
        }
#ifdef BACKLITE
        Ui_MainDlg.urOutput->setMode(KUrlRequester::DIRECTORY);
#else
        Ui_MainDlg.urOutput->setMode(KFile::Directory);
#endif
        Ui_MainDlg.urOutput->setUrl(KUrl(drive->device));

    }
}


void k9Main::bInputOpenClick() {
    QString result=k9Dialogs::getOpenFileName ( QDir::homePath(),"*.iso", 0,i18n ( "Open ISO Image" ) );
    if ( result!="" ) {
        k9Tools::setComboText ( Ui_MainDlg.cbInputDev,result );
        Open();
    }
}


void k9Main::bInputOpenDirClick() {
    QString result= k9Dialogs::getExistingDirectory ( QDir::homePath() );
//KDirSelectDialog::selectDirectory (QDir::homePath(), false, this,i18n("Open DVD folder")).path();
    if ( result!="" ) {
        k9Tools::setComboText ( Ui_MainDlg.cbInputDev,result );
        Open();
    }
}


long k9Main::getFreeSpace ( const QString & _path ) {
    long fs=k9Tools::getFreeSpace ( _path );
    if ( fs!=-1 ) {
        return fs;
    } else
        return -1;


}


void k9Main::setInput ( QString _input ) {
    bool found=false;
    for ( int i=0;i< driveList.count();i++ ) {
        k9CdDrive *drive= ( k9CdDrive* ) driveList.at ( i );
        if ( drive->device == _input ) {
            Ui_MainDlg.cbInputDev->setCurrentIndex ( i );
	    Ui_MainDlg.cbInput->setCurrentIndex(iDVD);
            found=true;
        }
    }
    if ( !found ) {
        Ui_MainDlg.urInput->setUrl(KUrl(_input) );
	Ui_MainDlg.cbInput->setCurrentIndex(iFolder);
    }
}

void k9Main::setOutput ( QString _output ) {
    Ui_MainDlg.cbOutputDev->setCurrentIndex ( 0 );
    for ( int i=0 ;i <recorderList.count();i++ ) {
        k9CdDrive * drive= ( k9CdDrive* ) recorderList.at ( i );
        QString c ( drive->device );
        //        qDebug ("output=" +c);
        if ( c==_output ) {
            Ui_MainDlg.cbOutputDev->setCurrentIndex ( i );
            break;
        }
    }
}

void k9Main::Clone ( QString _input,QString _output ) {
    setInput ( _input );
    setOutput ( _output );
    m_playbackOptions->setwithMenus ( true );
    m_quickScan=true;
    Open();
    checkAll ( true );
    Copy();
}

void k9Main::updateFactor() {

// m_update->updateFactor();
    updateFactor_internal();
}

void k9Main::updateFactor_internal() {
    if ( dvd->getopened() && m_mutex.tryLock() ) {
        updateSelection();
        setDVDSize();
        double dbfactor=dvd->getfactor ( withMenus(),true,m_useDvdAuthor );
	

//    Ui_MainDlg.factor->setValue((int) ( dbfactor*100)-100 );
        uint64_t sizeSelected=dvd->getsizeSelected ( FALSE );
        if ( sizeSelected ==0 ) {
            changeStatusbar ( "",sbFactor );
	    changeStatusbar ( "",sbSelSize );
	}
        else {
            changeStatusbar ( QString::number ( dbfactor,'f',2 ),sbFactor );
            changeStatusbar ( i18n("%1 MB").arg(QString::number ((double)sizeSelected/512.,'f',2)),sbSelSize);
	}
        emit SelectionChanged ( dvd,withMenus() );
        m_mutex.unlock();
    }
}

bool k9Main::withMenus() {
    return m_playbackOptions->withMenus();
}


void k9Main::setDrives ( k9CdDrives* _value ) {
    drives = _value;
}

void k9Main::cbInputChanged(int _index)
{
   switch (_index) {
     case iDVD: //DVD
	Ui_MainDlg.cbInputDev->show();
        Ui_MainDlg.urInput->hide();
      break;
     case iISO: //Iso Image
	Ui_MainDlg.cbInputDev->hide();
        Ui_MainDlg.urInput->show();
#ifdef BACKLITE
        Ui_MainDlg.urInput->setMode(KUrlRequester::OPEN);
        Ui_MainDlg.urInput->setFilter("Iso Image (*.iso)");
#else
        Ui_MainDlg.urInput->setMode(KFile::ExistingOnly | KFile::File);
        Ui_MainDlg.urInput->fileDialog()->setOperationMode(KFileDialog::Opening);
        Ui_MainDlg.urInput->setFilter("*.iso|Iso Image (*.iso)");
#endif

      break;
     case iFolder: //Folder
	Ui_MainDlg.cbInputDev->hide();
        Ui_MainDlg.urInput->show();
#ifdef BACKLITE
        Ui_MainDlg.urInput->setMode(KUrlRequester::DIRECTORY);
#else
        Ui_MainDlg.urInput->setMode(KFile::ExistingOnly | KFile::Directory);
        Ui_MainDlg.urInput->fileDialog()->setOperationMode(KFileDialog::Opening);
#endif
      break;
   }
}

void k9Main::cbOutputChanged(int _index)
{
  k9Config config;
   QString title=dvd->getDVDTitle();
   title= title.isEmpty() ? i18n("unknown"):title;
   m_parent->updateDock(_index);
   switch (_index) {
     case oDVD:
	Ui_MainDlg.wOutput->show();
        Ui_MainDlg.urOutput->hide();
	
      break;
     case oISO:
	Ui_MainDlg.wOutput->hide();
        Ui_MainDlg.urOutput->show();
#ifdef BACKLITE
        Ui_MainDlg.urOutput->setMode(KUrlRequester::SAVE);
        Ui_MainDlg.urOutput->setFilter("Iso Image (*.iso)");
#else
        Ui_MainDlg.urOutput->setMode( KFile::File);
        Ui_MainDlg.urOutput->fileDialog()->setOperationMode(KFileDialog::Saving);
        Ui_MainDlg.urOutput->setFilter("*.iso|Iso Image (*.iso)");
#endif
        Ui_MainDlg.urOutput->setUrl(KUrl(QDir::cleanPath(config.getPrefOutputIso()+"/"+title+".iso") ));
      break;
     case oFolder:
     case oAudio:
	Ui_MainDlg.wOutput->hide();
        Ui_MainDlg.urOutput->show();
#ifdef BACKLITE
        Ui_MainDlg.urOutput->setMode( KUrlRequester::DIRECTORY);
#else
        Ui_MainDlg.urOutput->setMode( KFile::Directory);
        Ui_MainDlg.urOutput->fileDialog()->setOperationMode(KFileDialog::Saving);
#endif
	Ui_MainDlg.urOutput->setUrl(KUrl(QDir::cleanPath(_index==oAudio?config.getPrefOutputAudio():config.getPrefOutput())));
      break;
     case oMP4:
	Ui_MainDlg.wOutput->hide();
        Ui_MainDlg.urOutput->show();
#ifdef BACKLITE
        Ui_MainDlg.urOutput->setFilter("Audio Video Interleave (*.avi);;MPEG-1 and MPEG-2 systems (*.mpg);;MPEG-4 format (*.mp4);;Advanced Streaming Format (*.asf);;Adobe Flash (*.swf);;Adobe Flash video files (*.flv);;RealVideo (*.rm);;QuickTime (*.mov);;Matroska (*.mkv)");
        Ui_MainDlg.urOutput->setMode(KUrlRequester::SAVE) ;
#else
        Ui_MainDlg.urOutput->setFilter("*.avi|Audio Video Interleave (*.avi)\n*.mpg|MPEG-1 and MPEG-2 systems (*.mpg)\n*.mp4|MPEG-4 format (*.mp4)\n*.asf|Advanced Streaming Format (*.asf)\n*.swf|Adobe Flash (*.swf)\n*.flv|Adobe Flash video files (*.flv)\n*.rm|RealVideo (*.rm)\n*.mov|QuickTime (*.mov)\n*.mkv|Matroska (*.mkv)");
        Ui_MainDlg.urOutput->setMode(KFile::File) ;
        Ui_MainDlg.urOutput->fileDialog()->setOperationMode(KFileDialog::Saving);
#endif
	Ui_MainDlg.urOutput->setUrl(KUrl(QDir::cleanPath(config.getPrefOutputMpeg4()+"/"+title+".avi") ));
	break;
     case oMPEG2:
	Ui_MainDlg.wOutput->hide();
        Ui_MainDlg.urOutput->show();
#ifdef BACKLITE
        Ui_MainDlg.urOutput->setFilter("MPEG-1 and MPEG-2 systems (*.mpg)");
        Ui_MainDlg.urOutput->setMode(KUrlRequester::SAVE) ;
#else
        Ui_MainDlg.urOutput->setFilter("*.mpg|MPEG-1 and MPEG-2 systems (*.mpg)");
        Ui_MainDlg.urOutput->setMode(KFile::File) ;
        Ui_MainDlg.urOutput->fileDialog()->setOperationMode(KFileDialog::Saving);
#endif
	Ui_MainDlg.urOutput->setUrl(KUrl(QDir::cleanPath(config.getPrefOutputMpeg2()+"/"+title+".mpg") ));
      break;

   }
   updateSelection();
}

void k9Main::showPopup(const QPoint & iPosition) {
    columns.clear();

    QMenu menu(listView1);

    for (int i=1; i< listView1->columnCount() ;i++) {
	k9ListColumn *col=new k9ListColumn(listView1,i);
	columns << col;
	QAction *m=new QAction(listView1->headerItem()->text(i),this)  ;
	m->setCheckable(true);
	m->setChecked(!listView1->isColumnHidden(i) );
	connect(m, SIGNAL(toggled(bool)), col, SLOT(setVisible(bool)));
	menu.addAction(m);
    }
    menu.exec(listView1->mapToGlobal(iPosition));

}

void k9ListColumn::setVisible(bool value) {
   QTreeWidget *parent=(QTreeWidget*)this->parent() ;

   parent->setColumnHidden(m_column,!value);
   k9Main* maindlg=(k9Main*)parent->parent();
   maindlg->resizeColumns();
} 


