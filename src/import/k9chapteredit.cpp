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


#include "k9chapteredit.h"
#include "k9drawimage.h"
#include <qlabel.h>
#include <qpixmap.h>
#include <qslider.h>
#include <qlayout.h>
#include <kurlrequester.h>
#include <qcheckbox.h>
#include "k9title.h"
#include <kpushbutton.h>
#ifndef BACKLITE
#include <kiconloader.h>
#endif
#include <QPalette>
k9ChapterEdit::k9ChapterEdit(QWidget* parent)
: QWidget(parent)
{
  Ui_chapterEdit.setupUi(this);
  m_aviDecode=NULL;
  m_wimage=new k9DrawImage(Ui_chapterEdit.image);
  QGridLayout *l=new QGridLayout(Ui_chapterEdit.image);
  l->addWidget(m_wimage,0,0);
  QPalette p;
  p.setColor(QPalette::Window,Qt::black);
  m_wimage->setPalette(p);
  //Ui_chapterEdit.bTitleButton->setIcon(SmallIcon("frame_image"));
}

k9ChapterEdit::~k9ChapterEdit()
{
   if (m_aviDecode)
      delete m_aviDecode;
}

/*$SPECIALIZATION$*/

void k9ChapterEdit::sliderReleased() {
 //  m_aviDecode->seek((double)slider->value() / 100);
 //  m_aviDecode->readFrame();
}
void k9ChapterEdit::sliderChanged(int _value) {
   if (m_aviDecode->opened()) {
        //m_aviDecode->seek((double)slider->value() / 100);
        m_aviDecode->readFrame((double)_value / 1000);
        QTime t;
        t=t.addMSecs((double)_value);
        Ui_chapterEdit.lPosition->setText(t.toString("hh:mm:ss"));
    }
}


void k9ChapterEdit::urlSelected(const KUrl& _url) {
  bool fileChanged=_url.toLocalFile() !=m_aviFile->getFileName();
  if (!m_aviDecode) {
     m_aviDecode =new k9AviDecode(0,0);
     connect(m_aviDecode,SIGNAL(drawFrame(QImage)),this,SLOT(drawImage(QImage)));
  }
  m_aviDecode->open(_url.toLocalFile());
  if (!m_noUpdate) {
    m_aviFile->setFileName(_url.toLocalFile());
    if (fileChanged) {
        m_aviDecode->readFrame(0);
	m_aviFile->setStart(QTime(0,0,0));
	m_aviFile->setImage(m_image);
	QTime end(0,0,0);
	end=end.addSecs((int)(m_aviDecode->getDuration()));
	m_aviFile->setEnd(end);
	m_aviFile->setNext(NULL);
	m_aviFile->setPrevious(NULL);
        Ui_chapterEdit.ckBreakEnd->setChecked(false);
        Ui_chapterEdit.ckBreakStart->setChecked(false);
        ckBreakEndClick();
        ckBreakStartClick();
	setAviFile(m_aviFile);
    } 
  }

  QTime t;
  int sec=t.msecsTo(m_aviFile->getStart());
  Ui_chapterEdit.slider->setMaximum((int)(m_aviDecode->getDuration()*1000));

  Ui_chapterEdit.slider->setValue(sec);
  
}

void k9ChapterEdit::drawImage(QImage _image) {
  //QPixmap p(*_image);
  //image->setPixmap(p);
  m_wimage->setImage(_image);
  m_image=_image;
}

void k9ChapterEdit::bStartClick() {
   QTime t;
   int secs=Ui_chapterEdit.slider->value()/1000;
   t=t.addSecs(secs);
   Ui_chapterEdit.twstart->setTime(t);
}

void k9ChapterEdit::bEndClick() {
   QTime t;
   int secs=Ui_chapterEdit.slider->value()/1000;
   t=t.addSecs(secs);
   Ui_chapterEdit.twend->setTime(t);
}

void k9ChapterEdit::setAviFile(k9AviFile *_aviFile) {
    this->setEnabled(_aviFile!=NULL);
    m_aviFile=_aviFile;
    if (_aviFile) {
	setEnabled(true);
        m_noUpdate=true;
	Ui_chapterEdit.ckBreakStart->setEnabled(_aviFile->getReencode());
	Ui_chapterEdit.ckBreakEnd->setEnabled(_aviFile->getReencode());
	Ui_chapterEdit.twstart->setEnabled(_aviFile->getReencode());
	Ui_chapterEdit.twend->setEnabled(_aviFile->getReencode());


        urlSelected(m_aviFile->getFileName());

        k9AviFile *p=m_aviFile->getPrevious();
        k9AviFile *n=m_aviFile->getNext();
	QTime end(0,0,0);
	m_minTime=end;;
	end=end.addSecs((int)(m_aviDecode->getDuration()));
	m_maxTime=end;

        if (p) {
            if(p->getFileName() !=_aviFile->getFileName()) {
                Ui_chapterEdit.ckBreakStart->setEnabled(false);
                m_minTime.setHMS(0,0,0,0);
            } else {
                Ui_chapterEdit.ckBreakStart->setEnabled(true);
                m_minTime=p->getStart();
            }
        }
        if (n) {
            if(n->getFileName() !=_aviFile->getFileName()) {
                Ui_chapterEdit.ckBreakEnd->setEnabled(false);
                m_maxTime=_aviFile->getEnd();
            } else {
                Ui_chapterEdit.ckBreakEnd->setEnabled(true);
                m_maxTime=n->getEnd();
            }
        }


        Ui_chapterEdit.twstart->setTime(m_aviFile->getStart());
        Ui_chapterEdit.twend->setTime(m_aviFile->getEnd());
        Ui_chapterEdit.urFile->setUrl(m_aviFile->getFileName());

        Ui_chapterEdit.ckBreakStart->setChecked(m_aviFile->getBreakPrevious());
        Ui_chapterEdit.ckBreakEnd->setChecked(m_aviFile->getBreakNext());
    } else
	setEnabled(false);
    m_noUpdate=false;

}

void k9ChapterEdit::startChanged(const QTime& _time) {
    if (!m_noUpdate) {
        QTime t=_time;
        if (t > m_maxTime )
            t=m_maxTime;
        else if (t < m_minTime)
            t=m_minTime;
        else if (t >m_aviFile->getEnd())
            t=m_aviFile->getEnd();
        if (t!=_time)
            Ui_chapterEdit.twstart->setTime(t);
        m_aviFile->setStart(t);
        if (!m_aviFile->getBreakPrevious() && m_aviFile->getPrevious()!=NULL) {
            m_aviFile->getPrevious()->setEnd(t);
        }
        QTime t2;
        Ui_chapterEdit.slider->setValue(t2.msecsTo(t));
        m_aviFile->setImage(m_image);
    }
}

void k9ChapterEdit::endChanged(const QTime& _time) {
    if (!m_noUpdate) {
        QTime t=_time;
        if (t > m_maxTime )
            t=m_maxTime;
        else if (t < m_minTime)
            t=m_minTime;
        else if (t <m_aviFile->getStart())
            t=m_aviFile->getStart();
        if (t!=_time)
            Ui_chapterEdit.twend->setTime(t);        
        m_aviFile->setEnd(t);
        QTime t2;
        if (!m_aviFile->getBreakNext() && m_aviFile->getNext()!=NULL) {
            QImage i=m_image;
            m_aviDecode->readFrame((double)t2.msecsTo(t)/1000.0);
            m_aviFile->getNext()->setImage(m_image);
            m_image=i;
            m_aviFile->getNext()->setStart(t);
        }   
        Ui_chapterEdit.slider->setValue(t2.msecsTo(t));
    }
}

void k9ChapterEdit::ckBreakStartClick() {
    m_aviFile->setBreakPrevious(Ui_chapterEdit.ckBreakStart->isChecked());  

}

void k9ChapterEdit::bTitleButtonClick() {
   m_aviFile->getTitle()->getButton()->setImage(m_image);
}

void k9ChapterEdit::ckBreakEndClick() {
    m_aviFile->setBreakNext(Ui_chapterEdit.ckBreakEnd->isChecked());
}

