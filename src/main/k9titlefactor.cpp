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


#include "k9titlefactor.h"
#include "k9cellcopylist.h"
#include "k9dvdsize.h"
#include <qgroupbox.h>
#include <qslider.h>
#include <qlabel.h>
#include <qcheckbox.h>



k9TitleFactor::k9TitleFactor(QWidget* parent)
        : QWidget(parent) {
    Ui_titleFactor.setupUi(this);
    m_current=NULL;
    m_slFactorPressed=false;
    m_factor=-1;
    m_dvd=NULL;
    m_updating=false;
    SelectionChanged( NULL,m_withMenus);
    m_useDvdAuthor=false;
    Ui_titleFactor.slFactor->setPageStep(0);
}

k9TitleFactor::~k9TitleFactor() {}

/*$SPECIALIZATION$*/
void k9TitleFactor::setUseDvdAuthor( bool _value) {
   m_useDvdAuthor=_value;
}

float k9TitleFactor::getFactor() {
    if (m_factor==-1)
	    m_factor=m_dvd->getfactor( m_withMenus,false,m_useDvdAuthor);
    return m_factor;	
}

void k9TitleFactor::ckAutoClicked(bool checked) {
    if (m_current!=NULL && !m_updating) {
        m_current->setforceFactor(checked);
        if ( checked) {
	    m_current->setfactor( getFactor());
            Ui_titleFactor.slFactor->setValue(m_current->getfactor()*100.0);
            setMinFactor();
        } else {
            Ui_titleFactor.slFactor->setValue(getFactor()*100.0);
            Ui_titleFactor.slFactor->setMinimum(100);
        }
	double size;
	size =m_current->getChaptersSize_mb(true) / getFactor();
        Ui_titleFactor.lTargetSize->setText(i18n("size : %1 MB").arg(size ));
    }
}

void k9TitleFactor::slFactorReleased() {
    m_slFactorPressed=false;
}

void k9TitleFactor::slFactorPressed() {
    if (m_current !=NULL)  {
        m_current->setforceFactor(true);
        setMinFactor();
    }
    m_slFactorPressed=true;

}

void k9TitleFactor::slFactor_valueChanged( int ) {
    if (m_current !=NULL ) {
	if (m_slFactorPressed) {
		m_current->setfactor( (double)Ui_titleFactor.slFactor->value() /100);
		for (int i=0;i <m_current->getTitles().count();i++) {
			k9DVDTitle * title=m_current->getTitles().at(i);
			title->setfactor( (double)Ui_titleFactor.slFactor->value() /100);
		}
		m_factor=-1;
	}
	double size;

	if (m_current->getforceFactor()) {	
        	Ui_titleFactor.tFactor->setText(QString("%1").arg(m_current->getfactor(),4,'f',2));
		size =m_current->getChaptersSize_mb(true) / m_current->getfactor();
	
	} else {
 		Ui_titleFactor.tFactor->setText(QString("%1").arg(getFactor(),4,'f',2));
		size =m_current->getChaptersSize_mb(true) / getFactor();
	}
        Ui_titleFactor.lTargetSize->setText(i18n("size : %1 MB").arg(size ));


    }

}

void k9TitleFactor::changedTitle(k9DVDTitle *_title) {
	if (_title ==NULL) {
		Ui_titleFactor.gbTitle->hide();
		Ui_titleFactor.lbTitle->setText(i18n("Select a Title in the treeview..."));		
		return;
	}
	
	Ui_titleFactor.gbTitle->show();
	m_current=_title;
	
	m_updating=true;
	Ui_titleFactor.gbTitle->setChecked(m_current->getforceFactor());
	m_updating=false;
	
        Ui_titleFactor.lbTitle->setText(i18n("Shrink Factor for %1").arg(m_current->getname()));
	double size;
	if (m_current->getforceFactor()) {
		Ui_titleFactor.slFactor->setValue(m_current->getfactor()*100.0);
		Ui_titleFactor.tFactor->setText(QString("%1").arg(m_current->getfactor(),4,'f',2));        
		setMinFactor();
		size =m_current->getChaptersSize_mb(true) / m_current->getfactor();
	} else {
		Ui_titleFactor.slFactor->setValue(getFactor()*100.0);
		Ui_titleFactor.tFactor->setText(QString("%1").arg(getFactor(),4,'f',2));
		Ui_titleFactor.slFactor->setMinimum(100);
		size =m_current->getChaptersSize_mb(true) / getFactor();
	}
        Ui_titleFactor.lTargetSize->setText(i18n("size : %1 MB").arg(size ));
		
}


void k9TitleFactor::setMinFactor() {
    k9CellCopyList *lst=new k9CellCopyList(m_dvd->getdvd(),m_dvd);
    double minFactor=lst->getMinFactor( true); //TODO : corriger pour copie sans menus
    Ui_titleFactor.slFactor->setMinimum(minFactor*100);

    delete lst;
}

void k9TitleFactor::SelectionChanged( k9DVD *_dvd,bool _withMenus) {
    m_dvd=_dvd;
    if (_dvd==NULL) 
	    m_current=NULL;
    m_factor=-1;
    m_withMenus=_withMenus;

    changedTitle( m_current);
    
}

