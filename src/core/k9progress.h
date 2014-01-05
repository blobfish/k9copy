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

#ifndef K9PROGRESS_H
#define K9PROGRESS_H

#include "k9common.h"
#include "ui_progress.h"
#include <QImage>
#include <QTimer>
#include "k9drawimage.h"
#include <QMovie>
#include <QDialog>
class k9Progress : public QDialog {
    Q_OBJECT

public:
    k9Progress(QWidget* parent = 0);
    ~k9Progress();
    virtual k9Process * getProcess() const;
    virtual void setProgress(long _position,long _total);
    virtual void setLabelText(const QString _text);
    virtual void setElapsed(const QString _text);
    virtual void setTitle(const QString _text);
    virtual void setImage(QString _fileName) ;
    virtual void setImage(const QImage &_image);
    virtual void setMovie(QString _fileName);

    virtual int execute();

    bool getCanceled() const;

    /*$PUBLIC_FUNCTIONS$*/

public slots:
    /*$PUBLIC_SLOTS$*/

protected:
    /*$PROTECTED_FUNCTIONS$*/
    k9Process *m_process;
    k9DrawImage *m_wimage;
    bool m_canceled;
    void closeEvent ( QCloseEvent * event );
    void showEvent ( QShowEvent * event );
protected slots:
    /*$PROTECTED_SLOTS$*/
    void bCancelClick();
    void gbOutputToggled(bool state);

private:
    Ui::Progress Ui_Progress;
    int m_outputSize;
    QMovie *m_movie;
};

#endif

