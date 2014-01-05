//
// C++ Interface:
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9BURNPROGRESS_H
#define K9BURNPROGRESS_H

#include "k9common.h"
#include "ui_progress.h"
#include "k9drawimage.h"
#include "k9logview.h"
#include <QImage>
#include <QTimer>
#include <QDialog>
class k9BurnProgress : public QDialog {
    Q_OBJECT

public:
    k9BurnProgress(QWidget* parent = 0);
    ~k9BurnProgress();
    virtual k9Process * getProcess() const;
    virtual void setProgress(long _position,long _total);
    virtual void setLabelText(const QString _text);
    virtual void setElapsed(const QString _text);
    virtual void setTitle(const QString _text);
    virtual void setMovie(QString _fileName);

    virtual int execute();

    bool getCanceled() const;

    /*$PUBLIC_FUNCTIONS$*/

public slots:
    /*$PUBLIC_SLOTS$*/

protected:
    /*$PROTECTED_FUNCTIONS$*/
    k9Process *m_process;
    bool m_canceled;
    void closeEvent ( QCloseEvent * event );
protected slots:
    /*$PROTECTED_SLOTS$*/
    void bCancelClick();
    void gbOutputToggled(bool state);

private:
    Ui::Progress Ui_Progress;
    k9LogView *m_logView;
    int m_outputSize;
};

#endif

