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

#ifndef K9ASTSTREAMS_H
#define K9ASTSTREAMS_H

#include "k9common.h"
#include "ui_aststreams.h"
#include "k9assistant.h"
#include <QCheckBox>

class k9DVD;
class k9DVDTitle;
class k9DVDAudioStream;
class k9DVDSubtitle;
class k9astStreams : public QWidget
{
  Q_OBJECT

public:
  k9astStreams(k9Assistant* parent,k9DVD *_dvd );
  ~k9astStreams();
  enum eStreamType {AUDIO,SUBTITLE};
  void updateStreams();
  void selectOne(bool);
  void updateColumns();
  void hideSubtitles(bool _value);

  /*$PUBLIC_FUNCTIONS$*/
public slots:
  /*$PUBLIC_SLOTS$*/
  void addTitle(k9DVDTitle *_title);
  void removeTitle(k9DVDTitle *_title);
protected:
  /*$PROTECTED_FUNCTIONS$*/
   void resizeEvent ( QResizeEvent * event );
   void showEvent ( QShowEvent * event );
protected slots:
  /*$PROTECTED_SLOTS$*/
  void itemChanged(QTreeWidgetItem *_item,int _column);
private:
  Ui::astStreams Ui_astStreams;
  k9Assistant *m_parent;
  QTreeWidget *m_streams;
  k9DVD *m_dvd;
  bool m_locked;
  bool m_selectOne;
};



class _k9StreamWidget : public QObject,public QTreeWidgetItem {
Q_OBJECT
public:
    _k9StreamWidget(k9DVDAudioStream *_stream,QTreeWidgetItem *_treeWidgetItem);
    _k9StreamWidget(k9DVDSubtitle *_stream,QTreeWidgetItem *_treeWidgetItem);
    k9DVDAudioStream *audioStream;
    k9DVDSubtitle *subtitle;
    k9astStreams *astStreams;
    void setDefault(bool _state) { setCheckState(1,_state?Qt::Checked:Qt::Unchecked);} ;
    bool isDefault() { return (checkState(1)==Qt::Checked);};
    void showDefault( bool _state) {if (_state) setCheckState(1,Qt::Unchecked); else setData(1,Qt::CheckStateRole,QVariant()); };
    bool operator< ( const QTreeWidgetItem & other ) const;
private:
};
#endif

