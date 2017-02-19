#include "arenascene.h"
#include <QDebug>
#include <QDrag>
#include <QMimeData>
#include <QPainter>
#include <QLabel>
#include <QPoint>
#include <QPointF>

ArenaScene::ArenaScene()
{
}

void ArenaScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        int x = event->scenePos().x() / 30;
        int y = event->scenePos().y() / 30;
        emit sig_mousePosition(x, y);
    }

}

void ArenaScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void ArenaScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/Demo-QLabeldata"))
    {
        int x = event->scenePos().x() / 30;
        int y = event->scenePos().y() / 30;
        emit sig_createHealth(x,y);
    }

}
