#include "protagonistmodel.h"

ProtagonistModel::ProtagonistModel(std::unique_ptr<Protagonist> p)
{
    _hero = std::move(p);
}

std::shared_ptr<Protagonist> ProtagonistModel::getHero() const
{
    return _hero;
}

void ProtagonistModel::setEnergyLevel(int value)
{
    _hero->setEnergy(value);
    emit sig_energyChanged(value);
}

void ProtagonistModel::setHealthLevel(float value)
{
    _hero->setHealth(value);
    emit sig_healthChanged(value);
}

