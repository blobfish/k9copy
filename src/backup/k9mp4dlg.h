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

#ifndef K9MP4DLG_H
#define K9MP4DLG_H
#include "k9common.h"
#include "ui_mp4dlg.h"
#include <QTimer>
#include <qimage.h>
#include <qdatetime.h>
#include <QDialog>
class k9DrawImage;

class k9MP4Dlg : public QDialog
{
  Q_OBJECT

public:
  k9MP4Dlg(QWidget* parent = 0);
  ~k9MP4Dlg();
  /*$PUBLIC_FUNCTIONS$*/
  void setfps(QString _fps);
  void setsize(QString _size);
  void setbitrate(QString _bitrate);
  void setremain(QString _remain);
  void setProgress(int _progress);
  void setTitleLabel(QString _titleLabel);
  void setImage(QString _fileName) ;

signals:
  /*$PUBLIC_SLOTS$*/
  void sigCancel();
private:
    k9DrawImage *m_wimage;
    QTime m_timer;
    QTimer m_timerUpdate;
    Ui::MP4Dlg Ui_MP4Dlg;

    QString m_titleLabel;
    QString m_fps;
    QString m_remain;
    int m_progress;
    QString m_bitrate;
    QString m_size;
    QString m_fileName;
    bool m_update;
public slots:
  void Update();
protected slots:
  /*$PROTECTED_FUNCTIONS$*/
  void Cancel();

};

#endif

