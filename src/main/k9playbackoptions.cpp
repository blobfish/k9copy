//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9playbackoptions.h"
#include <kpushbutton.h>
#ifndef BACKLITE
#include <kiconloader.h>
#endif

k9PlaybackOptions::k9PlaybackOptions(k9Main *mainWindow, QWidget* parent)
: QWidget(parent)
{
    Ui_playbackOptionsw.setupUi(this);
    setMain( mainWindow);
    m_main->setPlaybackOptions(this);
    enable(false);
    k9Config config;
    Ui_playbackOptionsw.ckMenu->setChecked(config.getKeepMenus());
    ckMenuClick();
  
}

k9PlaybackOptions::~k9PlaybackOptions()
{
}

/*$SPECIALIZATION$*/
 void k9PlaybackOptions::setMain(k9Main* _value) { 
     m_main = _value; m_dvd=m_main->dvd; 
}


void k9PlaybackOptions::fillTitleList() {
    QString txt;
    if (Ui_playbackOptionsw.lbSequence->currentItem() !=NULL)
        Ui_playbackOptionsw.lbSequence->currentItem()->text();

    Ui_playbackOptionsw.lbSequence->clear();
    k9DVDTitle *title=m_dvd->getstart();
  
    bool found=title !=NULL;
    //Reconstitution de l'ordre de lecture à partir des titres du DVD
    while (found) {
        if (title->isSelected()) {
	        lbItem *item=new lbItem(Ui_playbackOptionsw.lbSequence,title->getname());
		item->setTitle(title);
		if (title->getname()==txt) 
		Ui_playbackOptionsw.lbSequence->setCurrentItem(item);
	}
	title=title->getnextTitle();
	found=title!=NULL;	
    }

    
    
    for (int i=0;i<m_dvd->gettitleCount();i++) {
        k9DVDTitle* l_track=m_dvd->gettitle(i);
        if (l_track->getIndexed()) {
            QString sTitle=l_track->getname();
            if(l_track->isSelected()) {
   		bool foundtitle=false;
                for (int j=0; j<Ui_playbackOptionsw.lbSequence->count();j++) {
		   lbItem *tmp=(lbItem*)Ui_playbackOptionsw.lbSequence->item(j);
		   if (tmp->getTitle() == l_track)
			foundtitle=true;
		}	
		if (!foundtitle) {
			lbItem *item=new lbItem(Ui_playbackOptionsw.lbSequence,sTitle);
			item->setTitle(l_track);
			if (sTitle==txt) 
			Ui_playbackOptionsw.lbSequence->setCurrentItem(item);
		}
            }
        }
    }
    if (Ui_playbackOptionsw.lbSequence->currentRow()==-1)
        Ui_playbackOptionsw.lbSequence->setCurrentRow(0);
    setSequence();
    enable(!Ui_playbackOptionsw.ckMenu->isChecked() );

}

void k9PlaybackOptions::ckMenuClick() {
     
    if (m_main!=NULL) {
        enable((!Ui_playbackOptionsw.ckMenu->isChecked()) && m_dvd->getopened());
 	m_main->updateFactor();
    } else
	enable(false);
}


void k9PlaybackOptions::setSequence() {
    for (int i=0;i <m_dvd->gettitleCount();i++)
	m_dvd->gettitle(i)->setnextTitle( NULL);

    lbItem *lbi = (lbItem*)Ui_playbackOptionsw.lbSequence->item(0);
    if (lbi !=NULL) {
	lbItem *lbi2;
	m_dvd->setstart(lbi->getTitle());
        for (int i=0 ; i < Ui_playbackOptionsw.lbSequence->count()-1;i++) {
		lbi=(lbItem*)Ui_playbackOptionsw.lbSequence->item(i);
		lbi2=(lbItem*)Ui_playbackOptionsw.lbSequence->item(i+1);
		lbi->getTitle()->setnextTitle(lbi2->getTitle());
         }
    }
}

void k9PlaybackOptions::lbSequenceChanged(QListWidgetItem *_item,QListWidgetItem *) {
    if (_item == NULL)
	return;
    lbItem *lbi=(lbItem*) _item;
    k9DVDTitle *title=lbi->getTitle();
    Ui_playbackOptionsw.cbDefAudio->clear();
    Ui_playbackOptionsw.cbDefSub->clear();
    lstAudioDef.clear();
    lstSubDef.clear();
    Ui_playbackOptionsw.cbDefAudio->addItem(i18n("default"));
    lstAudioDef.append(NULL);
    Ui_playbackOptionsw.cbDefSub->addItem(i18n("none"));
    lstSubDef.append(NULL);

    for (int i=0;i < title->getaudioStreamCount();i++) {
	if (title->getaudioStream(i)->getselected()) {
//		if ( !title->getDefAudioSet())
//		   title->setDefAudio(title->getaudioStream(i));
		Ui_playbackOptionsw.cbDefAudio->addItem( QString("%1 - %2").arg(title->getaudioStream(i)->getID()).arg(title->getaudioStream(i)->getlanguage()));
		if (title->getaudioStream(i)==title->getDefAudio()) {
		   Ui_playbackOptionsw.cbDefAudio->setCurrentIndex(Ui_playbackOptionsw.cbDefAudio->count()-1);
		}
		lstAudioDef.append(title->getaudioStream(i));
	}
    }

    for (int i=0;i < title->getsubPictureCount();i++) {
	if (title->getsubtitle(i)->getselected()) {
                QPixmap icon;
		Ui_playbackOptionsw.cbDefSub->addItem(QString("%1 - %2").arg(title->getsubtitle(i)->getID().first()).arg(title->getsubtitle(i)->getlanguage()));
		if (title->getsubtitle(i)==title->getDefSubtitle()) {
		   Ui_playbackOptionsw.cbDefSub->setCurrentIndex(Ui_playbackOptionsw.cbDefSub->count()-1);
		}
		lstSubDef.append(title->getsubtitle(i));
	}
    }

}

void k9PlaybackOptions::cbDefAudioActivated(int _index) {
    int cur=Ui_playbackOptionsw.lbSequence->currentRow();
    lbItem *lbi=(lbItem*)Ui_playbackOptionsw.lbSequence->item(cur);
    lbi->getTitle()->setDefAudio(lstAudioDef.at(_index));
}
 
void k9PlaybackOptions::cbDefSubActivated(int _index) {
    int cur=Ui_playbackOptionsw.lbSequence->currentRow();
    lbItem *lbi=(lbItem*)Ui_playbackOptionsw.lbSequence->item(cur);
    lbi->getTitle()->setDefSubtitle(lstSubDef.at(_index));
}

void k9PlaybackOptions::titleRenamed (k9DVDTitle *_title, QString _newName) {
    for (int j=0; j<Ui_playbackOptionsw.lbSequence->count();j++) {
	    lbItem *tmp=(lbItem*)Ui_playbackOptionsw.lbSequence->item(j);
	     if (tmp->getTitle() == _title) {
                Ui_playbackOptionsw.lbSequence->item(j)->setText(_newName);
		break;
	     }
	}	

}

bool k9PlaybackOptions::withMenus() {
    return Ui_playbackOptionsw.ckMenu->isChecked();
}

void k9PlaybackOptions::setwithMenus(bool _value) {
    Ui_playbackOptionsw.ckMenu->setChecked(_value);

}


void k9PlaybackOptions::enable(bool _value) {
    Ui_playbackOptionsw.frPlayback->setEnabled(_value);
}

void k9PlaybackOptions::clear() {
    Ui_playbackOptionsw.lbSequence->clear();
    enable(false);
}

