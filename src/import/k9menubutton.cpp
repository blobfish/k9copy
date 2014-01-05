//
// C++ Implementation: k9menubutton
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9menubutton.h"
#include <qpainter.h>
#include <qwmatrix.h>
#include <qbitmap.h>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

k9CanvasSprite::k9CanvasSprite (const QPixmap &pixmap, k9MenuScene *scene,k9MenuButton *button) : QGraphicsItemGroup() {
    m_text=0;
    m_pixmap=0;
    scene->addItem(this);
    m_button=button;
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    m_text=new QGraphicsSimpleTextItem(this,scene);
    m_txtColor=Qt::yellow;
    m_pixmap=new QGraphicsPixmapItem(pixmap);
    m_bPixmap=pixmap;
    this->addToGroup(m_pixmap);
    this->addToGroup(m_text);
   // m_text->setPos(this->pos().x(),m_pixmap->boundingRect().bottom()+10);
    m_textPosition=BOTTOM;
    m_txtValue="";
    m_selsize=10;
}

void k9CanvasSprite::setTextPosition(const eTextPosition &_position) {
    QRectF updateRect=sceneBoundingRect();
    m_textPosition=_position;
    m_text->setBrush(QBrush(m_txtColor,Qt::SolidPattern));
    m_text->setText(m_txtValue);
    QFontMetrics m(m_text->font());
    int h=m.boundingRect(m_txtValue).height();
    int w=m.boundingRect(m_txtValue).width();
    switch(_position) {
        case RIGHT:
            m_text->setPos(m_pixmap->boundingRect().width(),(m_pixmap->boundingRect().height()-h)/2);
            break;
        case BOTTOM:
            m_text->setPos((m_pixmap->boundingRect().width()-w)/2 ,m_pixmap->boundingRect().height());
            break;
    }
    update(boundingRect());
    scene()->update(updateRect);
}


QRectF k9CanvasSprite::boundingRect() const {

    QRectF r;
    if (m_pixmap){
        r=m_pixmap->boundingRect();
        r.moveTopLeft(m_pixmap->pos());
    }
    if (m_text) {
        QRectF r2=m_text->boundingRect();
        r2.moveTopLeft(m_text->pos());
        r=r.unite(r2);
    }
    r.adjust(-m_selsize,-m_selsize,m_selsize,m_selsize);

    return r;

}

void k9CanvasSprite::drawSizeRect (QPainter *painter) {
    QBrush br;
    br.setColor(Qt::green);
    br.setStyle(Qt::SolidPattern);
    QRectF r=boundingRect();
    painter->fillRect(QRectF(r.left(),r.top(),m_selsize,m_selsize),br);
    painter->fillRect(QRectF(r.left()+r.width()/2 - m_selsize/2,r.top(),m_selsize,m_selsize),br);
    painter->fillRect(QRectF(r.right() - m_selsize,r.top(),m_selsize,m_selsize),br);

    painter->fillRect(QRectF(r.left(),r.top()+r.height()/2 -m_selsize/2,m_selsize,m_selsize),br);
    painter->fillRect(QRectF(r.right()-m_selsize,r.top()+r.height()/2 -m_selsize/2,m_selsize,m_selsize),br);

    painter->fillRect(QRectF(r.left(),r.bottom()-m_selsize,m_selsize,m_selsize),br);
    painter->fillRect(QRectF(r.left()+r.width()/2 - m_selsize/2,r.bottom()-m_selsize,m_selsize,m_selsize),br);
    painter->fillRect(QRectF(r.right() - m_selsize,r.bottom()-m_selsize,m_selsize,m_selsize),br);


}


void k9CanvasSprite::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget ) {
    if (isSelected()) {
        QPen pen;
        pen.setStyle(Qt::DashLine);
        pen.setColor(Qt::red);
        pen.setWidth(3);
        painter->setPen(pen);
        painter->drawRect(boundingRect().adjusted(2,2,-2,-2));

        drawSizeRect(painter);

    }
    else
        QGraphicsItemGroup::paint(painter,option,widget);
}

void k9CanvasSprite::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    m_startPoint=event->scenePos();
    QRectF r(boundingRect());
    QRectF tl(r.left(),r.top(),m_selsize,m_selsize);
    QRectF tc(r.left()+r.width()/2 -m_selsize/2,r.top(),m_selsize,m_selsize);
    QRectF tr(r.right()-m_selsize,r.top(),m_selsize,m_selsize);
    QRectF ml(r.left(),r.top()+r.height()/2 -m_selsize/2,m_selsize,m_selsize);
    QRectF mr(r.right()-m_selsize,r.top()+r.height()/2 -m_selsize/2,m_selsize,m_selsize);
    QRectF bl(r.left(),r.bottom()-m_selsize,m_selsize,m_selsize);
    QRectF bc(r.left()+r.width()/2 -m_selsize/2,r.bottom()-m_selsize,m_selsize ,m_selsize);
    QRectF br(r.right()-m_selsize,r.bottom()-m_selsize,m_selsize,m_selsize);

    QPointF offset=event->scenePos() - event->lastScenePos();

    if ( tl.contains(event->pos()))
        m_resize=STL;
    else if (tc.contains(event->pos()))
        m_resize=STC;
    else if (tr.contains(event->pos()))
        m_resize=STR;
    else if (ml.contains(event->pos()))
        m_resize=SML;
    else if (mr.contains(event->pos()))
        m_resize=SMR;
    else if (bl.contains(event->pos()))
        m_resize=SBL;
    else if (bc.contains(event->pos()))
        m_resize=SBC;
    else if (br.contains(event->pos()))
        m_resize=SBR;
    else {
        m_resize=SNONE;
        QGraphicsItemGroup::mousePressEvent(event);
    }
}

void k9CanvasSprite::mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) {
    QRectF oldRect=sceneBoundingRect();
    //oldRect.adjust(pos().x(),pos().y(),pos().x(),pos().y());

    QPointF offset;
    QSizeF s(m_pixmap->pixmap().size());
    switch(m_resize) {
        case STL:
            offset= event->scenePos() - event->lastScenePos();
            setPos(pos()+offset);
            s-=QSize(offset.x(),offset.y());
            if (s.width()>0 && s.height()>0) {
                m_pixmap->setPixmap(m_bPixmap.scaled(s.toSize()));
            }
            break;
        case SBL:
            offset= event->scenePos() - event->lastScenePos();
            setPos(pos().x()+offset.x(),pos().y());
            s-=QSize(offset.x(),-offset.y());
            if (s.width()>0 && s.height()>0) {
                m_pixmap->setPixmap(m_bPixmap.scaled(s.toSize()));
            }
            break;
        case STR:
            offset= event->scenePos() - event->lastScenePos();
            setPos(pos().x(),pos().y()+offset.y());
            s+=QSize(offset.x(),-offset.y());
            if (s.width()>0 && s.height()>0) {
                m_pixmap->setPixmap(m_bPixmap.scaled(s.toSize()));
            }
            break;
        case SBR:
            offset= event->scenePos() - event->lastScenePos();
            s+=QSize(offset.x(),offset.y());
            if (s.width()>0 && s.height()>0) {
                m_pixmap->setPixmap(m_bPixmap.scaled(s.toSize()));
            }
            break;
        case STC:
            offset=QPointF(0,event->scenePos().y()-event->lastScenePos().y());
            setPos(pos().x(),pos().y()+offset.y());
            s-=QSize(offset.x(),offset.y());
            if (s.width()>0 && s.height()>0) {
                m_pixmap->setPixmap(m_bPixmap.scaled(s.toSize()));
            }
            break;
        case SBC:
            offset=QPointF(0,event->scenePos().y()-event->lastScenePos().y());
            s+=QSize(offset.x(),offset.y());
            if (s.width()>0 && s.height()>0) {
                m_pixmap->setPixmap(m_bPixmap.scaled(s.toSize()));
            }
            break;
        case SML:
            offset=QPointF(event->scenePos().x()-event->lastScenePos().x(),0);
            setPos(pos().x()+offset.x(),pos().y());
            s-=QSize(offset.x(),offset.y());
            if (s.width()>0 && s.height()>0) {
                m_pixmap->setPixmap(m_bPixmap.scaled(s.toSize()));
            }
            break;
        case SMR:
            offset=QPointF(event->scenePos().x()-event->lastScenePos().x(),0);
            s+=QSize(offset.x(),offset.y());
            m_pixmap->setPixmap(m_bPixmap.scaled(s.toSize()));
            break;
        default:
            break;
    }
    if (m_resize != SNONE) {
        setTextPosition(m_textPosition);
        update(boundingRect());
        scene()->update(oldRect);
    }else
        QGraphicsItemGroup::mouseMoveEvent(event);
}

void k9CanvasSprite::setPixmap(const QPixmap &pixmap) {
    m_bPixmap=pixmap;
    m_pixmap->setPixmap(pixmap);
    setTextPosition(m_textPosition);
}

void k9CanvasSprite::scalePixmap(int width,int height) {
   m_pixmap->setPixmap(m_bPixmap.scaled(width,height));
   setTextPosition(m_textPosition);
}

void k9CanvasSprite::setFont(const QFont &font) {
    m_text->setFont(font);
}

QFont k9CanvasSprite::getFont() {
    return m_text->font();
}
void k9CanvasSprite::setTextColor(const QColor &color) {
    m_txtColor=color;
    setTextPosition(m_textPosition);
}

QColor k9CanvasSprite::getTextColor() {
    return m_txtColor;
}

void k9CanvasSprite::setText(const QString &text) {
    m_txtValue=text;
    setTextPosition(m_textPosition);
}

QString k9CanvasSprite::getText() {
    return m_txtValue;
}

k9MenuButton* k9CanvasSprite::getButton() const {
    return m_button;
}


void k9MenuButton::update() {
    int count=m_scene->items().count();

    m_sprite->setTextPosition(m_textPosition);

    m_sprite->setZValue(count--);
    emit sigsetLeft(getLeft());
    emit sigsetTop(getTop());
    emit sigsetWidth(getWidth());
    emit sigsetHeight(getHeight());
}

k9MenuButton::k9MenuButton(k9MenuScene *parent, const char *)
 : QObject(parent),m_width(0),m_height(0)
{
    m_scene=parent;
    m_sprite=new k9CanvasSprite(QPixmap(),parent,this);
    m_pixmap=NULL;
}


void k9MenuButton::remove() {
    if (m_sprite )
	delete m_sprite;

    m_sprite=NULL;
}

k9MenuButton::~k9MenuButton()
{

}

void k9MenuButton::setImage(const QImage &_image) {
    m_image=_image;
    emit sigsetImage(this,_image);
    loadImage();

}

void k9MenuButton::setImage(const QString &_fileName){
    m_image.load(_fileName);
    emit sigsetImage(this,m_image);
    loadImage();
}

void k9MenuButton::loadImage() {
    QPixmap pix=QPixmap::fromImage(m_image);
    m_sprite->setPixmap(pix);
    m_sprite->scalePixmap(m_width,m_height);
    update();
}

void k9MenuButton::setWidth(int _value,bool _reload) {
    m_width=_value;
    emit sigsetWidth(_value);
    if (_reload)
        loadImage();
}

int k9MenuButton::getWidth() {
    return m_sprite->boundingRect().width();
}

void k9MenuButton::setHeight(int _value,bool _reload) {
    m_height=_value;
    emit sigsetHeight(_value);
    if (_reload)
        loadImage();
}

int k9MenuButton::getHeight() {
    return m_sprite->boundingRect().height();
}

void k9MenuButton::setTop(int _value,bool _reload) {
    m_sprite->setPos(m_sprite->pos().x(), _value);
    emit sigsetTop(_value);
    if (_reload)
        loadImage();
}

void k9MenuButton::setLeft(int _value,bool _reload) {
    m_sprite->setPos(_value,m_sprite->pos().y());
    emit sigsetLeft(_value);
    if (_reload)
        loadImage();
}

int k9MenuButton::getTop() {
    return m_sprite->boundingRect().top()+m_sprite->pos().y();
}

int k9MenuButton::getLeft() {
    return m_sprite->boundingRect().left()+m_sprite->pos().x();
}

void k9MenuButton::setFont(QFont _font) {
    m_sprite->setFont(_font);
    update();
    emit sigsetFont(_font);
}

QFont k9MenuButton::getFont() {
    return m_sprite->getFont();
}

void k9MenuButton::setColor(QColor _color) {
    m_sprite->setTextColor(_color);
    emit sigsetColor(_color);
}

QColor k9MenuButton::getColor() {
    return m_sprite->getTextColor();
}

void k9MenuButton::setText(QString _text) {
    m_sprite->setText(_text);
    update();
    emit sigsetText(_text);
}

QString k9MenuButton::getText() {
    return m_sprite->getText();
}

void k9MenuButton::select(bool _state) {
    m_sprite->setSelected(_state);
}

void k9MenuButton::moveBy(int _x,int _y,bool _reload) {
    int x=getLeft();
    int y=getTop();
    m_sprite->moveBy(_x,_y);
    emit sigsetLeft(x+_x);
    emit sigsetTop(y+_y);

    if(_reload)
        loadImage();
}



eTextPosition k9MenuButton::getTextPosition() const {
    return m_textPosition;
}


void k9MenuButton::setTextPosition(const eTextPosition& _value) {
    m_textPosition = _value;
    update();
    emit sigsetTextPosition(_value);
}


QImage k9MenuButton::getImage() const {
    return m_image;
}


QString k9MenuButton::getScript() const {
    return m_script;
}


void k9MenuButton::setScript(const QString& _value) {
    m_script = _value;
}


int k9MenuButton::getAngle() const {
    return m_angle;
}


void k9MenuButton::setAngle(int _value) {
    m_angle = _value;
}


k9CanvasSprite* k9MenuButton::getSprite() const {
    return m_sprite;
}


int k9MenuButton::getNum() const {
    return m_num;
}


void k9MenuButton::setNum(int _value) {
    m_num = _value;
}
