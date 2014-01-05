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

#ifndef K9MENCODERCMDGEN_H
#define K9MENCODERCMDGEN_H

#include "k9common.h"
#include "ui_mencoderCmdGen.h"
#include <QScrollArea>
#include <QLayout>
#include <qdom.h>
#include <QLabel>
#include <QTreeWidget>
class _k9CheckListItem;

class k9Grid:public QObject {
public:
   k9Grid (int columns,QWidget *parent) :QObject(parent),m_columns(columns){
      m_widget=new QFrame();
      m_widget->resize(300,20);
      m_layout=new QGridLayout(m_widget);
      m_layout->setSpacing(2);
      m_col=0;m_row=0;
      m_rowHeight=0;
   };
   void addWidget(QWidget *_widget) {
       int h=_widget->height();
       m_rowHeight=qMax(h,m_rowHeight);
       _widget->setParent(m_widget);
       m_layout->addWidget(_widget,m_row,m_col);
       m_col++;
       if (m_col >=m_columns) {
           m_widget->resize(m_widget->width(),m_widget->height()+m_rowHeight);
	   m_widget->setMinimumSize(0,m_widget->height());
	   m_col=0;
	   m_row++;
	   m_rowHeight=0;
       }
   }
   QWidget *widget() { return m_widget;}
private:
   int m_columns;
   int m_col,m_row;
   int m_rowHeight;
   QFrame *m_widget;
   QGridLayout *m_layout;
};


class k9MencoderCmdGen : public QWidget
{
  Q_OBJECT

public:
  k9MencoderCmdGen(QWidget* parent,const QString &_cmd,const QString &_encoder);
  ~k9MencoderCmdGen();
  /*$PUBLIC_FUNCTIONS$*/
  static bool getMencoderOptions(QString &_cmd,const QString &_encoder);
public slots:
  /*$PUBLIC_SLOTS$*/
private:
   Ui::mencoderCmdGen Ui_mencoderCmdGen;
   QTreeWidget *listView;
   QStackedWidget *wsOptions;

   QScrollArea *m_scrollView;
   k9Grid *m_grid;
   k9Grid *m_hbox;
   int m_row,m_page,m_cpt;
   QDomDocument m_doc;
   QString m_cmd;
   QString m_encoder;
   void addWidgets(_k9CheckListItem *_item,QString _root,QString _cat);
   void loadXml();
   void fillListView();
   void addInt(QDomElement _eOpt);
   void addFloat(QDomElement _eOpt);
   void addBool(QDomElement _eOpt);
   void addString(QDomElement _eOpt);
   void addList(QDomElement _eOpt);

   const QString & getCmd(const QString &_root);
   void selectCodec(const QString &_root,const QString &_codec);
   void parseCodecOptions(const QString &_root,const QString &_codec,const QString & _options);
   void parseCmd(const QString &_cmd);

protected:
  /*$PROTECTED_FUNCTIONS$*/

protected slots:
  /*$PROTECTED_SLOTS$*/
    void listViewCurrentChanged(QTreeWidgetItem *,QTreeWidgetItem*);
    void listViewItemChanged(QTreeWidgetItem*,int);
};

#endif

