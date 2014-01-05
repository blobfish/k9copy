//
// C++ Implementation: 
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "k9mencodercmdgen.h"
#include <QScrollArea>
#include <qtabwidget.h>
#include <qfile.h>
#include <qdom.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <QSpinBox>
#include <kdialog.h>
#ifndef BACKLITE
#include <kstandarddirs.h>
#endif
#include <QHeaderView>
class _k9CheckListItem: public QTreeWidgetItem {
public:
    _k9CheckListItem(QTreeWidgetItem *_item):QTreeWidgetItem(_item,1001) {
  	setFlags(Qt::ItemIsUserCheckable |Qt::ItemIsEnabled);
 	setCheckState(0,Qt::Unchecked);

    };
    QString root;
    QDomDocument doc;
    int page;
    QFrame *frame;
    bool isOn() { return checkState(0)==Qt::Checked;}
    void setOn(bool value) { setCheckState(0, value ? Qt::Checked : Qt::Unchecked);}
    void stateChange(bool _state);
};

void _k9CheckListItem::stateChange(bool _state) {
    QDomElement eRoot=doc.documentElement().elementsByTagName(root).item(0).toElement();
    QDomElement eCodec=eRoot.elementsByTagName(text(0)).item(0).toElement();
    eCodec.setAttribute("selected",_state?"true":"false");
}


k9MencoderCmdGen::k9MencoderCmdGen(QWidget* parent,const QString &_cmd,const QString &_encoder)
: QWidget(parent),m_encoder(_encoder)
{
    Ui_mencoderCmdGen.setupUi(this);

    setWindowTitle(KDialog::makeStandardCaption(i18n("MEncoder options")));
    listView=Ui_mencoderCmdGen.listView;
    wsOptions=Ui_mencoderCmdGen.wsOptions;

    m_row=0; m_page=1;
    loadXml();
    parseCmd(_cmd);

    fillListView();
    connect( listView,SIGNAL(currentItemChanged ( QTreeWidgetItem *, QTreeWidgetItem * )),this,SLOT(listViewCurrentChanged(QTreeWidgetItem *,QTreeWidgetItem*)));

}

void k9MencoderCmdGen::fillListView() {
    listView->clear();
    listView->header()->hide();
    QTreeWidgetItem *item=new QTreeWidgetItem(listView);
    item->setExpanded(true);
    item->setText(0,i18n("Audio Codec"));
    QDomElement root=m_doc.documentElement().elementsByTagName("AUDIO").item(0).toElement();
    for (int i=0; i< root.childNodes().count();i++) {
        QDomElement eChild=root.childNodes().item(i).toElement();
        _k9CheckListItem *child=new _k9CheckListItem(item);
        child->setText(0,eChild.nodeName());
        child->root="AUDIO";
        child->doc=m_doc;
        QDomAttr attr=eChild.attributeNode("selected");
        if (!attr.isNull())
            child->setOn(attr.value()=="true");

        addWidgets(child, "AUDIO",eChild.nodeName());

    }
    item=new QTreeWidgetItem(listView);
    item->setText(0,i18n("Video Codec"));
    item->setExpanded(true);
    root=m_doc.documentElement().elementsByTagName("VIDEO").item(0).toElement();
    for (int i=0; i< root.childNodes().count();i++) {
        QDomElement eChild=root.childNodes().item(i).toElement();
        _k9CheckListItem *child=new _k9CheckListItem(item);
        child->setText(0,eChild.nodeName());
        child->root="VIDEO";
        child->doc=m_doc;
        QDomAttr attr=eChild.attributeNode("selected");
        if (!attr.isNull())
            child->setOn(attr.value()=="true");

        addWidgets(child, "VIDEO",eChild.nodeName());


    }

    item=new QTreeWidgetItem(listView);
    item->setText(0,i18n("Filters"));
    item->setExpanded(true);
    root=m_doc.documentElement().elementsByTagName("FILTERS").item(0).toElement();
    for (int i=0; i< root.childNodes().count();i++) {
        QDomElement eChild=root.childNodes().item(i).toElement();
        _k9CheckListItem *child=new _k9CheckListItem(item);
        child->setText(0,eChild.nodeName());
        child->root="FILTERS";
        child->doc=m_doc;
        QDomAttr attr=eChild.attributeNode("selected");
        if (!attr.isNull())
            child->setOn(attr.value()=="true");

        addWidgets(child, "FILTERS",eChild.nodeName());
    }
   connect(listView,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(listViewItemChanged(QTreeWidgetItem*,int)));

}

void k9MencoderCmdGen::listViewItemChanged(QTreeWidgetItem *_item,int _col) {
    if (_item->type()==1001 && _col==0) {
	_k9CheckListItem *it=(_k9CheckListItem*)_item;
	it->stateChange(it->isOn());
    }
}

k9MencoderCmdGen::~k9MencoderCmdGen()
{
}

/*$SPECIALIZATION$*/

void k9MencoderCmdGen::listViewCurrentChanged(QTreeWidgetItem *_item,QTreeWidgetItem *) {
    if (_item->parent() == NULL)
        wsOptions->setCurrentIndex(0);
    else if (_item->type()==1001) {
        _k9CheckListItem *item=(_k9CheckListItem*) _item;
        wsOptions->setCurrentIndex(item->page);
    

    }    
}

void k9MencoderCmdGen::loadXml() {
#ifdef BACKLITE
    QFile file(":/data/mencoder-options");
#else
    QFile file(KGlobal::dirs()->findResource( "data", QString("k9copy/%1.xml").arg(m_encoder)));
#endif
    if ( !file.open( QIODevice::ReadOnly ) )
        return;
    if ( !m_doc.setContent( &file ) ) {
        file.close();
        return;
    }
    file.close();


 }

void k9MencoderCmdGen::addWidgets(_k9CheckListItem *_listItem,QString _root,QString _cat) {
    m_cpt=0;
    //QVBox *vbox=new QVBox(wsOptions);

    QWidget *page=new QWidget();    
    QVBoxLayout *vbox = new QVBoxLayout(page);
    QFrame *fr=new QFrame(page);
    vbox->addWidget(fr);    
    
    _listItem->page=wsOptions->addWidget(page);
    m_page++;


    _listItem->frame=fr;

    
    QGridLayout *m_grid1=new QGridLayout(fr);
    m_scrollView=new QScrollArea(fr);
    //m_scrollView->viewport()->setPaletteBackgroundColor(this->paletteBackgroundColor());
    m_scrollView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_grid1->addWidget(m_scrollView,0,0);
  

    //TODO: suppression de l'object m_grid
    m_grid=new k9Grid(2,this);
    m_scrollView->setWidget(m_grid->widget());

   //finds the VIDEO node
    QDomElement eVideo=m_doc.documentElement().elementsByTagName(_root).item(0).toElement();
    //gets the codec list
    QDomNodeList lCodecs=eVideo.elementsByTagName(_cat);
    for (int i=0; i<lCodecs.count();i++) {
        QDomElement eCodec=lCodecs.item(i).toElement();
        //gets codec options
        QDomNodeList lOptions=eCodec.elementsByTagName("opt");
        for (int j=0;j<lOptions.count();j++) {
            QDomElement eOpt=lOptions.item(j).toElement();
            QDomAttr aType=eOpt.attributeNode("type");
            if (aType.value() == "int")
                addInt(eOpt);
            else if(aType.value()=="float")
                addFloat(eOpt);
            else if (aType.value()=="bool")
                addBool(eOpt);
            else if (aType.value()=="string")
                addString(eOpt);
            else if (aType.value()=="")
                addList(eOpt);
        }
    }
}

void k9MencoderCmdGen::addList(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   bool bSel=false;
   QDomAttr aSel=_eOpt.attributeNode("selected");
   if (!aSel.isNull())
       bSel=aSel.value()=="true";


   QCheckBox *ckLabel=new QCheckBox(sName,m_grid->widget() );
   m_grid->addWidget(ckLabel);
   ckLabel->setChecked(bSel);
   ckLabel->setObjectName(QString("ck%1").arg(m_row++));
   m_hbox=new k9Grid(2,m_grid->widget());
//   m_hbox->setFrameShape(QFrame::StyledPanel);
//   m_hbox->setFrameShadow(QFrame::Raised);

   if ( !_eOpt.attributeNode("description").isNull())  {
        ckLabel->setToolTip(_eOpt.attributeNode("description").value());
        ckLabel->setWhatsThis(_eOpt.attributeNode("description").value());
   }


    QDomNodeList lOption=_eOpt.elementsByTagName("subopt");
    for (int i=0; i<lOption.count();i++) {
        QDomElement eSubOpt=lOption.item(i).toElement();
        QDomAttr aType=eSubOpt.attributeNode("type");
        if (aType.value() == "int")
            addInt(eSubOpt);
        else if(aType.value()=="float")
            addFloat(eSubOpt);
        else if (aType.value()=="bool")
            addBool(eSubOpt);
        else if (aType.value()=="string")
            addString(eSubOpt);
    }

  _eOpt.setAttribute("checkbox",ckLabel->objectName());
   m_grid->addWidget(m_hbox->widget());

}


void k9MencoderCmdGen::addInt(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   bool blist=false;
   if (_eOpt.nodeName()=="subopt")
        blist=true; 

   int iMin=_eOpt.attributeNode("min").value().toInt();
   int iMax=_eOpt.attributeNode("max").value().toInt();
   int iDefault=_eOpt.attributeNode("default").value().toInt();

   k9Grid *parent;
   if (!blist) {
       parent=m_grid;
   } else
       parent=m_hbox;

    bool bSel=false;
    QDomAttr aSel=_eOpt.attributeNode("selected");
    if (!aSel.isNull())
        bSel=aSel.value()=="true";

    QCheckBox *ckLabel=new QCheckBox(sName,parent->widget());
    ckLabel->setObjectName(QString("ck%1").arg(m_row++) );
    parent->addWidget(ckLabel);
    ckLabel->setChecked(bSel);
//    ckLabel->setBackgroundColor(parent->backgroundColor());

    if ( !_eOpt.attributeNode("description").isNull()) {
        ckLabel->setToolTip(_eOpt.attributeNode("description").value());
        ckLabel->setWhatsThis(_eOpt.attributeNode("description").value());
    }
    _eOpt.setAttribute("checkbox",ckLabel->objectName());


   QSpinBox *sb= new QSpinBox(parent->widget());
   sb->setMinimum(iMin);
   sb->setMaximum(iMax);
   sb->setValue(iDefault);
   sb->setSingleStep(1);
   sb->setObjectName(QString("int%1").arg(m_row++));
   parent->addWidget(sb);
   
   if (iMax <1000)
        sb->setFixedWidth(50);
    else   
        sb->setFixedWidth(100);
    if( !_eOpt.attributeNode("special").isNull()){
        sb->setSpecialValueText(_eOpt.attributeNode("special").value());
        if (_eOpt.attributeNode("special").value()== _eOpt.attributeNode("default").value())
            sb->setValue(iMin-1);

    }

   _eOpt.setAttribute("widget",sb->objectName());

   //m_grid->addWidget(label,m_row++,0);
}
void k9MencoderCmdGen::addFloat(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   double dMin=_eOpt.attributeNode("min").value().toDouble();
   double dMax=_eOpt.attributeNode("max").value().toDouble();
   double dDefault=_eOpt.attributeNode("default").value().toDouble();
   bool bSel=false;
   bool blist=false;
    if (_eOpt.nodeName()=="subopt")
        blist=true; 
  
   k9Grid *parent;
   if (!blist) {
      parent=m_grid;
   } else
       parent=m_hbox;
    QDomAttr aSel=_eOpt.attributeNode("selected");
    if (!aSel.isNull())
        bSel=aSel.value()=="true";
    
    QCheckBox *ckLabel=new QCheckBox(sName,parent->widget());
    ckLabel->setObjectName(QString("ck%1").arg(m_row++));
    ckLabel->setChecked(bSel);
    parent->addWidget(ckLabel);
    if ( !_eOpt.attributeNode("description").isNull()) {
            ckLabel->setToolTip(_eOpt.attributeNode("description").value());
            ckLabel->setWhatsThis(_eOpt.attributeNode("description").value());
    }
    _eOpt.setAttribute("checkbox",ckLabel->objectName());

   double dStep=0.01;
   int iPrecision=2;
   if (!_eOpt.attributeNode("step").isNull()) {
        dStep=_eOpt.attributeNode("step").value().toDouble();
    }
   if (!_eOpt.attributeNode("precision").isNull()) {
        iPrecision=_eOpt.attributeNode("precision").value().toInt();
    }
   QDoubleSpinBox *sb= new QDoubleSpinBox(parent->widget());
   sb->setRange(dMin,dMax);
   sb->setValue(dDefault);
   sb->setSingleStep(dStep);
   sb->setDecimals(iPrecision);

 //QDoubleSpinBox(dMin,dMax,dDefault,parent->widget(),dStep,iPrecision);
   sb->setObjectName(QString("float%1").arg(m_row++));
   parent->addWidget(sb);

   if (dMax <1000)
        sb->setFixedWidth(80);
    else   
        sb->setFixedWidth(120);
    if( !_eOpt.attributeNode("special").isNull()){
        sb->setSpecialValueText(_eOpt.attributeNode("special").value());
    }
   
   _eOpt.setAttribute("widget",sb->objectName());
}

void k9MencoderCmdGen::addBool(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   bool blist=false;
   if (_eOpt.nodeName()=="subopt")
        blist=true; 

   bool bSel=false;
   k9Grid *parent;
   if (!blist) {
        parent = m_grid;
   } else
        parent = m_hbox;

    QDomAttr aSel=_eOpt.attributeNode("selected");
    if (!aSel.isNull())
        bSel=aSel.value()=="true";

   QCheckBox *ckLabel=new QCheckBox(sName,parent->widget());
   ckLabel->setObjectName(QString("ck%1").arg(m_row++));
   parent->addWidget(ckLabel);

    if ( !_eOpt.attributeNode("description").isNull()) {
        ckLabel->setToolTip(_eOpt.attributeNode("description").value());
        ckLabel->setWhatsThis(_eOpt.attributeNode("description").value());
    }
    QWidget *w=new QWidget(parent->widget());
    parent->addWidget(w);

    if ((_eOpt.attributeNode("default").value()=="true") && bSel)
        ckLabel->setChecked(true);   

   _eOpt.setAttribute("checkbox",ckLabel->objectName());

}

void k9MencoderCmdGen::addString(QDomElement _eOpt) {
   QString sName=_eOpt.attributeNode("name").value();
   bool blist=false;
   if (_eOpt.nodeName()=="subopt")
        blist=true; 
    
   QString sDefault=_eOpt.attributeNode("default").value();
   k9Grid *parent;
   if( !blist) {
        parent=m_grid;
   } else
        parent=m_hbox;

    bool bSel=false;
    QDomAttr aSel=_eOpt.attributeNode("selected");
    if (!aSel.isNull())
        bSel=aSel.value()=="true";
    
    QCheckBox *ckLabel=new QCheckBox(sName,parent->widget());
    parent->addWidget(ckLabel);
    ckLabel->setObjectName(QString("ck%1").arg(m_row++));
    ckLabel->setChecked(bSel);
    if ( !_eOpt.attributeNode("description").isNull()) {
            ckLabel->setToolTip(_eOpt.attributeNode("description").value());
            ckLabel->setWhatsThis(_eOpt.attributeNode("description").value());
    }
   _eOpt.setAttribute("checkbox",ckLabel->objectName());

   QComboBox *cb=new QComboBox(parent->widget());
   cb->setObjectName(QString("string%1").arg(m_row++));
   parent->addWidget(cb);
   
   QDomNodeList values=_eOpt.elementsByTagName("value");
   int def=0;
   if (values.count()==0) {
        cb->setEditable(true);
        cb->setEditText(sDefault);
  } else {
        for (int i=0;i<values.count();i++) {
            QDomElement e=values.item(i).toElement();
            cb->addItem(e.attributeNode("name").value());
            if (e.attributeNode("name").value() ==sDefault)
                def=cb->count()-1;
        }
        cb->setCurrentIndex(def);
    }
   _eOpt.setAttribute("widget",cb->objectName());

}

const QString & k9MencoderCmdGen::getCmd(const QString &_root) {
 /*   QString str;
    QTextStream ts(&str,IO_WriteOnly);
    m_doc.save(ts,4);
    QFile f ("/home/jmp/doc.xml");
    f.open(IO_WriteOnly);
    f.writeBlock(str.latin1(),str.length());
    f.close();
*/
    m_cmd="";
    QString m_sepOpt=":";
    QString m_sepValues=",";
    //finds the VIDEO node
    QDomElement eRoot=m_doc.documentElement().elementsByTagName(_root).item(0).toElement();

    //gets the codec list
    QDomNodeList lCodecs=eRoot.childNodes();
    for (int i=0; i<lCodecs.count();i++) {
        QString sCmd="",sCmd1="";
        QDomElement eCodec=lCodecs.item(i).toElement();
        QDomAttr eSep=eCodec.attributeNode("separator");
        if (!eSep.isNull())
            m_sepOpt=eSep.value();

        QDomAttr eAttr=eCodec.attributeNode("selected");
        if (eAttr.isNull())
            continue;
        if (eAttr.value()=="false")
            continue;

        //gets codec options
        QDomNodeList lOptions=eCodec.elementsByTagName("opt");
        for (int j=0;j<lOptions.count();j++) {
            QDomElement eOpt=lOptions.item(j).toElement();
            QDomAttr aType=eOpt.attributeNode("type");  
            QDomAttr aCheckBox=eOpt.attributeNode("checkbox");
            QDomAttr aWidget=eOpt.attributeNode("widget");
            QCheckBox *ck=this->findChild<QCheckBox*>(aCheckBox.value());
            if (ck->isChecked()) {
                if (sCmd1.isEmpty()){
                    if (eCodec.attributeNode("options").isNull())
                        sCmd1=" -"+eCodec.tagName()+" ";
                    else
                        sCmd1=eCodec.tagName()+" -"+eCodec.attributeNode("options").value()+" ";

                }
                if(aType.isNull()) {
                    //build list
                    eSep=eOpt.attributeNode("separator");
                    if (!eSep.isNull())
                        m_sepValues=eSep.value();
                    QDomNodeList lSubOpt=eOpt.elementsByTagName("subopt");
                    QString sCmd2="";
                    QString sTmp="";
                    for (int k=0;k<lSubOpt.count();k++) {
                        QDomElement eSOpt=lSubOpt.item(k).toElement();
                        QDomAttr aSType=eSOpt.attributeNode("type");  
                        QDomAttr aSWidget=eSOpt.attributeNode("widget");
                        QString sPrefix="";
                        if (!eSOpt.attributeNode("prefix").isNull())
                            sPrefix=eSOpt.attributeNode("prefix").value();
                        QCheckBox *Sck=this->findChild<QCheckBox*>(eSOpt.attributeNode("checkbox").value());
                        if(aSType.value()=="int" && Sck->isChecked()) {
                            QSpinBox *isb= this->findChild<QSpinBox*>(aSWidget.value());
                            if (!sCmd2.isEmpty())
                                sCmd2 +=m_sepValues;
                            sCmd2+=sTmp+sPrefix+isb->text().trimmed() ;      
                            sTmp="";
                        } else if(aSType.value()=="float" && Sck->isChecked()) {
                            QDoubleSpinBox *isb= this->findChild<QDoubleSpinBox*>(aSWidget.value());
                            if (!sCmd2.isEmpty())
                                sCmd2 +=m_sepValues;
                            sCmd2+=sTmp+sPrefix+isb->text().replace(",",".").trimmed();      
                            sTmp="";
                        } else if(aSType.value()=="string" && Sck->isChecked()) {
                            QComboBox *cb= this->findChild<QComboBox*>(aSWidget.value());
                            if (!sCmd2.isEmpty() )
                                sCmd2 +=m_sepValues;
                           // sCmd2+= cb->text(cb->currentItem());
                            sCmd2+= sTmp+ sPrefix+cb->currentText();
                            sTmp="";
                        }else if(aSType.value()=="bool" && Sck->isChecked()) {
                            if (!sCmd2.isEmpty() )
                                sCmd2 +=m_sepValues;
                            sCmd2+= sTmp+ eSOpt.attributeNode("name").value();
                            sTmp="";                        
                        } else if(sPrefix==""){
                                sTmp +=m_sepValues;
                        }
                    }
                    if (!sCmd.isEmpty())
                        sCmd +=m_sepOpt;;
                    sCmd+= eOpt.attributeNode("name").value() + "=" +sCmd2;
                }else if(aType.value()=="int") {
                    QSpinBox *isb= this->findChild<QSpinBox*>(aWidget.value());
                    if (!sCmd.isEmpty())
                        sCmd +=m_sepOpt;
                    sCmd+= eOpt.attributeNode("name").value() + "=" + isb->text();
                } else if(aType.value()=="float") {
                    QDoubleSpinBox *isb= this->findChild<QDoubleSpinBox*>(aWidget.value());
                    if (!sCmd.isEmpty())
                        sCmd +=m_sepOpt;
                    sCmd+= eOpt.attributeNode("name").value() + "=" + isb->text().replace(",",".");
                } else if(aType.value()=="string") {
                    QComboBox *cb=this->findChild<QComboBox*>(aWidget.value());
                    if (!sCmd.isEmpty())
                        sCmd +=m_sepOpt;
                    sCmd+= eOpt.attributeNode("name").value() + "=" + cb->currentText();
                } else if(aType.value()=="bool") {
                    if (!sCmd.isEmpty()) 
                            sCmd +=m_sepOpt;
                    sCmd+=eOpt.attributeNode("name").value() ;
                }
            }
        }
        m_cmd+=sCmd1+sCmd;

    }
    
    return m_cmd;
}

bool k9MencoderCmdGen::getMencoderOptions(QString &_cmd,const QString &_encoder) {
    KDialog *dlg= new KDialog();
    dlg->setWindowTitle(dlg->makeStandardCaption("MEncoder options"));
    k9MencoderCmdGen * m_cmdGen=new k9MencoderCmdGen(NULL,_cmd,_encoder);
    m_cmdGen->m_cmd=_cmd;
    dlg->setMainWidget(m_cmdGen);
    dlg->resize(800,600);
    bool res=dlg->exec();
    if (res) {
        QString m_video=m_cmdGen->getCmd("VIDEO").trimmed();
        QString m_audio=m_cmdGen->getCmd("AUDIO").trimmed();
        QString m_filters=m_cmdGen->getCmd("FILTERS").trimmed();
        if (!m_video.isEmpty())
            m_video="-ovc "+m_video;
        if (!m_audio.isEmpty())
            m_audio="-oac " +m_audio;
        _cmd=" "+m_video+" "+m_audio+" "+m_filters;
	_cmd=_cmd.trimmed();
    }
    delete m_cmdGen;
    return res;
}

void k9MencoderCmdGen::parseCmd(const QString &_cmd){
    QString cmd(_cmd);
    cmd=cmd.replace("\n"," ").trimmed();
    QStringList cmdList=cmd.split(" ");
    for (QStringList::iterator it=cmdList.begin();it!=cmdList.end();++it) {
        if (*it=="-ovc") {
            ++it;
            selectCodec("VIDEO",*it);            
        } else if (*it=="-oac") {
            ++it;
            selectCodec("AUDIO",*it);            
        } else { 
            if (*it=="-vf") 
                selectCodec("FILTERS","vf");
            else if (*it=="-af") 
                selectCodec("FILTERS","af");
            QDomNodeList lRoot=m_doc.documentElement().childNodes();
            bool bFound=false;
            QString sCodec=*it;
            for (int i=0;i<lRoot.count();i++) {
                QDomElement eRoot=lRoot.item(i).toElement();
                QDomNodeList lCodec=eRoot.childNodes();
                for (int j=0;j<lCodec.count();j++) {
                    QDomElement eCodec=lCodec.item(j).toElement();
                    QString sOptName="";
                    if (eCodec.attributeNode("options").isNull())
                        sOptName="-"+eCodec.nodeName();
                    else
                        sOptName="-"+eCodec.attributeNode("options").value();
                    if ( sOptName ==sCodec) {
                        if (!bFound)
                            ++it;
                        parseCodecOptions(eRoot.nodeName(),eCodec.nodeName(),*it);
                        bFound=true;        
                    }
                }

            }
        }
    }
}

void k9MencoderCmdGen::parseCodecOptions(const QString &_root,const QString &_codec,const QString & _options) {

    QString m_sepOpt=":";
    QString m_sepValues=",";

    QDomElement eRoot=m_doc.documentElement().elementsByTagName(_root).item(0).toElement();
    QDomElement eCodec=eRoot.elementsByTagName(_codec).item(0).toElement();

    if (! eCodec.attributeNode("separator").isNull())
        m_sepOpt=eCodec.attributeNode("separator").value();

    QStringList slOptions=_options.split(m_sepOpt);
    QDomNodeList lOpt=eCodec.elementsByTagName("opt");


    for (int i=0;i <lOpt.count();i++) {
        QDomElement eOpt=lOpt.item(i).toElement();
        QString sName=eOpt.attributeNode("name").value();
        if (! eOpt.attributeNode("separator").isNull())
            m_sepValues=eOpt.attributeNode("separator").value();

        //extract the option (optname=value)
        QStringList sl=slOptions.filter(sName,Qt::CaseInsensitive);
        QString sOpt="";
        foreach (QString s,slOptions) 
            if (s==sName || s.startsWith(sName+"="))
                sOpt=s;

        if (sOpt!="") {
            //QString sOpt=*(sl.at(0));
            QString value;
            if (eOpt.attributeNode("type").isNull()) {
                //it's a list of values
                QString sValues=sOpt.replace("="+m_sepValues,"= "+m_sepValues).replace(m_sepValues+m_sepValues,m_sepValues+" "+m_sepValues).replace(m_sepValues+m_sepValues,m_sepValues+" "+m_sepValues).section("=",1);
                QStringList slValues=sValues.split(m_sepValues);
                //loop on values nodes
                QDomNodeList lSOpt=eOpt.elementsByTagName("subopt");
                for (int j=0;j<lSOpt.count();j++) {
                    QDomElement eSOpt=lSOpt.item(j).toElement();
                    //if (eSOpt.attributeNode("type").value()=="int") {
                    if (eSOpt.attributeNode("prefix").isNull()) {
                        if (j< slValues.count() && (slValues.at(j)) !=" " ) {
                            QString sVal=(slValues.at(j));
                            if (eSOpt.attributeNode("type").value()=="bool")
                                sVal="true";
                            eSOpt.setAttribute("default",sVal);
                            eSOpt.setAttribute("selected","true");
                        }
                    } else {
                        QString sPrefix=eSOpt.attributeNode("prefix").value();
                        for (int k=0;k<slValues.count();k++) {
                            QString s=(slValues.at(k));
                            if (s.startsWith(sPrefix)) {
                                eSOpt.setAttribute("default",s.replace(sPrefix,""));
                                eSOpt.setAttribute("selected","true");
                            }
                        }
                    }
                }
            } else if (eOpt.attributeNode("type").value()=="bool" )
                value="true";
            else
                value=sOpt.section("=",1,1);
            eOpt.setAttribute("default",value);
            eOpt.setAttribute("selected","true");
        }
    }
}

void k9MencoderCmdGen::selectCodec(const QString &_root,const QString &_codec) {
    QDomElement root=m_doc.documentElement().elementsByTagName(_root).item(0).toElement();
    QDomElement codec=root.elementsByTagName(_codec).item(0).toElement();
    if (!codec.isNull())
        codec.setAttribute("selected","true");
}

