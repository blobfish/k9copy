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

#include "k9aststreams.h"
#include "k9dvd.h"
#include "k9dvdtitle.h"
#include <QDebug>

class _k9TitleWidget : public QTreeWidgetItem {
public:
    _k9TitleWidget(QTreeWidget *_parent,k9DVDTitle *_title):QTreeWidgetItem(_parent,QTreeWidgetItem::UserType+5) {
        title=_title;
    }
    k9DVDTitle *title;
    bool operator< ( const QTreeWidgetItem & other ) const  {
        return title->getnumTitle() <((_k9TitleWidget*)&other)->title->getnumTitle();
    }

};

bool _k9StreamWidget::operator < ( const QTreeWidgetItem & other ) const  {
    bool res=false;
    _k9StreamWidget *it1=(_k9StreamWidget*) this;
    _k9StreamWidget *it2=(_k9StreamWidget*) &other;
    if (it1->type() != it2->type()) {
        res=it1->type()==k9astStreams::AUDIO;
    } else {
        if (it1->type()==k9astStreams::AUDIO)
            res=(it1->audioStream->getID() < it2->audioStream->getID());
        else
            res=(it1->subtitle->getID().at(0) < it2->subtitle->getID().at(0));
    }

    return res;
}

_k9StreamWidget::_k9StreamWidget(k9DVDAudioStream *_stream,QTreeWidgetItem *_treeWidgetItem):QObject(0),QTreeWidgetItem(_treeWidgetItem,k9astStreams::AUDIO) {
    audioStream=_stream;
    subtitle=NULL;
    showDefault(false);
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    setCheckState(0,_stream->getselected()?Qt::Checked:Qt::Unchecked);
    setText(0,i18n("%4 - %1 %2 %3 ch").arg(_stream->getlanguage()).arg(_stream->getformat()).arg(_stream->getchannels()).arg(_stream->getID()));
    setIcon(0,SmallIcon("sound"))    ;
    //setText(1,i18n("%1 MB",_stream->getsize_mb()));
}

_k9StreamWidget::_k9StreamWidget(k9DVDSubtitle *_stream,QTreeWidgetItem *_treeWidgetItem):QObject(0),QTreeWidgetItem(_treeWidgetItem,k9astStreams::SUBTITLE) {
    audioStream=NULL;
    subtitle=_stream;
    showDefault(false);
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);
    setCheckState(0,_stream->getselected()?Qt::Checked:Qt::Unchecked);
    setText(0,i18n("%3 - %1 %2").arg(_stream->getlanguage()).arg(_stream->getcontent()).arg(_stream->getID().first()));
    setIcon(0,SmallIcon("subtitle"));

    //setText(1,i18n("%1 MB",_stream->getsize_mb()));
}



k9astStreams::k9astStreams(k9Assistant* parent,k9DVD *_dvd)
        : QWidget(parent) {
    Ui_astStreams.setupUi(this);
    m_parent=parent;
    m_dvd=_dvd;
    m_streams=Ui_astStreams.lstreams;
    m_streams->setRootIsDecorated(false);
    m_streams->setSortingEnabled(true);
    m_locked=false;
    m_selectOne=false;
}

k9astStreams::~k9astStreams() {
}

void k9astStreams::showEvent(QShowEvent *) {
    resizeEvent(0);
}

void k9astStreams::resizeEvent ( QResizeEvent *  ) {
    if ( ! m_streams->isColumnHidden(1)) {
    	m_streams->setColumnWidth(0,m_streams->viewport()->width());
    	m_streams->resizeColumnToContents(1);
    	m_streams->setColumnWidth(0,m_streams->viewport()->width()-m_streams->columnWidth(1));
    }

}

void k9astStreams::itemChanged(QTreeWidgetItem *_item,int _column) {

    if (_item->type()==QTreeWidgetItem::UserType+5)
        return;
    switch (_column) {
    case 0: {
        _k9StreamWidget *item2=(_k9StreamWidget*)_item;

        if ((!m_locked) && m_selectOne &&(_item->type()==AUDIO || _item->type()==SUBTITLE  ) && _item->checkState(0)==Qt::Checked) {
            m_locked=true;
            QTreeWidgetItem *parent=_item->parent();
            for (int i=0; i <parent->childCount();i++) {
                _k9StreamWidget *item=(_k9StreamWidget*) parent->child(i);
                if ( (item!=item2) && ( item->type()==item2->type()))
                    item->setCheckState(0,Qt::Unchecked);
            }
            m_locked=false;
        }
        if (_item->checkState(0)==Qt::Checked) {
            item2->setDefault(false);
            item2->showDefault(true);
        } else
            item2->showDefault(false);

    }
    break;
    case 1: {
        //only one stream /type /type can be selected
        if (_item->checkState(1)==Qt::Checked) {
            QTreeWidgetItem *parent=_item->parent();
            for (int i=0; i <parent->childCount();i++) {
                _k9StreamWidget *item=(_k9StreamWidget*) parent->child(i);
                _k9StreamWidget *item2=(_k9StreamWidget*)_item;
                //default checkbox is unchecked only if the stream is selected
                if ( (item!=_item) && ( item->type()==item2->type()) && (item->checkState(0)==Qt::Checked))
                    item->setDefault(false);
            }
        }
    }
    break;
    default:
        break;
    }
}


/*$SPECIALIZATION$*/

void k9astStreams::addTitle(k9DVDTitle *_title) {
    _k9TitleWidget *item=new _k9TitleWidget(m_streams,_title);
    item->setText(0,_title->getname());
    for (int i=0; i < _title->getaudioStreamCount();i++) {
        new _k9StreamWidget(_title->getaudioStream(i),item);
    }
    for (int i=0;i <_title->getsubPictureCount();i++) {
        _k9StreamWidget *subtitle=new _k9StreamWidget(_title->getsubtitle(i),item);
        if (m_parent->destination()==dstAudio)
		subtitle->setHidden(true);
    }
    item->setExpanded(true);
    m_streams->sortItems(0,Qt::AscendingOrder);
    updateColumns();
}

void k9astStreams::removeTitle(k9DVDTitle *_title) {
    for (int i=0; i< m_streams->topLevelItemCount();i++) {
        _k9TitleWidget *item=(_k9TitleWidget*)m_streams->topLevelItem(i);
        if (item->title == _title) {
            m_streams->takeTopLevelItem(i);
        }
    }

}

void k9astStreams::updateStreams() {
    for (int i=0; i< m_streams->topLevelItemCount();i++) {
        _k9TitleWidget *itemTitle=(_k9TitleWidget*)m_streams->topLevelItem(i);
        itemTitle->title->setDefAudio(NULL);
        itemTitle->title->setDefSubtitle(NULL);
        for (int c=0;c < itemTitle->childCount();c++) {
            if (m_streams->topLevelItem(i)->childCount() ==0)
                continue;
            _k9StreamWidget *w= (_k9StreamWidget*)itemTitle->child(c);
            if (w->type()==AUDIO) {
                bool state=(w->checkState(0)==Qt::Checked)?true:false;
                w->audioStream->setselected(state);
                if (w->audioStream->getselected() && (w->isDefault())) {
                    w->audioStream->gettitle()->setDefAudio(w->audioStream);
                }
            }
            if (w->type()==SUBTITLE) {
                bool state=(w->checkState(0)==Qt::Checked)?true:false;
                w->subtitle->setselected(state);
                if (w->subtitle->getselected() && (w->isDefault())) {
                    w->subtitle->gettitle()->setDefSubtitle(w->subtitle);
                }
            }
        }
    }
}

void k9astStreams::selectOne( bool _value) {
    m_selectOne=_value;
    //keep only the first selected stream from each title (avi)
    for (int i=0;m_selectOne && (i< m_streams->topLevelItemCount());i++) {
        bool audio=false;
        bool subp=false;
        for (int c=0;c < m_streams->topLevelItem(i)->childCount()-1;c++) {
            _k9StreamWidget *w=(_k9StreamWidget*)m_streams->topLevelItem(i)->child(c);
            if ( (w->type()==AUDIO)&&( w->checkState(0)==Qt::Checked) && (!audio)) {
                itemChanged(w,0);
                audio=true;
            }
            if ( (w->type()==SUBTITLE)&&( w->checkState(0)==Qt::Checked) && (!subp)) {
                itemChanged(w,0);
                subp=true;
            }
        }
    }
}

void k9astStreams::hideSubtitles(bool _value) {
   for (int i=0;i< m_streams->topLevelItemCount();i++) {
        for (int c=0;c < m_streams->topLevelItem(i)->childCount();c++) {
            _k9StreamWidget *w=(_k9StreamWidget*)m_streams->topLevelItem(i)->child(c);      
            if ( w->type()==SUBTITLE) {
                w->setHidden(_value);
		if (_value)
		    w->setCheckState(0,Qt::Unchecked);
            }
        }
    }

}

void k9astStreams::updateColumns() {
    bool visible;
    visible =((m_parent->destination()==dstDVD) ||(m_parent->destination()==dstFolder) || (m_parent->destination()==dstIso)) ;
 //   qDebug() << "visible :" << visible << " destination :" << m_parent->destination();
    m_streams->setColumnHidden(1,!visible);
    if (visible) {
        m_streams->setColumnWidth(0,m_streams->viewport()->width());
        m_streams->resizeColumnToContents(1);
        m_streams->setColumnWidth(0,m_streams->viewport()->width()-m_streams->columnWidth(1));
    }
}
