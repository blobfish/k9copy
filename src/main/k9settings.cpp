//
// C++ Implementation: k9Settings
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9settings.h"

#include <qframe.h>
#ifndef BACKLITE
#include <kglobal.h>
#include <kiconloader.h>
#endif
#include <kaction.h>
#include <qlayout.h>
#include <kdialog.h>
k9Settings::k9Settings(QWidget *parent,const QString &):  KPageDialog (parent) {
#ifndef BACKLITE
    setFaceType(KPageDialog::List);
    setButtons( Ok|Apply|Cancel);
#endif
    setWindowTitle(KDialog::makeStandardCaption(i18n("Settings"),this));
    //setButtons( Default|Ok|Apply|Cancel);

    //setInitialSize(QSize(750,350), false);
    KPageWidgetItem *page;


    m_prefDVD =new k9prefDVD(this);
    page = addPage(m_prefDVD,i18n("DVD"));
    page->setHeader(i18n("DVD Backup"));
    page->setIcon(KIcon("dvdcopy"));

    m_prefMencoder =new k9prefMencoder(this);
    page = addPage(m_prefMencoder,i18n("Encoders"));
    page->setHeader(i18n("MPEG-4 Codecs"));
    page->setIcon(KIcon("mencoder"));

    m_prefMPEG4 =new k9prefMPEG4(this);
    page = addPage(m_prefMPEG4,i18n("MPEG-4"));
    page->setHeader(i18n("MPEG-4 Encoding"));
    page->setIcon(KIcon("video-mp4"));


    m_prefPreview =new k9prefPreview(this);
    page = addPage(m_prefPreview,i18n("Preview"));
    page->setHeader(i18n("Title preview"));
    page->setIcon(KIcon("video-projector"));

    m_prefAuthor = new k9prefAuthor(this);
    page = addPage(m_prefAuthor,i18n("Authoring"));
    page->setHeader(i18n("Authoring"));
    page->setIcon(KIcon("author"));

    m_prefDirs=new k9prefDirs(this);
    page = addPage(m_prefDirs,i18n("Paths"));
    page->setHeader(i18n("Default paths"));
    page->setIcon(KIcon("folder"));
    
#ifndef BACKLITE
    m_configDlg=new kConfigDlg(this);
    page = addPage(m_configDlg,i18n("Devices"));
    page->setHeader(i18n("Devices not detected by k9copy"));
    page->setIcon(KIcon("drive-optical"));
#endif

    connect(this, SIGNAL(applyClicked()), SLOT(slotApplyClicked()));
    connect(this, SIGNAL(okClicked()), SLOT(slotOkClicked()));

}

void k9Settings::slotOkClicked() {

    m_prefDVD->save();
    m_prefMPEG4->save();
    m_prefMencoder->save();
    m_prefPreview->save();
    m_prefAuthor->save();
#ifndef BACKLITE
    m_configDlg->save();
#endif
    m_prefDirs->save();

}

void k9Settings::slotApplyClicked() {
   slotOkClicked();
   m_prefMPEG4->load();
}

k9Settings::~k9Settings() {
 /*   delete m_configDlg;
    delete m_prefDVD;
    delete m_prefMPEG4;
    delete m_prefMencoder;
    delete m_prefPreview;
*/
}
