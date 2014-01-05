#ifndef K9MENUSCENE_H
#define K9MENUSCENE_H

#include <QGraphicsScene>
#include <QPainter>
class k9MenuScene : public QGraphicsScene
{
    Q_OBJECT
public:
    k9MenuScene(QObject *parent);
    void setBackgroundPixmap(const QPixmap &);
protected:
    void drawBackground ( QPainter * painter, const QRectF & rect );
private:
    QPixmap m_pixmap;
};

#endif // K9MENUSCENE_H
