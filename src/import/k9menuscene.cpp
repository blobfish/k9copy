#include "k9menuscene.h"
#include <QDebug>
k9MenuScene::k9MenuScene(QObject *parent):QGraphicsScene(parent)
{
    QPixmap pm(720,576);
    m_pixmap=pm;
    m_pixmap.fill(Qt::black);
}


void k9MenuScene::drawBackground ( QPainter * painter, const QRectF & rect ) {
    QGraphicsScene::drawBackground(painter,rect);
    QRectF r=this->sceneRect().intersected(rect);
    if ((rect.left() < r.left()) || (rect.right()>r.right()) ||
        (rect.top()< r.top()) || (rect.bottom()>r.bottom())) {
        QBrush br(Qt::SolidPattern);
        br.setColor(Qt::gray);
        painter->fillRect(rect,br);
    }
    painter->drawPixmap(r.toRect(), m_pixmap.copy(r.toRect()));
}

void k9MenuScene::setBackgroundPixmap(const QPixmap &pixmap) {
    m_pixmap=pixmap;
    update(sceneRect());

}
