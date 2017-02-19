#ifndef HEALTHPACKMODEL_H
#define HEALTHPACKMODEL_H

#include "world.h"

class HealthpackModel
{
public:
    HealthpackModel(std::vector<std::unique_ptr<Tile> > h);

    std::vector<std::shared_ptr<Tile> > getHealthpacks() const;

private:
    std::vector<std::shared_ptr<Tile>> _healthpacks;
};

#endif // HEALTHPACKMODEL_H
