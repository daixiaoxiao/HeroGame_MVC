#ifndef ARENAMODEL_H
#define ARENAMODEL_H

#include <memory>
#include "world.h"
#include "protagonistmodel.h"
#include "enemymodel.h"
#include "healthpackmodel.h"

class ArenaModel
{
public:
    ArenaModel(QString filename);
    std::vector<std::shared_ptr<Tile>> getTiles() const;
    std::shared_ptr<ProtagonistModel> getHeroModel() const;
    std::shared_ptr<EnemyModel> getEnemyModel() const;
    std::shared_ptr<HealthpackModel> getHealthpackModel() const;

    World getWorld() const;
    int getRows() const;
    int getCols() const;

private:
    std::vector<std::shared_ptr<Tile>> _tiles;
    std::shared_ptr<ProtagonistModel> _heroModel;
    std::shared_ptr<EnemyModel> _enemyModel;
    std::shared_ptr<HealthpackModel> _healthpackModel;
    World _world;
    int _rows;
    int _cols;
};

#endif // ARENAMODEL_H
