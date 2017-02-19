#ifndef ARENASCENE_H
#define ARENASCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>

class ArenaScene: public QGraphicsScene
{
    Q_OBJECT

public:
    ArenaScene();
    void mousePressEvent(QGraphicsSceneMouseEvent * event);

    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

signals:
    void sig_mousePosition(int x, int y);
    void sig_createHealth(int x, int y);
};

#endif // ARENASCENE_H
