//
// C++ Interface: k9menubutton
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9MENUBUTTON_H
#define K9MENUBUTTON_H

#include <qobject.h>
#include <qpixmap.h>
#include <qimage.h>
#include <QGraphicsPixmapItem>
#include "k9menuscene.h"
#include <QGraphicsTextItem>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

enum eTextPosition {BOTTOM=1,RIGHT=2};

class k9MenuButton;
class k9CanvasSprite : public QGraphicsItemGroup {
public:
    k9CanvasSprite (const QPixmap &pixmap,k9MenuScene *scene,k9MenuButton *button);
    int type() const {return  (UserType+1);}
    static int itemType() { return (UserType +1);}
    k9MenuButton* getButton() const;
    QGraphicsPixmapItem *getPixmapItem() {return m_pixmap;}
    QGraphicsSimpleTextItem *getTextItem() {return m_text;}
    QRectF boundingRect() const;
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    void setPixmap (const QPixmap &pixmap);
    void setTextPosition(const eTextPosition& _value);
    eTextPosition getTextPosition() { return m_textPosition;}
    QFont getFont();
    void setFont(const QFont &font);
    void setTextColor(const QColor &color);
    QColor getTextColor();
    void setText (const QString &text);
    QString getText();
    void scalePixmap(int width,int height);
private:
    enum eSize { SNONE, STL,STC,STR,SML,SMR,SBL,SBC,SBR};
    k9MenuButton *m_button;
    QGraphicsPixmapItem *m_pixmap;
    QGraphicsSimpleTextItem *m_text;
    QPointF m_startPoint;
    void drawSizeRect (QPainter *painter);
    eSize m_resize;
    QPixmap m_bPixmap;
    eTextPosition m_textPosition;
    QString m_txtValue;
    QColor m_txtColor;
    int m_selsize;
protected:
    void mousePressEvent ( QGraphicsSceneMouseEvent * event ) ;
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
};


class k9MenuButton : public QObject {
    Q_OBJECT
public:


    k9MenuButton(k9MenuScene *parent = 0, const char *name = 0);

    ~k9MenuButton();
    void setWidth(int _value,bool _reload=true);
    int getWidth();
    void setHeight(int _value,bool _reload=true);
    int getHeight();
    void setTop(int _value,bool _reload=true);
    void setLeft(int _value,bool _reload=true);
    void setFont(QFont _font);
    QFont getFont();
    void setColor(QColor _color);
    QColor getColor();
    void setText(QString _text);
    QString getText();
    void select(bool);
    int getTop();
    int getLeft();

    void moveBy(int x,int y,bool _reload=true);
    void update();
    void remove();
public slots:
    void setImage(const QImage &_image);
    void setImage(const QString &_fileName);

    void setTextPosition(const eTextPosition& _value);
    eTextPosition getTextPosition() const;

    QImage getImage() const;

    void setScript(const QString& _value);
    QString getScript() const;

    void setAngle(int _value);
    int getAngle() const;

	k9CanvasSprite* getSprite() const;

	void setNum(int _value);
	

	int getNum() const;
	
	

private:
    k9MenuScene *m_scene;
    k9CanvasSprite *m_sprite;
    int m_num;

    QImage m_image;
    int  m_width;
    int m_height;
    int m_angle;
    eTextPosition m_textPosition;
    QString m_script;
    QPixmap m_pixmap;
    void loadImage();
signals:
    void sigsetTop(int);
    void sigsetLeft(int);
    void sigsetWidth(int);
    void sigsetHeight(int);

    void sigsetFont(const QFont &);
    void sigsetColor(const QColor &);
    void sigsetImage(k9MenuButton *,const QImage &);
    void sigsetText(const QString &);
    void sigsetTextPosition(int);
};

#endif
