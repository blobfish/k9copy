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

#include "k9asttitles.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include <QObject>
#include "kviewmpeg2.h"
#include <QLayout>
#include <QCheckBox>
_k9ItemWidget::_k9ItemWidget(k9DVDTitle *_title,QTreeWidget *_treeWidget,int _type):QObject(0),QTreeWidgetItem(_treeWidget,_type) {
    switch (_type) {
    case k9astTitles::TITLE :
        title=_title;
        setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
        setCheckState(0,Qt::Unchecked);
        setTextAlignment(2,Qt::AlignRight);
	setTextAlignment(3,Qt::AlignRight);
        connect (title,SIGNAL(selectionChanged()),this,SLOT(titleChanged()));
        break;
    }

}
_k9ItemWidget::_k9ItemWidget(k9DVDChapter *_chapter,QTreeWidgetItem *_treeWidget,int _type):QObject(0),QTreeWidgetItem(_treeWidget,_type) {
    chapter=_chapter;
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    setCheckState(0,Qt::Unchecked);
    connect (chapter,SIGNAL(selectionChanged()),this,SLOT(chapterChanged()));
    setTextAlignment(2,Qt::AlignRight);
    setTextAlignment(3,Qt::AlignRight);
}

void _k9ItemWidget::titleChanged() {
    if (title->isSelected())  {
        setCheckState(0,Qt::Checked);
        astTitle->titleSelected(title);
    } else {
        setCheckState(0,Qt::Unchecked);
        astTitle->titleUnSelected(title);
    }
}

void _k9ItemWidget::chapterChanged() {
    if (chapter->getSelected())
        setCheckState(0,Qt::Checked);
    else
        setCheckState(0,Qt::Unchecked);
}

bool _k9ItemWidget::operator< ( const QTreeWidgetItem & other ) const  {
    _k9ItemWidget *it=(_k9ItemWidget*)&other;
    if (it->type()!= k9astTitles::TITLE)
        return true;
    switch (treeWidget()->sortColumn()) {
    case 1:
        return title->getnumTitle() <((_k9ItemWidget*)&other)->title->getnumTitle();
    case 2:
        return title->getlength() <((_k9ItemWidget*)&other)->title->getlength();
    case 3:
        return title->gettotalsize_mb() <((_k9ItemWidget*)&other)->title->gettotalsize_mb();
    default:
        return true;
    }
}

k9astTitles::k9astTitles(QWidget* parent,k9DVD *_dvd)
        : QWidget(parent) {
    Ui_astTitles.setupUi(this);
    m_dvd=_dvd;
    m_src=Ui_astTitles.twSource;
    m_preview=NULL;
    m_currentItem=NULL;
    loadPlayer();
    QTreeWidgetItem *header=m_src->headerItem();
    header->setText(0,"");
    QCheckBox *ck=new QCheckBox(m_src);
    ck->setChecked(true);
    connect(ck,SIGNAL(toggled( bool )),this,SLOT(selectAll(bool)));
    connect(m_src,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(ItemChanged(QTreeWidgetItem*,int)));

    m_src->setItemWidget(header,0,ck);
    m_src->setSortingEnabled(true);
}


void k9astTitles::loadPlayer() {
    k9Config config;
    m_useXine=config.getUseXine();
    m_usePhonon=config.getUsePhonon();
    m_useMplayer=config.getUseMplayer();

    if (m_preview) {
        delete m_preview;
        delete m_layout;
    }

    if (m_useXine) {
#ifndef BACKLITE
        m_preview=new k9xine(Ui_astTitles.wPreview);
#endif
    } else if (m_usePhonon) {
        m_preview=new k9Phonon(Ui_astTitles.wPreview);
    } else if (m_useMplayer)
        m_preview=new K9Mplayer(Ui_astTitles.wPreview);
    else
        m_preview=new kViewMPEG2(Ui_astTitles.wPreview);

    connect(this,SIGNAL(sigshowPreview( k9DVD*, k9DVDTitle*,int )),m_preview,SLOT(open( k9DVD*, k9DVDTitle*,int )));
    connect(this,SIGNAL(sigstopPreview()),m_preview,SLOT(bStopClick()));

    m_layout=new QGridLayout(Ui_astTitles.wPreview);
    m_layout->addWidget(m_preview,0,0);
    m_preview->show();

}

void k9astTitles::itemActivated(QTreeWidgetItem* _item,int) {
    _k9ItemWidget *item=(_k9ItemWidget*)_item;
    if (item) {
       if (item->type()==TITLE)
          emit sigshowPreview(m_dvd,item->title,1);
    }

}

void k9astTitles::CurrentItemChanged(QTreeWidgetItem *_new,QTreeWidgetItem *_old) {
    if (m_updating)
        return;
    _k9ItemWidget *previous,*current;
    previous=(_k9ItemWidget*)_old;
    current=(_k9ItemWidget*)_new;
    if (current) {
        if (current->type() ==TITLE) {
/*            for (int i=0;i <m_src->topLevelItemCount();i++) {
                _k9ItemWidget *item=(_k9ItemWidget*) m_src->topLevelItem(i);
                if (item != current)
                    item->setExpanded(false);
                else
                    item->setExpanded(true);
            }
*/
            m_currentItem=current;
            itemActivated(current,0);
        }
    }
}

void k9astTitles::ItemChanged(QTreeWidgetItem *_item,int col) {
    Q_UNUSED(col);
    if (m_updating)
        return;
    m_updating=true;
    _k9ItemWidget *item=(_k9ItemWidget*)_item;
    if ((item->type()==TITLE)) {
        for (int i=0;i< item->childCount();i++) {
            _k9ItemWidget *it=(_k9ItemWidget*)item->child(i);
            it->setCheckState(0,item->checkState(0));
        }

    } else if (item->type()==CHAPTER) {
        _k9ItemWidget *it=(_k9ItemWidget*)_item->parent();
        bool selected=false;
        for (int i=0;i<it->childCount();i++) {
            _k9ItemWidget *it2=(_k9ItemWidget*)it->child(i);
            if (it2->checkState(0)==Qt::Checked)
                selected=true;
        }
        if (!selected)
            it->setCheckState(0,Qt::Unchecked);
        else
            it->setCheckState(0,Qt::Checked);
    }
    m_updating=false;
}

k9astTitles::~k9astTitles() {
    emit sigstopPreview();
}

void k9astTitles::unloadPreview() {
    if (m_preview)
        delete m_preview;
    m_preview =NULL;
}
/*$SPECIALIZATION$*/

void k9astTitles::stopPreview() {
    emit sigstopPreview();
}

void k9astTitles::titleSelected(k9DVDTitle *_title) {
    if (!m_updating)
        emit addTitle(_title);
}

void k9astTitles::titleUnSelected(k9DVDTitle *_title) {
    if (!m_updating)
        emit removeTitle(_title);
}

void k9astTitles::selectAll(bool _state) {
    bool oldState;
    for (int i=0; i < m_src->topLevelItemCount();i++) {
        _k9ItemWidget *item=(_k9ItemWidget*) m_src->topLevelItem(i);
        oldState=item->title->isSelected();
        if (_state) {
            m_updating=true;
            // all streams are checked by default
            for (int j=0;j < item->title->getaudioStreamCount();j++) {
                item->title->getaudioStream(j)->setselected(true);
                if (j==0)
                    item->title->setDefAudio(item->title->getaudioStream(j));
            }
            for (int j=0;j < item->title->getsubPictureCount();j++) {
                item->title->getsubtitle(j)->setselected(true);
                if (j==0)
                    item->title->setDefSubtitle(item->title->getsubtitle(j));
            }
            item->title->setvideoSelected(true);
            m_updating=false;
            if (_state!=oldState)
                emit addTitle(item->title);
        } else {
            m_updating=true;
            item->title->setSelected(false);
            item->title->setDefSubtitle(NULL);
            item->title->setDefAudio(NULL);
            m_updating=false;
            if (_state!=oldState)
                emit removeTitle(item->title);
        }
    }
}

void k9astTitles::fill() {
    m_src->clear();
    m_currentItem=NULL;

    for (int i=0;i < m_dvd->gettitleCount();i++) {
        k9DVDTitle *title=m_dvd->gettitle(i);
        if (title->getIndexed()) {
            m_updating=true;

            _k9ItemWidget *item=new _k9ItemWidget(title,m_src,k9astTitles::TITLE );
            item->astTitle=this;
            item->setText(0,"");
            item->setText(1,QString("%1").arg(title->getname()));
	    item->setText(2,QString("%1").arg(title->getlength().toString("hh:mm:ss")));	
            item->setText(3,QString("%1 %2").arg(title->gettotalsize_mb(),0,'f',2).arg(i18n("MB")));
            if (!m_currentItem) {
                m_currentItem=item;
            }
            addChapters(item,title);
            // all streams are checked by default
            title->setSelected(true);
            m_updating=false;
            emit addTitle(title);
        }
    }
    m_src->sortItems(1,Qt::AscendingOrder);
}

void k9astTitles::addChapters(QTreeWidgetItem *_parent,k9DVDTitle *_title) {

    //_parent->setExpanded(true);

    int ch=0;
    for (int i=0;i< _title->getchapterCount();i++) {
        _k9ItemWidget *it =new _k9ItemWidget(_title->getChapter(i),_parent,k9astTitles::CHAPTER);
        it->setText(1,i18n("chapter %1").arg(++ch));
        QString s;

        s=QString("%1").arg((double)(_title->getChapter(i)->getsectors()) /512,0,'f',2);
	it->setText(2,QString("%1").arg(_title->getChapter(i)->getLength().toString("hh:mm:ss")));
        it->setText(3,i18n("%1 MB").arg(s));
    }
    for (int j=0;j <_title->getTitles().count();j++) {
        k9DVDTitle *title2=_title->getTitles().at(j);
        for (int i=0;i< title2->getchapterCount();i++) {
            _k9ItemWidget *it =new _k9ItemWidget(title2->getChapter(i),_parent,k9astTitles::CHAPTER);
            it->setText(1,i18n("chapter %1").arg(++ch));
            QString s;
            s=QString("%1").arg((double)(title2->getChapter(i)->getsectors()) /512,0,'f',2);
	    it->setText(2,QString("%1").arg(title2->getChapter(i)->getLength().toString("hh:mm:ss")));
            it->setText(3,i18n("%1 MB").arg(s));
        }

    }

}

void k9astTitles::ItemExpanded(QTreeWidgetItem *) {
   resizeEvent(0);
}

void k9astTitles::resizeEvent ( QResizeEvent *  ) {
    m_src->setColumnWidth(1,m_src->viewport()->width());
    m_src->resizeColumnToContents(0);
    m_src->resizeColumnToContents(2);
    m_src->resizeColumnToContents(3);
    m_src->setColumnWidth(1,m_src->viewport()->width()-m_src->columnWidth(0)-m_src->columnWidth(2)-m_src->columnWidth(3)-10);
}

void k9astTitles::updateTitles() {
    for (int j=0;j< m_src->topLevelItemCount();j++) {
        _k9ItemWidget *item=(_k9ItemWidget*)m_src->topLevelItem(j);
        if (item->checkState(0)==Qt::Unchecked) {
            item->title->setSelected(false);

        } else if (!item->title->isSelected()) {
            m_updating=true;
            
	    for(int iChapter=0;iChapter<item->childCount();iChapter++) {
		_k9ItemWidget *itc=(_k9ItemWidget*)item->child(iChapter);
		itc->chapter->setSelected(itc->checkState(0)==Qt::Checked);
	    }
	    item->title->setSelected(true);
            emit addTitle(item->title);

            m_updating=false;

        }
    }
//reselectionner les titres qui ont été recochés
}

bool k9astTitles::isOk() {
    bool ok=false;
    for (int i=0;i < m_dvd->gettitleCount();i++) {
        k9DVDTitle *title=m_dvd->gettitle(i);
        ok= (title->getIndexed() && title->isSelected())?true:ok;

    }
    return ok;

}

