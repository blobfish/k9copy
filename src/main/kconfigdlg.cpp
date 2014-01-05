/**************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   k9copy@free.fr                                                            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "kconfigdlg.h"
#ifndef BACKLITE
#include "kmessagebox.h"
#include <kiconloader.h>
#include <kstdguiitem.h>
#include <kguiitem.h>
#endif
#include <kpushbutton.h>
#include <kdialog.h>
kConfigDlg::kConfigDlg(QWidget *parent)
    : QWidget(parent)
{
  Ui_configDlg.setupUi(this);

  setWindowTitle(KDialog::makeStandardCaption(i18n("Devices"),this));
  QStringList ldev;
  QStringList llabels;
  QStringList lIO;
  Ui_configDlg.tblDevices->setColumnWidth(0,150);
  Ui_configDlg.tblDevices->setColumnWidth(1,210);
  Ui_configDlg.tblDevices->setColumnWidth(2,40);
  Ui_configDlg.tblDevices->setColumnWidth(3,45);
  //KSimpleConfig settings("K9Copy");
  k9Config config;
  
  ldev=config.getDevices();
  llabels=config.getDevicesLabels();
  lIO=config.getDevicesIO();
  int row=0;
  foreach ( QString dev,ldev)
  {
    Ui_configDlg.tblDevices->setRowCount(row+1);
    QTableWidgetItem *item = new QTableWidgetItem(0);
    Ui_configDlg.tblDevices->setItem(row,0,item);
    item = new QTableWidgetItem(0);
    Ui_configDlg.tblDevices->setItem(row,1,item);

    Ui_configDlg.tblDevices->item(row,0)->setText(dev);
    QString it2=llabels.at(row);
    Ui_configDlg.tblDevices->item(row,1)->setText(it2);

    row++;

  }
  for ( int j = 0; j < Ui_configDlg.tblDevices->rowCount(); ++j )
  {
	QTableWidgetItem *I = new QTableWidgetItem(0);
	Ui_configDlg.tblDevices->setItem(j,2,I);
	I->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	QTableWidgetItem *O = new QTableWidgetItem(0);
	Ui_configDlg.tblDevices->setItem(j,3,O);
	O->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    //QStringList::Iterator it=lIO.at(j);
    QString c=lIO[j];
    if((c =="I") || (c =="IO")) I->setCheckState(Qt::Checked);
    if((c =="O") || (c =="IO")) O->setCheckState(Qt::Checked);
//    Ui_configDlg.tblDevices->setItem( j, 2, I );
//    Ui_configDlg.tblDevices->setItem( j, 3, O);
  }


//    buttonCancel->setIcon(KStandardGuiItem::cancel().icon());
//    buttonOk->setIcon(KStandardGuiItem::ok().icon());
     connect(Ui_configDlg.bAdd,SIGNAL(clicked()),this,SLOT(bAddClick()));
     connect(Ui_configDlg.bRemove,SIGNAL(clicked()),this,SLOT(bRemoveClick()));
#ifndef BACKLITE
     Ui_configDlg.bAdd->setGuiItem(KStandardGuiItem::Add);
     Ui_configDlg.bRemove->setGuiItem(KStandardGuiItem::Remove);
#endif

}


kConfigDlg::~kConfigDlg()
{}




/*!
    \fn kConfigDlg::bAddClick()
 */
void kConfigDlg::bAddClick()
{
  Ui_configDlg.tblDevices->setRowCount(Ui_configDlg.tblDevices->rowCount()+1);
  QTableWidgetItem *I = new QTableWidgetItem(0);
  Ui_configDlg.tblDevices->setItem(Ui_configDlg.tblDevices->rowCount()-1,2,I);
  I->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  I->setCheckState(Qt::Unchecked);
  QTableWidgetItem *O = new QTableWidgetItem(0);
  Ui_configDlg.tblDevices->setItem(Ui_configDlg.tblDevices->rowCount()-1,3,O);
  O->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
  O->setCheckState(Qt::Unchecked);
}


/*!
    \fn kConfigDlg::bRemoveClick()
 */
void kConfigDlg::bRemoveClick()
{
  Ui_configDlg.tblDevices->removeRow(Ui_configDlg.tblDevices->currentRow());
}


void kConfigDlg::save()
{
  QStringList ldev;
  QStringList llabels;
  QStringList lIO;
  //KSimpleConfig settings("K9Copy");
  k9Config config;
  ldev.clear();
  llabels.clear();
  for (int i=0;i<Ui_configDlg.tblDevices->rowCount();i++)
  {
    QString c=Ui_configDlg.tblDevices->item(i,0)->text();
    if (!c.isEmpty())
    {
      if (QFile::exists(c))
      {
        ldev.append(Ui_configDlg.tblDevices->item(i,0)->text());
        c=Ui_configDlg.tblDevices->item(i,1)->text();
        if (c.isEmpty()) c=Ui_configDlg.tblDevices->item(i,0)->text();
        llabels.append(c);
        QTableWidgetItem *I = (QTableWidgetItem*)Ui_configDlg.tblDevices->item(i,2);
        QTableWidgetItem *O = (QTableWidgetItem*)Ui_configDlg.tblDevices->item(i,3);
        if (O->checkState()==Qt::Checked && I->checkState()==Qt::Checked)
        {
          lIO.append("IO");
        }
        else
        {
          if (O->checkState()==Qt::Checked) lIO.append("O");
          if (I->checkState()==Qt::Checked) lIO.append("I");
          if ((!O->checkState()==Qt::Checked ) && (!I->checkState()==Qt::Checked))
          {
            lIO.append("x");
          }
        }
      }
      else
      {
        k9Dialogs::error( c + " is not a valid device", "Devices");
      }
    }
  }
  
  config.setDevicesLabels(llabels);
  config.setDevices(ldev);
  config.setDevicesIO(lIO);
  config.save();
}
