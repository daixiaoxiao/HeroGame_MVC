#ifndef PROTAGONISTMODEL_H
#define PROTAGONISTMODEL_H

#include <QObject>
#include <memory>
#include "world.h"

class ProtagonistModel:public QObject
{
    Q_OBJECT

public:
    ProtagonistModel(std::unique_ptr<Protagonist> p);

    std::shared_ptr<Protagonist> getHero() const;
    void setEnergyLevel(int value);
    void setHealthLevel(float value);

signals:
    void sig_energyChanged(int value);
    void sig_healthChanged(float value);

private:
    std::shared_ptr<Protagonist> _hero;
};

#endif // PROTAGONISTMODEL_H
