//
// C++ Interface:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9PROCESSLIST_H
#define K9PROCESSLIST_H
#include "k9common.h"
#include "ui_processList.h"
#include <qmap.h>
#include <qtimer.h>
#include <QDialog>
class k9ProcessList : public QDialog {
    Q_OBJECT

public:
    k9ProcessList(QWidget* parent = 0);
    ~k9ProcessList();
    /*$PUBLIC_FUNCTIONS$*/
    void execute();
    void addProgress(const QString &_text);
    void setProgress (k9Process * _process,int _position, int _total);
    void setText(k9Process *_process, const QString &_text,int _col);
    void setFileName(k9Process *_process,const QString &_fileName);
    void setPos(k9Process *_process,double _pos);
    k9Process *addProcess(const QString &label);

    void setMaxProcess(int _value);
    int getMaxProcess() const;
    bool getCancel() const;
    void clear();

    bool getError() const;
	
private:
    Ui::processList Ui_processList;
public slots:
    /*$PUBLIC_SLOTS$*/

protected:
    /*$PROTECTED_FUNCTIONS$*/
    int m_maxProcess;
    QMap <k9Process*,QTreeWidgetItem*> m_items;
    QList <k9Process*> m_processes;
    QEventLoop m_eventLoop;
    QTimer m_timer;
    bool m_cancel;
    bool m_error;
    bool m_waitSync;
    int getNbRunning();
    void wait(int _msec);

protected slots:
    /*$PROTECTED_SLOTS$*/
    void timeout();
    virtual void bCancelClick();
    virtual void processExited(k9Process *);
};

#endif

