#include "k9logview.h"
#include "k9common.h"
#include "ui_k9logview.h"
#include <QDebug>

k9LogView::k9LogView(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::k9LogView)
{
    m_ui->setupUi(this);

    m_started=true;
    QTimer::singleShot(1000,this,SLOT(timerDone()));

}

k9LogView::~k9LogView()
{
    delete m_ui;
}

void k9LogView::closeEvent ( QCloseEvent *  ) {
    m_started=false;
}


void k9LogView::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void k9LogView::timerDone() {
 k9logEntry *logItem;
 while (k9log::count()>0 && m_started) {
     logItem=k9log::dequeue();
     if (logItem) {
        QIcon *icon;
        switch(logItem->level) {
            case k9logLevel::ERROR :
               icon=new QIcon(SmallIcon("dialog-error"));
               break;
            case k9logLevel::WARNING :
               icon=new QIcon(SmallIcon("dialog-warning"));
               break;
            default:
               icon=new QIcon(SmallIcon("dialog-information"));
               break;
        }
        m_ui->lwLogs->addItem(new QListWidgetItem(*icon,logItem->text,0));
        m_ui->lwLogs->scrollToBottom();
        delete icon;
     }
 }
 if (m_started)
    QTimer::singleShot(1000,this,SLOT(timerDone()));

}

QListWidget *k9LogView::list() {
    return m_ui->lwLogs;
}
