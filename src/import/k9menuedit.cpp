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

#include "k9common.h"
#include "k9menuedit.h"
#include "k9menu.h"
#include "k9title.h"
#include "k9newdvd.h"
#include <qlayout.h>
#include "k9menubutton.h"
#include <kurlrequester.h>
#include <kcolorbutton.h>
#include <qcombobox.h>

#include <qlabel.h>
#ifndef BACKLITE
#include <KIconLoader>
#include <KFileDialog>
#endif
#include <QGraphicsView>
#include <QResizeEvent>
#include <QFontDialog>
void k9GraphicsView::drawBackground ( QPainter * painter, const QRectF & rect )  {
        QGraphicsView::drawBackground(painter,rect);
    }

void k9GraphicsView::resizeEvent ( QResizeEvent * event ) {
    QPointF p=mapFromScene(sceneRect().width(),sceneRect().height());
    qreal fx=(qreal)event->size().width()/p.x() ;
    qreal fy=(qreal)event->size().height()/p.y();
    scale(qMin(fx,fy),qMin(fx,fy));
}


k9MenuEdit::k9MenuEdit(QWidget* parent, const char* ,k9MenuScene *scene)
        : QWidget(parent) {
    Ui_menuEdit.setupUi(this);
    QGridLayout *gridLayout = new QGridLayout(Ui_menuEdit.widget);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    m_graphicsView = new k9GraphicsView(Ui_menuEdit.widget);
    gridLayout->addWidget(m_graphicsView, 0, 0, 1, 1);
#ifdef BACKLITE
    Ui_menuEdit.bAddText->setIcon (SmallIcon("list-add"));
#else
     Ui_menuEdit.bAddText->setGuiItem(KStandardGuiItem::Add);
     Ui_menuEdit.bAddText->setText("");
#endif
    m_noUpdate=false;
    m_scene=scene;
    m_format=PAL;
    m_imageHeight=576;
    m_scene=new k9MenuScene(this);
    m_scene->setSceneRect( 0,0,720,m_imageHeight);
    m_graphicsView->setScene(m_scene);
    m_graphicsView->setMaximumWidth(722);
    m_text=NULL;
    Ui_menuEdit.cbPosTitle->setEnabled(false);

#ifdef BACKLITE
        Ui_menuEdit.urBackground->setFilter("jpeg (*.jpeg);;png (*.png);;All(*.*)");
        Ui_menuEdit.urBackground->setMode(KUrlRequester::OPEN) ;
#else
        Ui_menuEdit.urBackground->setFilter("*.jpg|jpeg (*.jpg)\n*.png|png (*.png)\n*.*|All (*.*)");
        Ui_menuEdit.urBackground->setMode(KFile::File) ;
        Ui_menuEdit.urBackground->fileDialog()->setOperationMode(KFileDialog::Opening);
#endif

}
/*void k9MenuEdit::updateTextPos(const QPoint &_point) {
    emit updatePos(_point);
}
*/
k9MenuEdit::~k9MenuEdit() {

}

/*$SPECIALIZATION$*/

k9CanvasSprite *convertToButton(QGraphicsItem *item) {
    if (item->type()==k9CanvasSprite::itemType())
        return (k9CanvasSprite*) item;
    else
        return NULL;
}


void k9MenuEdit::selectionChanged() {

    if (m_scene->selectedItems().count()>0) {
        foreach(QGraphicsItem *item,m_scene->selectedItems())  {
            m_text=qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
            if (m_text)
                Ui_menuEdit.cbColor->setColor(m_text->brush().color());
            k9CanvasSprite *button=convertToButton(item);
            if (button!=0) {
                m_text=button->getTextItem();
                Ui_menuEdit.cbPosTitle->setEnabled(true);
                Ui_menuEdit.cbPosTitle->setCurrentIndex(button->getTextPosition()-1);
                Ui_menuEdit.cbColor->setColor(button->getTextColor());
            }
        }
   }else
        Ui_menuEdit.cbPosTitle->setEnabled(false);

}

void k9MenuEdit::bFontClick() {
    if (m_scene->selectedItems().count()>0) {
        bool dlg=false;
        QFont myFont;
        foreach( QGraphicsItem *item, m_scene->selectedItems()) {
                k9CanvasSprite *spr=convertToButton(item);
                if (spr) {
                    if (!dlg) {
                        myFont=spr->getButton()->getFont();
                        if (!k9Dialogs::getFont( myFont ))
                            return;
                        dlg=true;
                    }
                    spr->getButton()->setFont(myFont);
                }
                m_text=qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
                if (m_text) {
                   if (!dlg) {
                        myFont=m_text->font();
                        if(!k9Dialogs::getFont( myFont ))
                           return;
                        dlg=true;
                    }
                    m_text->setFont(myFont);
                }
        }
    }
}

void k9MenuEdit::urBackgroundSelected(const KUrl &_url) {
      QImage img;
      img.load(_url.toLocalFile());
      if ( m_scene->selectedItems().count()>0 ) {
//If a button is selected, set the button image ...
        foreach( QGraphicsItem *item, m_scene->selectedItems()) {
                k9CanvasSprite *spr=convertToButton(item);
                if (spr) {
                    spr->getButton()->setImage(img);
                }
        }
      } else {
//...otherwise, set the menu background image
        img=img.scaled(720,m_imageHeight,Qt::KeepAspectRatioByExpanding);
        setBackgroundImage(img);
    }
}

void k9MenuEdit::cbColorChanged(const QColor &_color) {
    if (m_noUpdate)
        return;

    foreach( QGraphicsItem *item, m_scene->selectedItems()) {
        k9CanvasSprite *spr=convertToButton(item);
        if (spr) {
            spr->getButton()->setColor(_color);

        }
        m_text=qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (m_text) {
            m_text->setBrush(QBrush(_color));
        }
    }
}

void k9MenuEdit::cbPosTitleActivated(int _value) {
    if (m_noUpdate)
        return;
    foreach( QGraphicsItem *item, m_scene->selectedItems()) {
        k9CanvasSprite *spr=convertToButton(item);
        if (spr) {
            spr->getButton()->setTextPosition((eTextPosition)(_value+1));
        }
    }
}

void k9MenuEdit::setBackgroundImage(const QImage &_image) {
    m_background=_image;
    m_scene->setBackgroundPixmap(QPixmap::fromImage(_image));
    emit backgroundImageChanged(_image);
}

void k9MenuEdit::leTitleChanged(const QString &_value) {
    if (m_noUpdate)
        return;
    foreach( QGraphicsItem *item, m_scene->selectedItems()) {
        k9CanvasSprite *spr=convertToButton(item);
        if (spr) {
            QString c=_value;
            spr->getButton()->setText(c.replace("%n",QString::number(spr->getButton()->getNum())));
        }
        m_text=qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
        if (m_text) {
            m_text->setText(_value);
        }
    }
    //m_menuEditor->getSelected()->setText(_value);

}
/*

void k9MenuEdit::setText(const QString &_value) {
    if (m_text)
        m_text->setText(_value);
    emit textChanged(_value);
}

void k9MenuEdit::setFont(const QFont &_value) {
    if (m_text)
        m_text->setFont(_value);

    emit textFontChanged(_value);
}

void k9MenuEdit::setColor(const QColor &_value) {
    if (m_text)
        m_text->setPen(QPen(_value));

    emit textColorChanged(_value);
}

QGraphicsSimpleTextItem* k9MenuEdit::getText() const {
    return m_text;
}

*/
k9MenuEdit::eFormat k9MenuEdit::getFormat() const {
    return m_format;
}


void k9MenuEdit::setFormat(const eFormat& _value) {
    m_format = _value;
    if (_value==NTSC)
        m_imageHeight=480;
    else
        m_imageHeight=576;
    m_scene->setSceneRect(0,0,720,m_imageHeight);
/*    QPixmap pix=m_canvas->backgroundPixmap();
    QImage img=pix.toImage().scaled(720,m_imageHeight);
    m_canvas->resize(720,m_imageHeight);
    m_menuEditor->setMaximumSize(QSize(724,m_imageHeight+4));
    m_canvas->setBackgroundPixmap(QPixmap::fromImage(img));
    */
}


k9MenuScene *k9MenuEdit::getScene()  {
    return m_scene;
}


void k9MenuEdit::setScene(k9MenuScene* _value) {
    if (m_scene)
        disconnect(this,SLOT(selectionChanged()));
    m_scene = _value;
    connect(m_scene,SIGNAL(selectionChanged()),this,SLOT(selectionChanged()));
    m_text=NULL;
    m_graphicsView->setScene(m_scene);
    m_scene->setSceneRect(0,0,720,m_imageHeight);
    selectionChanged();
}

void k9MenuEdit::titleSelected(k9Title *_title) {
    disconnect (this,SIGNAL(startScriptChanged(const QString&)),0,0);
    disconnect (this,SIGNAL(endScriptChanged(const QString&)),0,0);
    Ui_menuEdit.cbEnd->setEnabled(true);
    m_scene->clearSelection();;
    Ui_menuEdit.lTitle->setText(i18n("Title %1 Menu").arg(_title->getNum()+1));
    setScene(_title->getMenu()->getScene());
    m_menuType=TITLEMENU;
    Ui_menuEdit.cbStart->clear();
    Ui_menuEdit.cbEnd->clear();
    m_startScripts.clear();
    m_endScripts.clear();


    Ui_menuEdit.cbStart->addItem(i18n("Play Menu"));
    m_startScripts << "";
    Ui_menuEdit.cbStart->addItem(i18n("Play Title"));
    m_startScripts << "if (g1==0) {g1=1; jump title 1 chapter 1;}";
    if (_title->getMenu()->getStartScript() !="")
        Ui_menuEdit.cbStart->setCurrentIndex(Ui_menuEdit.cbStart->count()-1);
    connect (this,SIGNAL(startScriptChanged(const QString&)),_title->getMenu(),SLOT(setStartScript(const QString&)));

    Ui_menuEdit.cbEnd->addItem(i18n("Play Root Menu"));
    m_endScripts << "g6=0; call vmgm menu;";
    Ui_menuEdit.cbEnd->addItem(i18n("Play Title Menu"));
    m_endScripts << "call menu;";
    k9NewDVD *newDVD=(k9NewDVD*)_title->parent() ;
    k9NewDVDItems *items=newDVD->getTitles();
    foreach (k9Title *title,*items) {
        Ui_menuEdit.cbEnd->addItem(i18n("Play Title %1").arg(title->getNum()+1));
        QString script=QString("g6=%1; call vmgm menu;" ).arg(title->getNum()+1);
        m_endScripts << script;
    }
    for (int i=0;i<m_endScripts.count();i++ ) {
        if (_title->getMenu()->getEndScript()== m_endScripts.at(i))
            Ui_menuEdit.cbEnd->setCurrentIndex(i);
    }
    connect (this,SIGNAL(endScriptChanged(const QString&)),_title->getMenu(),SLOT(setEndScript(const QString&)));

}

void k9MenuEdit::rootSelected(k9NewDVD *_newDVD) {
    disconnect (this,SIGNAL(startScriptChanged(const QString&)),0,0);
    disconnect (this,SIGNAL(endScriptChanged(const QString&)),0,0);
    Ui_menuEdit.cbEnd->setEnabled(false);
    m_scene->clearSelection();;
    Ui_menuEdit.lTitle->setText(i18n("Root Menu"));
    setScene(_newDVD->getRootMenu()->getScene());
    m_menuType=ROOTMENU;
    Ui_menuEdit.cbStart->clear();
    m_startScripts.clear();
    Ui_menuEdit.cbStart->addItem(i18n("Play Menu"));
    m_startScripts << "";
    k9NewDVDItems *items=_newDVD->getTitles();
    foreach (k9Title *title,*items) {
        Ui_menuEdit.cbStart->addItem(i18n("Play Title %1").arg(title->getNum()+1));
        // QString script=QString("if (g5==0) {g5=1; jump title %1;}" ).arg(title->getNum()+1);
        QString script=QString("g6=%1;" ).arg(title->getNum()+1);
        m_startScripts << script;
        if (script==_newDVD->getRootMenu()->getStartScript())
            Ui_menuEdit.cbStart->setCurrentIndex(Ui_menuEdit.cbStart->count()-1);
    }
    connect (this,SIGNAL(startScriptChanged(const QString&)),_newDVD->getRootMenu(),SLOT(setStartScript(const QString&)));
}

void k9MenuEdit::bAddTextClick() {
    k9Config config;
    m_text=new QGraphicsSimpleTextItem();
    m_text->setBrush(QColor(config.getPrefButtonTextColor()));
    m_text->setFont(config.getPrefButtonFont());
    m_text->setText(Ui_menuEdit.leTitle->text());
    m_text->setFlag(QGraphicsItem::ItemIsMovable);
    m_text->setFlag(QGraphicsItem::ItemIsSelectable);
    m_scene->addItem(m_text);
    m_text->setSelected(true);
}

void k9MenuEdit::cbStartActivated (int _value) {
    emit startScriptChanged(m_startScripts[_value]);
}

void k9MenuEdit::cbEndActivated (int _value) {
    emit endScriptChanged(m_endScripts[_value]);
}
