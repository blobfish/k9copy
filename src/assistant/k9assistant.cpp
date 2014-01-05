//
// C++ Implementation: k9assistant
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <KIconLoader>
#include <KApplication>
#include "k9assistant.h"
#include "k9astsource.h"
#include "k9asttitles.h"
#include "k9aststreams.h"
#include "k9astdestination.h"
#include "k9astmp4.h"
#include "k9astmpeg.h"
#include "k9astdvdoption.h"
#include "k9dvd.h"
#include "k9cddrive.h"
#include "k9execcopy.h"
#include "dvdread.h"
#include "k9settings.h"
#include "k9dvdtitle.h"
k9Assistant *k9Assistant::createAssistant() {
    KPageWidget * pageWidget=new KPageWidget();
    return new k9Assistant(pageWidget);

}

k9Assistant::k9Assistant(KPageWidget *_pageWidget)
        : KAssistantDialog(_pageWidget) {
    m_pageWidget=_pageWidget;
    m_dvd=new k9DVD();
    m_drives=new k9CdDrives(this);
}
void k9Assistant::run() {

    setCaption(makeStandardCaption(i18n("DVD backup assistant")));
    KIconLoader *loader = KIconLoader::global();
    
    k9astSource *source=new k9astSource(this,m_drives,m_dvd);
    m_sourceItem=new KPageWidgetItem(source,i18n("source"));
    m_sourceItem->setHeader( i18n( "Select the source of the backup" ) );
    m_sourceItem->setIcon(KIcon(loader->loadIconSet("media-optical",  KIconLoader::Desktop)));

    m_pageWidget->addPage(m_sourceItem);

    k9astDestination *destination=new k9astDestination(this,m_drives,m_dvd);
    m_destinationItem=new KPageWidgetItem(destination,i18n("destination"));
    m_destinationItem->setHeader( i18n( "Select the destination of the backup" ) );
    m_destinationItem->setIcon(KIcon(loader->loadIconSet("media-optical-recordable",  KIconLoader::Toolbar)));
    m_pageWidget->addPage(m_destinationItem);

    k9astTitles *titles=new k9astTitles(this,m_dvd);
    m_titlesItem=new KPageWidgetItem(titles,i18n("titles"));
    m_titlesItem->setHeader(i18n("Select the titles you want to copy"));
    m_titlesItem->setIcon(KIcon("titles"));
    m_titlesItem->setIcon(KIcon(loader->loadIconSet("title",  KIconLoader::Toolbar)));

    m_pageWidget->addPage(m_titlesItem);

    k9astStreams *streams=new k9astStreams(this,m_dvd);
    m_streamsItem=new KPageWidgetItem(streams,i18n("streams"));
    m_streamsItem->setHeader(i18n("Select the streams you want to keep"));
    m_streamsItem->setIcon(KIcon(loader->loadIconSet("speaker",  KIconLoader::Toolbar)));

    m_pageWidget->addPage(m_streamsItem);

    k9astMp4 *mp4=new k9astMp4(this,m_dvd);
    m_mp4Item=new KPageWidgetItem(mp4,i18n("mp4"));
    m_mp4Item->setHeader(i18n("Set mp4 options for each title"));
    m_mp4Item->setIcon(KIcon(loader->loadIconSet("mp4",  KIconLoader::Toolbar)));

    m_pageWidget->addPage(m_mp4Item);

    k9astDVDOption *DVDOption=new k9astDVDOption(this,m_dvd);
    m_DVDOptionItem=new KPageWidgetItem(DVDOption,i18n("DVDOption"));
    m_DVDOptionItem->setHeader(i18n("Set DVD playback options"));
    m_DVDOptionItem->setIcon(KIcon(loader->loadIconSet("dvdcopy",  KIconLoader::Toolbar)));

    m_pageWidget->addPage(m_DVDOptionItem);

    k9astMpeg *mpeg=new k9astMpeg(this,m_dvd);
    m_mpegItem=new KPageWidgetItem(mpeg,i18n("mpeg"));
    m_mpegItem->setHeader(i18n("Set mpeg options for each title"));
    m_mpegItem->setIcon(KIcon(loader->loadIconSet("mpeg",  KIconLoader::Toolbar)));

    m_pageWidget->addPage(m_mpegItem);

    connect(titles,SIGNAL(addTitle(k9DVDTitle*)),streams,SLOT(addTitle(k9DVDTitle*)));
    connect(titles,SIGNAL(removeTitle(k9DVDTitle*)),streams,SLOT(removeTitle(k9DVDTitle*)));
    connect(titles,SIGNAL(addTitle(k9DVDTitle*)),mp4,SLOT(addTitle(k9DVDTitle*)));
    connect(titles,SIGNAL(removeTitle(k9DVDTitle*)),mp4,SLOT(removeTitle(k9DVDTitle*)));

    connect(titles,SIGNAL(addTitle(k9DVDTitle*)),mpeg,SLOT(addTitle(k9DVDTitle*)));
    connect(titles,SIGNAL(removeTitle(k9DVDTitle*)),mpeg,SLOT(removeTitle(k9DVDTitle*)));

    connect(titles,SIGNAL(addTitle(k9DVDTitle*)),DVDOption,SLOT(addTitle(k9DVDTitle*)));
    connect(titles,SIGNAL(removeTitle(k9DVDTitle*)),DVDOption,SLOT(removeTitle(k9DVDTitle*)));

    connect(this,SIGNAL(helpClicked()),this,SLOT(helpClicked()));
    m_drives->scanDrives();

    setButtonText(KDialog::Help,i18n("Settings"));
    setButtonToolTip(KDialog::Help,i18n("k9copy settings"));
    setButtonIcon(KDialog::Help,KIcon(SmallIcon("configure")));
    if (m_path !="") {
	source->setPath(m_path);
    }
    exec();
}

void k9Assistant::back() {
    if (currentPage() ==m_destinationItem) {
	for (int i=0;i< m_dvd->gettitleCount();i++) {
	    k9DVDTitle *title=m_dvd->gettitle(i);
	    title->setSelected(false);

	}
    }
    KAssistantDialog::back();
}

void k9Assistant::next() {
    QString err="";
    k9astTitles *titles=(k9astTitles*)m_titlesItem->widget();
    k9astStreams *streams=(k9astStreams*)m_streamsItem->widget();
    k9astDestination *adestination=(k9astDestination*)m_destinationItem->widget();

    k9astMp4 *mp4=(k9astMp4*) m_mp4Item->widget();
    titles->stopPreview();

    if (currentPage() == m_sourceItem) {
        k9astSource *source=(k9astSource*)m_sourceItem->widget();
        if (source->openDvd()) {
            titles->fill();
	    adestination->updatePaths();
	} else 
	    err=m_dvd->geterrMsg();
    } else if (currentPage() == m_destinationItem) {

	if (!adestination->isOk())
		err=i18n("The destination of the copy isn't valid");
	streams->selectOne(false);
	if (adestination->isMatroska())
	   mp4->setMatroskaCodecs();
	if ((m_destination== dstAvi) && !adestination->isMatroska()) {
            streams->selectOne(true);
	    streams->hideSubtitles(false);
	    mp4->setAviCodecs();
	} else if (m_destination==dstAudio) {
	    mp4->setAudio();
	    streams->hideSubtitles(true);
	} else
	    streams->hideSubtitles(false);
	streams->updateColumns();
    } else if (currentPage() == m_titlesItem) {
        titles->updateTitles();
        if (!titles->isOk())
	    err=i18n("You must select at least on title !");
	else
            titles->stopPreview();

    } else if (currentPage() == m_DVDOptionItem) {
        k9astDVDOption *options=(k9astDVDOption*)m_DVDOptionItem->widget();
        options->updateOptions();
        k9astMpeg * mpeg=(k9astMpeg*)m_mpegItem->widget();
	mpeg->updateFactor();
    } else if (currentPage() == m_streamsItem) {
        streams->updateStreams();
        switch (destination()) {
        case dstAvi:
	case dstAudio:
            setAppropriate(m_DVDOptionItem,false);
            setAppropriate(m_mpegItem,false);
            setAppropriate(m_mp4Item,true);
            break;
        case dstMpeg:
            setAppropriate(m_DVDOptionItem,false);
            setAppropriate(m_mpegItem,true);
            setAppropriate(m_mp4Item,false);
            break;
        case dstDVD:
            setAppropriate(m_DVDOptionItem,true);
            setAppropriate(m_mpegItem,true);
            setAppropriate(m_mp4Item,false);
            break;
        default:
            break;

        }

    }
    if (err=="") {
    	KAssistantDialog::next();
	streams->updateColumns();
    } else
 	k9Dialogs::error(err,i18n("cannot continue"));

}

void k9Assistant::helpClicked() {
    k9Settings settings(this,i18n("Settings"));
    settings.exec();
    k9astTitles *titles=(k9astTitles*)m_titlesItem->widget();
    titles->stopPreview();
    titles->loadPlayer();

}

void k9Assistant::accept() {
    k9astDestination *destination=(k9astDestination*)m_destinationItem->widget();

    //stop preview before copying the dvd 
    k9astTitles *titles=(k9astTitles*)m_titlesItem->widget();
    titles->unloadPreview();


    k9ExecCopy execCopy;
    execCopy.setCopyMenus(m_copyMenus);
    execCopy.setPath(destination->getPath());
    if (m_DVDdestination== dstFolder)
	execCopy.setOutput(k9ExecCopy::oFolder);
    else if (m_DVDdestination==dstIso)
	execCopy.setOutput(k9ExecCopy::oISO);
    else 
	execCopy.setOutput(k9ExecCopy::oDVD);
    execCopy.setDvd(m_dvd);

    switch (m_destination) {
	case dstDVD:
    	    execCopy.copyDVD();
	    break;
	case dstAvi:
	    execCopy.CreateMP4();
	    break;
	case dstMpeg:
	    execCopy.extractMPEG2();
	    break;
	case dstAudio:
	    execCopy.extractAudio();
	default:
	    break;
     }

    KAssistantDialog::accept();
}

void k9Assistant::reject() {
    k9astTitles *titles=(k9astTitles*)m_titlesItem->widget();
    titles->unloadPreview();
    KAssistantDialog::reject();
}
k9Assistant::~k9Assistant() {
    delete m_dvd;
}


