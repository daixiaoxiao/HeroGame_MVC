#include "arenamodel.h"
#include <QString>

ArenaModel::ArenaModel(QString filename)
{
  _world = World();
  for(auto & tile : _world.createWorld(filename))
  {
      _tiles.push_back(std::move(tile));
  }
  _rows = _world.getRows();
  _cols = _world.getCols();

  _heroModel = std::make_shared<ProtagonistModel>(_world.getProtagonist());
  _enemyModel = std::make_shared<EnemyModel>(_world.getEnemies(6), 2);
  _healthpackModel = std::make_shared<HealthpackModel>(_world.getHealthPacks(5));
}

std::vector<std::shared_ptr<Tile> > ArenaModel::getTiles() const
{
    return _tiles;
}

std::shared_ptr<ProtagonistModel> ArenaModel::getHeroModel() const
{
    return _heroModel;
}

std::shared_ptr<EnemyModel> ArenaModel::getEnemyModel() const
{
    return _enemyModel;
}

std::shared_ptr<HealthpackModel> ArenaModel::getHealthpackModel() const
{
    return _healthpackModel;
}

World ArenaModel::getWorld() const
{
    return _world;
}

int ArenaModel::getRows() const
{
    return _rows;
}

int ArenaModel::getCols() const
{
    return _cols;
}



