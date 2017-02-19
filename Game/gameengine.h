#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <memory>
#include <QTimer>
#include "arenamodel.h"
#include "arenaview.h"
#include "world.h"
#include "pathfinding2.h"
#include "stategy.h"

enum Direction{
    Up, Down, Left, Right
};

class GameEngine: public QObject
{
    Q_OBJECT
public:
    GameEngine(QString filename);

    void moveOneStep(Direction d);
    bool checkBeforeMove(int x, int y);
    void create_EnemyNodes();
    void create_HpackNodes();

    void eatHpacks(int x, int y);

    std::shared_ptr<Enemy> checkWhetherEnemy(int x, int y);
    std::shared_ptr<Tile> checkWhetherHpack(int x, int y);
    void exe_moveOneStep(int x, int y);

    std::shared_ptr<ArenaModel> getArenamodel() const;
    std::shared_ptr<ArenaView> getArenaview() const;

    void renderPath(std::vector<std::shared_ptr<Node>> & path);

    void go_KillEnemyOrGetHpack(std::vector<std::shared_ptr<Node> > &nodes);
    void prepareEnemyNodes(int x, int y);
    void prepareHpackNodes(int x, int y);

    void checkEnemyNodes(int x, int y);
    void checkHpackNodes(int x, int y);

    inline int getInterval(){return _interval;}
    inline void setInterval(int value){ _interval = value;
                                        _timer->setInterval(_interval);
                                                                        _AItimer->setInterval(_interval);}

signals:
    void sig_hpackErased(int x, int y);
    void sig_enemyErased(int x, int y);
    void sig_goToNextEnemy();

    void sig_renderOneHpack(int x, int y);

public slots:
    void render();
    void ExecutePathFinding(int x0, int y0, int x, int y);
    void moveToMousePosition(int x, int y);
    void messageDialog(QString text);
    void moveViaPath();
    void goOrAlterPath(std::shared_ptr<Node> next, QTimer * timer);
    void dealWithEnemy(int x, int y);
    void renderPoisonArea(float poisonlevel);
    void AI();
    void AImotion();

    void checkIfPositionIsOk(int x, int y);
    void updateData();

    void executePoisonEffect();
    void setPathFHeurist(QString value);
    void setStrategyHeurist(QString value);

private:
    std::shared_ptr<ArenaModel> arenamodel;
    std::shared_ptr<ArenaView> arenaview;

    std::vector<std::shared_ptr<Tile>> tiles;
    std::vector<std::shared_ptr<Tile>> hpacks;
    std::vector<std::shared_ptr<Enemy>> enemies;

    std::shared_ptr<Pathfinding2> pathfinding2;
    std::shared_ptr<Stategy> strategy;

    std::vector<std::shared_ptr<Node>> _path;
    QTimer* _timer;
    QTimer* _AItimer;
    QPoint* _point, * _original_point, * Pnemy_point;
    int _interval{100};
    int _hpIndex, _enemyIndex;

    std::vector<std::shared_ptr<Node>> enemyNodes;
    std::vector<std::shared_ptr<Node>> hPackNodes;

    std::vector<std::shared_ptr<Node>> trackNodes;

    std::shared_ptr<Enemy> enemy_temp;
    int count;
};

#endif // GAMEENGINE_H
