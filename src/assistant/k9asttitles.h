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

#ifndef K9ASTTITLES_H
#define K9ASTTITLES_H

#include "k9common.h"
#include "ui_asttitles.h"


class kViewMPEG2;
class _k9ItemWidget;
class k9DVD;
class k9DVDTitle;
class k9DVDChapter;
class k9astTitles : public QWidget
{
  Q_OBJECT

public:
  enum itemType {TITLE,CHAPTER,AUDIO,SUBTITLE,VIDEO};

  k9astTitles(QWidget* parent,k9DVD *_dvd);
  ~k9astTitles();
  /*$PUBLIC_FUNCTIONS$*/
  void fill();
  void updateTitles();
  void titleSelected(k9DVDTitle *_title);
  void titleUnSelected(k9DVDTitle *_title);
  void stopPreview();
  void unloadPreview();
  bool isOk();
  void loadPlayer();
public slots:
  /*$PUBLIC_SLOTS$*/

protected:
  /*$PROTECTED_FUNCTIONS$*/
    void resizeEvent ( QResizeEvent * event );
    void addChapters(QTreeWidgetItem *_parent,k9DVDTitle *_title);
protected slots:
  /*$PROTECTED_SLOTS$*/
  void itemActivated(QTreeWidgetItem*,int);
  void CurrentItemChanged(QTreeWidgetItem *_new,QTreeWidgetItem *_old);
  void ItemChanged(QTreeWidgetItem*,int);
  void ItemExpanded(QTreeWidgetItem*);
  void selectAll(bool);
private:
  bool m_useXine,m_usePhonon,m_useMplayer;
  Ui::astTitles Ui_astTitles;
  k9DVD *m_dvd;
  QTreeWidget *m_src;
  bool m_updating;
  QWidget *m_preview;
  QGridLayout *m_layout;
  _k9ItemWidget *m_currentItem;
signals:
  void addTitle(k9DVDTitle *_title);
  void removeTitle(k9DVDTitle *_title);
  void sigshowPreview( k9DVD*, k9DVDTitle*,int );
  void sigstopPreview();
};


class _k9ItemWidget : public QObject,public QTreeWidgetItem {
Q_OBJECT
public:
    _k9ItemWidget(k9DVDTitle *_title,QTreeWidget *_treeWidget,int _type);
    _k9ItemWidget(k9DVDChapter *_chapter,QTreeWidgetItem *_treeWidget,int _type);
    k9DVDTitle *title;
    k9DVDChapter *chapter;
    k9astTitles *astTitle;
    bool operator< ( const QTreeWidgetItem & other ) const ;
private slots:
   void titleChanged();
   void chapterChanged();
};


#endif

