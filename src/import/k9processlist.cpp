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


#include "k9processlist.h"
#include <qstyle.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qlistview.h>
#include <qrect.h>
#include <qprogressbar.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <kpushbutton.h>

class _k9ProcessListItem : public QTreeWidgetItem {
public:
    QProgressBar *progress;

    _k9ProcessListItem(QTreeWidget *_listview,const QString & _text):QTreeWidgetItem(_listview) {
        m_fileName="";
        m_pos=0;
        m_num=_listview->invisibleRootItem()->childCount();
        setText(1,_text);
        progress=new QProgressBar();
        progress->setValue(100);
        progress->setMaximum(100);
        progress->resize(100,40);
        treeWidget()->setItemWidget ( this, 0, progress);
        _listview->resizeColumnToContents(1);

    }
    void setFileName(const QString &_fileName) {
        m_fileName=_fileName;
    }
    void setPos(double _pos) {
        m_pos=_pos;
    }

    QString getFileName() const {
        return m_fileName;
    }

    double getPos() const {
        return m_pos;
    }
    bool operator< ( const QTreeWidgetItem & other ) const  {
        return m_num <((_k9ProcessListItem*)&other)->m_num;
    }

private:
    QString m_fileName;
    double m_pos;
    int m_num;
};


k9ProcessList::k9ProcessList(QWidget* parent) : QDialog(parent) {
    Ui_processList.setupUi(this);
    m_maxProcess=4;
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(timeout()));
    Ui_processList.bCancel->setEnabled(false);
#ifndef BACKLITE
    Ui_processList.bCancel->setGuiItem(KStandardGuiItem::Stop);
#else
    Ui_processList.bCancel->setText(i18n("Stop"));
    Ui_processList.bCancel->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));
#endif

}

k9ProcessList::~k9ProcessList() {
    qDeleteAll(m_processes);
}
/*$SPECIALIZATION$*/
void k9ProcessList::timeout() {
    if (m_waitSync) {
        m_eventLoop.exit();
        m_waitSync=false;
    }
}

void k9ProcessList::bCancelClick() {
    m_cancel=true;
    foreach (k9Process *p,m_processes) {
        if (p->isRunning())
            p->kill();
    }

}

void k9ProcessList::wait(int _msec) {
    m_waitSync=true;
    m_timer.start(_msec);
    m_eventLoop.exec();
}

int k9ProcessList::getNbRunning() {
    int res=0;
    for (int i=0;i<m_processes.count();i++) {
        k9Process *p=m_processes.at(i);
        if (p->isRunning())
            res++;
    }
    return res;
}


void k9ProcessList::execute() {
    Ui_processList.bCancel->setEnabled(true);
    m_cancel=false;
    m_error=false;
    k9Process *p=NULL;
    foreach (p,m_processes) {
        while (getNbRunning() >=m_maxProcess && ! m_cancel) {
            wait(1000);
        }
        if (m_cancel)
            break;
        if (!p->start())
            m_error=true;
    }
    //waiting for processes
    foreach (k9Process *p,m_processes) {
        if (p->isRunning()) {
            p->sync();
        }
    }
    Ui_processList.bCancel->setEnabled(false);
}

void k9ProcessList::addProgress(const QString &_text) {
    QTreeWidgetItem *item = new _k9ProcessListItem(Ui_processList.lProcess,_text);
//    item->setPixmap(0,QPixmap::grabWidget(&b,0,0,b.width(),b.height()));
}

void k9ProcessList::setProgress (k9Process * _process,int _position, int _total) {
    _k9ProcessListItem *it =(_k9ProcessListItem*)m_items[_process];
    it->progress->setValue(_position);
//    it->setPixmap(0,QPixmap::grabWidget(&b,0,0,b.width(),b.height()));
}

void k9ProcessList::setText(k9Process *_process, const QString &_text,int _col) {
    QTreeWidgetItem *it =m_items[_process];
    it->setText(_col+1,_text);
    Ui_processList.lProcess->resizeColumnToContents(_col+1);
}

void k9ProcessList::setFileName(k9Process *_process,const QString &_fileName) {
    _k9ProcessListItem *it = (_k9ProcessListItem*)m_items[_process];
    it->setFileName(_fileName);
}

void k9ProcessList::setPos(k9Process *_process,double _pos) {
    _k9ProcessListItem *it = (_k9ProcessListItem*)m_items[_process];
    it->setPos(_pos);
}

k9Process *k9ProcessList::addProcess(const QString &label) {
    QString name=QString("process%1").arg(m_items.count()) ;
    k9Process *process=new k9Process(this,name.toUtf8());
    m_processes.append(process);
    QTreeWidgetItem *item = new _k9ProcessListItem(Ui_processList.lProcess,label);
    m_items[process]=item;
    setProgress(process,0,100);
    connect(process,SIGNAL(processExited( k9Process* )),this,SLOT(processExited(k9Process*)));
    return process;
}

void k9ProcessList::processExited(k9Process *_process) {
    if (!_process->normalExit())
        m_cancel=true;
    else if (_process->exitStatus() !=0 )
        m_error=true;

}

void k9ProcessList::clear() {
    m_processes.clear();
    m_items.clear();
}

int k9ProcessList::getMaxProcess() const {
    return m_maxProcess;
}


void k9ProcessList::setMaxProcess(int _value) {
    m_maxProcess = _value;
}


bool k9ProcessList::getCancel() const {
    return m_cancel;
}


bool k9ProcessList::getError() const {
    return m_error;
}
