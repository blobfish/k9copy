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


#include "k9newtitle.h"
#include "k9import.h"
#include <qradiobutton.h>
#include "k9avidecode.h"
#include "k9menu.h"
#include "k9menubutton.h"
#include "kpushbutton.h"

#include <QFile>
#include "k9cell.h"
#ifndef BACKLITE
#include <KFileDialog>
#endif
k9NewTitle::k9NewTitle(QWidget* parent)
        : QWidget(parent) {
    m_fileName="";
    Ui_newTitle.setupUi(this);
#ifdef BACKLITE
        Ui_newTitle.urFile->setFilter("Audio Video Interleave (*.avi);;MPEG-1 and MPEG-2 systems (*.mpg);;MPEG-4 format (*.mp4);;Advanced Streaming Format (*.asf);;Adobe Flash (*.swf);;Adobe Flash video files (*.flv);;RealVideo (*.rm);;QuickTime (*.mov);;Matroska (*.mkv);; All(*.*)");
        Ui_newTitle.urFile->setMode(KUrlRequester::OPEN) ;
#else
        Ui_newTitle.urFile->setFilter("*.avi|Audio Video Interleave (*.avi)\n*.mpg|MPEG-1 and MPEG-2 systems (*.mpg)\n*.mp4|MPEG-4 format (*.mp4)\n*.asf|Advanced Streaming Format (*.asf)\n*.swf|Adobe Flash (*.swf)\n*.flv|Adobe Flash video files (*.flv)\n*.rm|RealVideo (*.rm)\n*.mov|QuickTime (*.mov)\n*.mkv|Matroska (*.mkv)\n*.*|All (*.*)");
        Ui_newTitle.urFile->setMode(KFile::File) ;
        Ui_newTitle.urFile->fileDialog()->setOperationMode(KFileDialog::Opening);
#endif

#ifdef BACKLITE
    Ui_newTitle.bAdd->setIcon(SmallIcon("list-add"));
#else
    Ui_newTitle.bAdd->setGuiItem(KStandardGuiItem::Add);
#endif
    Ui_newTitle.ckReencode->setEnabled(false);
    Ui_newTitle.bAdd->setEnabled(false);
}

k9NewTitle::~k9NewTitle() {

}

/*$SPECIALIZATION$*/

bool k9NewTitle::isMpeg2(const QString &_fileName) {
   char buffer[2048];
   QFile f(_fileName);
   f.open(QIODevice::ReadOnly);
   f.read(buffer,2048);
   f.close();

   if (k9Cell::isNavPack((uchar*)buffer)==1)
	return true;
   else
	return false;
}

void k9NewTitle::urlSelected(const KUrl &_url) {
    m_fileName=_url.toLocalFile();
    k9AviDecode fileInfo(0,0);
    fileInfo.open(m_fileName);
    double duration=fileInfo.getDuration();
    fileInfo.close();
    QTime t(0,0,0);
    t=t.addSecs(duration); 
    Ui_newTitle.lTotalTime->setText(t.toString("hh:mm:ss"));
    Ui_newTitle.bAdd->setEnabled(true);
    bool mpeg2=isMpeg2(m_fileName);
    Ui_newTitle.ckReencode->setEnabled(mpeg2);
}

void k9NewTitle::drawImage (QImage _image) {
    m_image=_image;
}

void k9NewTitle::ckReencodeClicked() {
    if (Ui_newTitle.ckReencode->isChecked()) {
	Ui_newTitle.rbNumber->setChecked(true);
	Ui_newTitle.nbChapters->setValue(1);
    }
    Ui_newTitle.nbChapters->setEnabled(!Ui_newTitle.ckReencode->isChecked());
    Ui_newTitle.twLength->setEnabled(!Ui_newTitle.ckReencode->isChecked());
    Ui_newTitle.rbNumber->setEnabled(!Ui_newTitle.ckReencode->isChecked());
    Ui_newTitle.rbLength->setEnabled(!Ui_newTitle.ckReencode->isChecked());
	
}

void k9NewTitle::bAddClicked() {
    k9Config config;
    k9AviDecode fileInfo(0,0);
    if (!fileInfo.open(m_fileName)) {
        k9Dialogs::error(fileInfo.getError(),i18n("Add title"));
        return;
    }
    this->setCursor(QCursor(Qt::WaitCursor));

    k9LvItemImport *item=new k9LvItemImport(m_k9Import->getRoot(),k9LvItemImport::TITLE);
    item->setExpanded(true);
    k9Title *title=new k9Title( m_k9Import->getNewDVD());
    item->setText(0,i18n("title %1").arg(title->getNum() +1));

    connect( title->getButton(),SIGNAL(sigsetImage(k9MenuButton*, const QImage&)),m_k9Import,SLOT(buttonUpdated(k9MenuButton*, const QImage&)));
    item->setTitle(title);


    connect(&fileInfo,SIGNAL(drawFrame(QImage)),this,SLOT(drawImage(QImage)));



    QTime t;
    item->setText(1,t.addSecs(fileInfo.getDuration()).toString("hh:mm:ss"));

    double increment=0;
    int maxCh;
    if (Ui_newTitle.rbLength->isChecked()) {
        QTime t;
        increment=t.secsTo(Ui_newTitle.twLength->time());
        maxCh=999;
    }

    if (Ui_newTitle.rbNumber->isChecked()) {
        increment=fileInfo.getDuration() /Ui_newTitle.nbChapters->value();
        maxCh=Ui_newTitle.nbChapters->value();
    }

    double pos=0;
    k9AviFile *last=NULL;
    int width=config.getPrefButtonWidth();
    int height=config.getPrefButtonHeight();
    int left=-width+20;
    int top=50;

    int i=-1;
    while ( pos <fileInfo.getDuration() && i <maxCh-1){    
    //fileInfo.open(m_fileName);
        i++;
        fileInfo.readFrame(pos);
        k9AviFile *file=new k9AviFile(title,0);
        file->setImage(m_image);
        if (i==0)
            title->getButton()->setImage(m_image);

        file->setPrevious(last);
        if (last !=NULL)
            last->setNext(file);
        last=file;

        file->setFileName(m_fileName);
        file->setStart(t.addMSecs(pos*1000));
        pos+=increment;
        file->setEnd(t.addMSecs(qMin(pos,fileInfo.getDuration()) *1000));
	file->setReencode(!Ui_newTitle.ckReencode->isChecked());
        k9LvItemImport * itemch=new k9LvItemImport(item,k9LvItemImport::CHAPTER);
        itemch->setText(0,i18n("chapter %1").arg(i+1));
        itemch->setText(1,file->getStart().toString("hh:mm:ss") +" - "+file->getEnd().toString("hh:mm:ss") );
        itemch->setAviFile(file);
        itemch->setTitle(title);
        itemch->setIcon(0,QIcon(QPixmap::fromImage(m_image.scaled(50,40))));
        connect(file,SIGNAL(aviFileUpdated(k9AviFile*)),m_k9Import,SLOT(aviFileUpdated(k9AviFile*)));

        k9MenuButton *btn=title->getMenu()->addButton();
        btn->setNum(i+1);
        //_title->setButton(btn);
        btn->setImage(m_image);
        left +=width+10;
        if (left +width >720) {
            left=30;
            top+=height+25;
        }
        btn->setTop(top);
        btn->setLeft(left);
        btn->setWidth(width);
        btn->setHeight(height);
        btn->setScript(QString("jump title 1 chapter %1 ;").arg(i+1));
        btn->setTextPosition(BOTTOM);
        btn->setText(i18n("chapter %1").arg(i+1));
        btn->setColor(config.getPrefButtonTextColor());
        btn->setFont(config.getPrefButtonFont());
	file->setButton(btn);
        connect(file,SIGNAL(imageChanged(const QImage&)),btn,SLOT(setImage(const QImage&)));

    }
    title->getMenu()->setEndScript("call vmgm menu;");
    fileInfo.close();
    m_k9Import->lvDVD->sortItems(0,Qt::AscendingOrder);
    m_k9Import->setEnableCreate(true);
    m_k9Import->updateTotalTime();
    this->setCursor(QCursor(Qt::ArrowCursor));
    m_k9Import->getRoot()->treeWidget()->resizeColumnToContents(0);

}

void k9NewTitle::rbNumberClicked() {
    Ui_newTitle.twLength->setEnabled(false);
    Ui_newTitle.nbChapters->setEnabled(true);
}

void k9NewTitle::rbLengthClicked() {
    Ui_newTitle.twLength->setEnabled(true);
    Ui_newTitle.nbChapters->setEnabled(false);

}



void k9NewTitle::setK9Import ( k9Import* _value ) {
    m_k9Import = _value;
}
