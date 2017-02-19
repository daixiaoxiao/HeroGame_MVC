#include "enemymodel.h"
#include <QDebug>

EnemyModel::EnemyModel(std::vector<std::unique_ptr<Enemy>> e, int num_undefeated)
{
    for(auto & enemy : e)
    {
        _enemies.push_back(std::move(enemy));
    }

    int i = 0;
    for(auto e_ptr : _enemies)
    {
        QString typeInformation= typeid(*e_ptr).name();

        if(typeInformation == "5Enemy")
        {
            e_ptr->setDefeated(true);
            i++;
            if(i == num_undefeated)
            {
                break;
            }
        }
    }
    count_defeatedEnemy = e.size() - num_undefeated;
}

std::vector<std::shared_ptr<Enemy> > EnemyModel::getEnemies() const
{
    return _enemies;
}

int EnemyModel::getCount_defeatedEnemy() const
{
    return count_defeatedEnemy;
}
