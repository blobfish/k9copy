#ifndef K9LOGVIEW_H
#define K9LOGVIEW_H

#include <QWidget>
#include <QTimer>
#include "k9log.h"
#include <QObject>
#include <QListWidget>
namespace Ui {
    class k9LogView;
}

class k9LogView : public QWidget {
    Q_OBJECT
public:
    k9LogView(QWidget *parent = 0);
    ~k9LogView();
    QListWidget *list();
protected:
    void changeEvent(QEvent *e);
    void closeEvent ( QCloseEvent * event );
private slots:
    void timerDone();
private:
    Ui::k9LogView *m_ui;
    bool m_started;
};

#endif // K9LOGVIEW_H
