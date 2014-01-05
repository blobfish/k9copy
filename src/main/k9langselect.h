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

#ifndef K9LANGSELECT_H
#define K9LANGSELECT_H

#include "k9common.h"
#include <QObject>
#include "ui_langselectw.h"

#include "k9main.h"

class k9Main;
class k9DVD;

class ckLvLangItem : public QTreeWidgetItem {
public:
    ckLvLangItem( QTreeWidgetItem *parent,k9Main *dlg)
            : QTreeWidgetItem( parent,1001) {
        mainDlg=dlg;
        streamType=NONE;
        setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
	setOn(false);
    }
    bool isOn() { return checkState(0)==Qt::Checked;}
    void setOn(bool value) { setCheckState(0, value ? Qt::Checked : Qt::Unchecked);}

    eStreamType streamType;
    k9Main *mainDlg;
    QString language;
    void stateChange(bool state);
};



class k9LangSelect : public QWidget
{
  Q_OBJECT

public:
  k9LangSelect(k9Main *mainWindow,QWidget* parent = 0);
  ~k9LangSelect();

  void setMain(k9Main* _value);
  void fillLvLanguages();
  void update();
  void clear();
  void selectAll(bool _state);
  QTreeWidget *listView() { return Ui_langSelect.lvLanguages;}
  /*$PUBLIC_FUNCTIONS$*/

public slots:
  /*$PUBLIC_SLOTS$*/
  void listViewItemChanged(QTreeWidgetItem*,int);
protected:
  /*$PROTECTED_FUNCTIONS$*/
  k9Main * m_main;
  k9DVD *m_dvd;
  QList <ckLvLangItem*> langAudItems;
  QList <ckLvLangItem*> langSubItems;
  void updateLvLang(const eStreamType streamType,const QString & lang );

protected slots:
  /*$PROTECTED_SLOTS$*/
private:
   Ui::langSelect Ui_langSelect;
};

#endif

