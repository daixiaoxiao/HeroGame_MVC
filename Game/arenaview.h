#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <iostream>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <memory>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragLeaveEvent>
#include "world.h"
#include "arenascene.h"
#include "pathfinding2.h"
#include "node.h"

class ProtagonistModel;
class ArenaModel;

class ArenaView : public QGraphicsView
{
    Q_OBJECT
public:
    ArenaView(std::shared_ptr<ArenaModel> arena, QString filename);
    void renderAtBeginning();
    void renderTiles();
    void renderHero();
    void renderEnemy();
    void renderHeroItem();
    void renderHealthpack();
    void renderThePath(std::vector<std::shared_ptr<Node>> & path);
    void removeThePath();
    void renderPoisonedArea(float poisonlevel, int x, int y);
    void removePoisonedArea();

    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    int getSizeOfHpacks();
    ArenaScene *getArenaScene() const;

    void removeHpack(int x, int y);
    std::vector<std::shared_ptr<Tile> > getHealthpacksInArena() const;

    std::vector<std::shared_ptr<Tile>> _healthpacksInArena;

public slots:

    void defeatEnemy(int x, int y);
    void renderDropedHealthpack(int x, int y);

signals:
    void timeToUpdate();
    void updateNoOfHealthRemaing(int );
    void updateHealthPic();


private:
    ArenaScene * arenaScene;
    std::shared_ptr<ArenaModel> _arena;
    QString ImageName;
    int _Unit{30};

    std::vector<std::shared_ptr<Tile>> _tiles;
    int _cols;
    int _rows;

    std::vector<std::shared_ptr<Tile>> _healthpacksInRepository;
    std::vector<std::shared_ptr<Enemy>> _enemies;

    QGraphicsPixmapItem * heroPixItem;
    QGraphicsRectItem * healthBar;
    QGraphicsRectItem * energyBar;

    std::vector<QGraphicsRectItem *> pathNodeInView, hBarItems, poisonAreaItems;
    std::vector<QGraphicsPixmapItem *> hpackItems, enemyItems;
};

#endif // GAMEVIEW_H
