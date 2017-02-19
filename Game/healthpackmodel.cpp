#include "healthpackmodel.h"

HealthpackModel::HealthpackModel(std::vector<std::unique_ptr<Tile>> h)
{
    for(auto & healthpack : h)
    {
        _healthpacks.push_back(std::move(healthpack));
    }
}

std::vector<std::shared_ptr<Tile>> HealthpackModel::getHealthpacks() const
{
    return _healthpacks;
}
