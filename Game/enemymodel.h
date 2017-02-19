#ifndef ENEMYMODEL_H
#define ENEMYMODEL_H

#include "world.h"

class EnemyModel
{
public:
    EnemyModel(std::vector<std::unique_ptr<Enemy>> e, int num_undefeated);
    std::vector<std::shared_ptr<Enemy> > getEnemies() const;

    int getCount_defeatedEnemy() const;

private:
    std::vector<std::shared_ptr<Enemy>> _enemies;
    int count_defeatedEnemy;
};

#endif // ENEMYMODEL_H
