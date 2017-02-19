#include "gameengine.h"
#include "mainwindow.h"
#include <iostream>
#include <algorithm>
#include <cassert>
#include <qdebug.h>

GameEngine::GameEngine(QString filename)
{
    arenamodel = std::make_shared<ArenaModel>(filename);
    arenaview = std::make_shared<ArenaView>(arenamodel, filename);
    arenamodel->getHeroModel()->getHero()->setPos(0, 0);

    tiles = arenamodel->getTiles();
    hpacks = arenaview->_healthpacksInArena;
    enemies = arenamodel->getEnemyModel()->getEnemies();
    count = arenamodel->getEnemyModel()->getCount_defeatedEnemy();

    _timer = new QTimer(this);
    _timer->setInterval(_interval);

    _AItimer = new QTimer(this);
    _AItimer->setInterval(_interval);

    pathfinding2 = std::make_shared<Pathfinding2>(arenamodel);
    strategy = std::make_shared<Stategy>(arenamodel);

    create_EnemyNodes();
    create_HpackNodes();

    QObject::connect(_timer, &QTimer::timeout,
                     this, &GameEngine::moveViaPath);
    QObject::connect(_AItimer,&QTimer::timeout,
                     this, &GameEngine::AImotion);
    QObject::connect(arenamodel->getHeroModel()->getHero().get(), &Protagonist::posChanged,
                     this, &GameEngine::render);
    QObject::connect(arenaview->getArenaScene(), &ArenaScene::sig_mousePosition,
                     this, &GameEngine::moveToMousePosition);
    QObject::connect(this, &GameEngine::sig_hpackErased,
                     arenaview.get(), &ArenaView::removeHpack);
    QObject::connect(this, &GameEngine::sig_enemyErased,
                     arenaview.get(), &ArenaView::defeatEnemy);
    QObject::connect(this, &GameEngine::sig_goToNextEnemy,
                     this, &GameEngine::AI);
    QObject::connect(arenaview->getArenaScene(), &ArenaScene::sig_createHealth,
                     this, &GameEngine::checkIfPositionIsOk);
    QObject::connect(this, &GameEngine::sig_renderOneHpack,
                     arenaview.get(), &ArenaView::renderDropedHealthpack);
    QObject::connect(arenaview.get(), &ArenaView::timeToUpdate,
                     this, &GameEngine::updateData);
}

void GameEngine::checkIfPositionIsOk(int x, int y)
{
    int i = x;
    int j = y;
    auto compPos = [&i,&j](std::shared_ptr<Tile> a){

        return  (a->getXPos() == i && a->getYPos() == j);

    };
    auto enemyNode = std::find_if(enemies.begin(),enemies.end(),compPos);
    auto health = std::find_if(arenaview->_healthpacksInArena.begin(),arenaview->_healthpacksInArena.end(),compPos);
    int cols = arenamodel->getCols();
    float graylevel = tiles.at(x+y*cols)->getValue();
    int heroX = arenamodel->getHeroModel()->getHero()->getXPos();
    int heroY = arenamodel->getHeroModel()->getHero()->getYPos();

    if( arenaview->getSizeOfHpacks() > 0 ){

        if(checkBeforeMove(x,y) && enemyNode == enemies.end() && std::isfinite(graylevel) && !(x == heroX && y == heroY) && health == arenaview->_healthpacksInArena.end() ) {
            sig_renderOneHpack(x,y);
        }else{
            messageDialog("You can not place health pack in that position");
        }

    }else {
        messageDialog("Extra Health pack is not available");
    }
}

void GameEngine::updateData()
{
    create_HpackNodes();
}

void GameEngine::create_EnemyNodes()
{
    for(auto x: enemies){
        if(x->getDefeated() == false){
            std::shared_ptr<Node> enemyNode = std::make_shared<Node>(x->getXPos(),x->getYPos(),std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());

            enemyNodes.push_back(enemyNode);
        }
    }
}

void GameEngine::create_HpackNodes()
{
    hPackNodes.clear();
    hpacks = arenaview->_healthpacksInArena;
    for(auto x: hpacks){
        std::shared_ptr<Node> hPackNode = std::make_shared<Node>(x->getXPos(),x->getYPos(),std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());
        hPackNodes.push_back(hPackNode);
    }
}

void GameEngine::render()
{
    arenaview->renderHeroItem();
}

void GameEngine::renderPath(std::vector<std::shared_ptr<Node>> & path)
{
    arenaview->renderThePath(path);
}

void GameEngine::go_KillEnemyOrGetHpack(std::vector<std::shared_ptr<Node>> & nodes)
{
    std::shared_ptr<Node> nextNode= nodes.back();
    _point = new QPoint(nextNode->getX(),nextNode->getY());

    int x0 = arenamodel->getHeroModel()->getHero()->getXPos();
    int y0 = arenamodel->getHeroModel()->getHero()->getYPos();
    pathfinding2->findPath(x0, y0, _point->rx(),_point->ry(),_path);
    renderPath(_path);
    std::reverse(_path.begin(), _path.end());
    _AItimer->start();
    nodes.pop_back();
}

void GameEngine::prepareEnemyNodes(int x, int y)
{
    for(std::shared_ptr<Node> enemyNode: enemyNodes){
        strategy->findCost(x,y,enemyNode->getX(),enemyNode->getY(),enemyNode);
    }

    std::stable_sort(enemyNodes.begin(),enemyNodes.end(),[](const std::shared_ptr<Node> e1,const std::shared_ptr<Node> e2){
        return e1->getFinalCost() > e2->getFinalCost();
    });
}

void GameEngine::prepareHpackNodes(int x, int y)
{
    for(std::shared_ptr<Node> hPackNode: hPackNodes){
        strategy->findCost(x,y,hPackNode->getX(),hPackNode->getY(),hPackNode);
    }

    std::stable_sort(hPackNodes.begin(),hPackNodes.end(),[](const std::shared_ptr<Node> e1,const std::shared_ptr<Node> e2){
        return e1->getFinalCost() > e2->getFinalCost();
    });
}

void GameEngine::ExecutePathFinding(int x0, int y0, int x, int y)
{
    _point = new QPoint(x,y);
    _original_point = new QPoint(x0,y0);

    pathfinding2->findPath(_original_point->rx(),_original_point->ry(),_point->rx(),_point->ry(), _path);

    renderPath(_path);
    std::reverse(_path.begin(), _path.end());

    _timer->start();
}

void GameEngine::moveToMousePosition(int x, int y)
{
    trackNodes.clear();
    std::shared_ptr<Protagonist> prota = arenamodel->getHeroModel()->getHero();
    int x0 = prota->getXPos();
    int y0 = prota->getYPos();
    float h = prota->getHealth();
    if(!(x == x0 && y == y0) && checkBeforeMove(x,y))
    {
        if(auto e_ptr = checkWhetherEnemy(x, y))
        {
            if(!e_ptr->getDefeated())
            {
                h = h - e_ptr->getValue();
                if(h > 0)
                {
                    ExecutePathFinding(x0, y0, x, y);
                }
                else
                {
                    messageDialog("You can't destroy it! Find another enemy or go eat healthpack");
                }
            }
            else
            {
                messageDialog("You can't destroy defeated enemy!");
            }
        }
        else
        {
            ExecutePathFinding(x0, y0, x, y);
        }
    }
}

void GameEngine::messageDialog(QString text)
{
    QMessageBox msgBox;
    msgBox.setText(text);
    msgBox.exec();
}

void GameEngine::moveViaPath()
{
    std::shared_ptr<Protagonist> prota = arenamodel->getHeroModel()->getHero();

    if(!_path.empty())
    {
        std::shared_ptr<Node> next =  _path.back();
        goOrAlterPath(next, _timer);
        _path.pop_back();
    }

    if (prota->getXPos() == _point->rx() && prota->getYPos() == _point->ry())
    {
        _timer->stop();
        arenaview->removeThePath();

        int x = _point->rx();
        int y = _point->ry();

        dealWithEnemy(x, y);

        eatHpacks(x, y);
    }
}

void GameEngine::goOrAlterPath(std::shared_ptr<Node> next, QTimer * timer)
{
    std::shared_ptr<Protagonist> prota = arenamodel->getHeroModel()->getHero();
    float e = prota->getEnergy();

    int i = next->getX();
    int j = next->getY();
    auto compPos = [&i,&j](std::shared_ptr<Tile> a){

        return  (a->getXPos() == i && a->getYPos() == j);

    };
    auto enemy = std::find_if(std::begin(enemies),std::end(enemies),compPos);
    auto hpack = std::find_if(std::begin(hpacks),std::end(hpacks),compPos);
    if((enemy != std::end(enemies) || hpack != std::end(hpacks) )&& !(i == _point->rx() && j == _point->ry())){
        timer->stop();
        arenaview->removeThePath();

        trackNodes.push_back(next);

        pathfinding2->findAlternatePath(prota->getXPos(),prota->getYPos(),_point->rx(),_point->ry(),_path,trackNodes);
        renderPath(_path);
        std::reverse(_path.begin(), _path.end());
        timer->start();
    }else{
        prota->setPos(next->getX(), next->getY());
        e = e - (1 - (tiles.at(next->getX() + next->getY() * arenamodel->getCols()))->getValue());
        if(e > 0)
        {
            arenamodel->getHeroModel()->setEnergyLevel(e);
        }
        else
        {
            timer->stop();
            arenaview->removeThePath();
            arenamodel->getHeroModel()->setEnergyLevel(0);
            messageDialog("Not enough Energy. You Died!");
        }
    }
}

void GameEngine::dealWithEnemy(int x, int y)
{
    std::shared_ptr<Protagonist> prota = arenamodel->getHeroModel()->getHero();
    float h = prota->getHealth();
    if(auto e_ptr = checkWhetherEnemy(x, y))
    {
        QString typeInformation= typeid(*e_ptr).name();
        if(typeInformation == "5Enemy")
        {
            h = h - e_ptr->getValue();
            if(h > 0)
            {
                emit sig_enemyErased(x, y);
                e_ptr->setDefeated(true);

                checkEnemyNodes(x, y);

                arenamodel->getHeroModel()->setHealthLevel(h);
                arenamodel->getHeroModel()->setEnergyLevel(100);
                if(!(count = count - 1))
                {
                    messageDialog("Congratulationns, You Won!");
                }
            }
            else
            {
                arenamodel->getHeroModel()->setHealthLevel(0);
                messageDialog("Not enough Health.You Lose");
            }
        }
        else
        {
            PEnemy * pe = dynamic_cast<PEnemy *>(e_ptr.get());
            QObject::connect(pe, &PEnemy::poisonLevelUpdated,
                             this, &GameEngine::renderPoisonArea);
            QObject::connect(pe, &PEnemy::dead,
                             arenaview.get(), &ArenaView::removePoisonedArea);
            QObject::connect(pe, &PEnemy::poisonLevelUpdated,
                             this, &GameEngine::executePoisonEffect);

            enemy_temp = e_ptr;
            Pnemy_point = new QPoint(x, y);
            pe->poison();

            checkEnemyNodes(x, y);

            emit sig_enemyErased(enemy_temp->getXPos(), enemy_temp->getYPos());
            enemy_temp->setDefeated(true);
            arenamodel->getHeroModel()->setEnergyLevel(100);
            if(!(count = count - 1))
            {
                messageDialog("Congratulationns, You Won!");
            }
        }
    }
}

void GameEngine::AI()
{
    trackNodes.clear();
    float h = arenamodel->getHeroModel()->getHero()->getHealth();
    int x0 = arenamodel->getHeroModel()->getHero()->getXPos();
    int y0 = arenamodel->getHeroModel()->getHero()->getYPos();
    if( !(h > 0)){
        _AItimer->stop();
        arenaview->removeThePath();
        messageDialog("not Enough Health");
    }

    if(enemyNodes.size() != 0){

        prepareEnemyNodes(x0, y0);

        if(hPackNodes.size() != 0)
        {
            prepareHpackNodes(x0, y0);
            std::shared_ptr<Node> nextEnemyNode= enemyNodes.back();

            bool NextNodeHealth = false;
            int i = nextEnemyNode->getX();
            int j = nextEnemyNode->getY();
            auto compPos = [&i,&j](std::shared_ptr<Tile> a){

                return  (a->getXPos() == i && a->getYPos() == j);

            };
            auto enemy = std::find_if(enemies.begin(),enemies.end(),compPos);
            if(enemy != enemies.end()){
                NextNodeHealth = (h <= (*enemy)->getValue())? true: false;
            }

            if(!NextNodeHealth)
            {
                go_KillEnemyOrGetHpack(enemyNodes);
            }
            else
            {
                go_KillEnemyOrGetHpack(hPackNodes);
            }
        }
        else
        {
            go_KillEnemyOrGetHpack(enemyNodes);
        }
    }else{
        _AItimer->stop();
        arenaview->removeThePath();
        messageDialog("congratulations, You Won!!!");
    }
}

void GameEngine::AImotion()
{
    std::shared_ptr<Protagonist> prota = arenamodel->getHeroModel()->getHero();
    float h = prota->getHealth();
    if(!_path.empty())
    {
        std::shared_ptr<Node> next =  _path.back();
        goOrAlterPath(next, _AItimer);
        _path.pop_back();
    }

    if (prota->getXPos() == _point->rx() && prota->getYPos() == _point->ry())
    {
        _AItimer->stop();
        arenaview->removeThePath();

        h = prota->getHealth();
        int x = _point->rx();
        int y = _point->ry();

        if(auto e_ptr = checkWhetherEnemy(x, y))
        {
            QString typeInformation= typeid(*e_ptr).name();
            if(typeInformation == "5Enemy")
            {
                h = h - e_ptr->getValue();
                if(h > 0)
                {
                    emit sig_enemyErased(x, y);
                    e_ptr->setDefeated(true);

                    arenamodel->getHeroModel()->setHealthLevel(h);
                    arenamodel->getHeroModel()->setEnergyLevel(100);

                    emit sig_goToNextEnemy();
                }
                else
                {
                    _AItimer->stop();
                    arenamodel->getHeroModel()->setHealthLevel(0);
                    messageDialog("Not enough Health.You Lose");
                }
            }
            else
            {

                PEnemy * pe = dynamic_cast<PEnemy *>(e_ptr.get());
                QObject::connect(pe, &PEnemy::poisonLevelUpdated,
                                 this, &GameEngine::renderPoisonArea);
                QObject::connect(pe, &PEnemy::dead,
                                 arenaview.get(), &ArenaView::removePoisonedArea);
                QObject::connect(pe, &PEnemy::poisonLevelUpdated,
                                 this, &GameEngine::executePoisonEffect);

                enemy_temp = e_ptr;
                Pnemy_point = new QPoint(x, y);
                pe->poison();

                emit sig_enemyErased(enemy_temp->getXPos(), enemy_temp->getYPos());
                enemy_temp->setDefeated(true);
                arenamodel->getHeroModel()->setEnergyLevel(100);

                emit sig_goToNextEnemy();
            }
        }

        if(auto h_ptr = checkWhetherHpack(x, y))
        {
            float h = arenamodel->getHeroModel()->getHero()->getHealth();
            if((h + h_ptr->getValue()) > 100)
            {
                arenamodel->getHeroModel()->setHealthLevel(100);
            }
            else
            {
                arenamodel->getHeroModel()->setHealthLevel(h + h_ptr->getValue());
            }
            emit sig_hpackErased(h_ptr->getXPos(),h_ptr->getYPos());

            checkHpackNodes(x,y);

            emit sig_goToNextEnemy();
        }
    }
}

void GameEngine::executePoisonEffect()
{
    std::shared_ptr<Protagonist> prota = arenamodel->getHeroModel()->getHero();
    int x = prota->getXPos();
    int y = prota->getYPos();
    if((x >= enemy_temp->getXPos() - 1 && x <= enemy_temp->getXPos() + 1) && (y >= enemy_temp->getYPos() - 1 && y <= enemy_temp->getYPos() + 1) && !(enemyNodes.empty()))
    {
        arenamodel->getHeroModel()->setHealthLevel(prota->getHealth() - 10);
    }
}

void GameEngine::setPathFHeurist(QString value)
{
    pathfinding2->setHeuristicWeight(value);
}

void GameEngine::setStrategyHeurist(QString value)
{
    strategy->setHeuristicWeight(value);
}

void GameEngine::renderPoisonArea(float poisonlevel)
{
    arenaview->renderPoisonedArea(poisonlevel, Pnemy_point->rx(), Pnemy_point->ry());
}

std::shared_ptr<ArenaView> GameEngine::getArenaview() const
{
    return arenaview;
}

std::shared_ptr<ArenaModel> GameEngine::getArenamodel() const
{
    return arenamodel;
}

void GameEngine::moveOneStep(Direction d)
{
    int x = arenamodel->getHeroModel()->getHero()->getXPos();
    int y = arenamodel->getHeroModel()->getHero()->getYPos();
    switch(d){
    case Up:
        exe_moveOneStep(x, y-1);
        break;
    case Down:
        exe_moveOneStep(x, y+1);
        break;
    case Left:
        exe_moveOneStep(x-1, y);
        break;
    case Right:
        exe_moveOneStep(x+1, y);
        break;
    }
    x = arenamodel->getHeroModel()->getHero()->getXPos();
    y = arenamodel->getHeroModel()->getHero()->getYPos();
    eatHpacks(x, y);
}

bool GameEngine::checkBeforeMove(int x, int y)
{
    if( (x >= 0 && x < arenamodel->getCols()) && (y >= 0 && y < arenamodel->getRows()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void GameEngine::eatHpacks(int x, int y)
{
    if(auto h_ptr = checkWhetherHpack(x, y))
    {
        assert(h_ptr->getXPos() == x && h_ptr->getYPos() == y);
        float h = arenamodel->getHeroModel()->getHero()->getHealth();
        if((h + h_ptr->getValue()) > 100)
        {
            arenamodel->getHeroModel()->setHealthLevel(100);
        }
        else
        {
            arenamodel->getHeroModel()->setHealthLevel(h + h_ptr->getValue());
        }
        emit sig_hpackErased(x, y);
        hpacks.erase(hpacks.begin() + _hpIndex);
        checkHpackNodes(x, y);
    }
}

std::shared_ptr<Enemy> GameEngine::checkWhetherEnemy(int x, int y)
{
    for(std::vector<std::shared_ptr<Enemy>>::iterator it = enemies.begin(); it != enemies.end(); ++it)
    {
        if((*it)->getXPos() == x && (*it)->getYPos() == y)
        {
            return *it;
        }
    }
    return nullptr;
}

std::shared_ptr<Tile> GameEngine::checkWhetherHpack(int x, int y)
{
    _hpIndex = 0;
    for(std::vector<std::shared_ptr<Tile>>::iterator it = hpacks.begin(); it != hpacks.end(); ++it)
    {
        if((*it)->getXPos() == x && (*it)->getYPos() == y)
        {
            return *it;
        }
        _hpIndex++;
    }
    return nullptr;
}

void GameEngine::checkEnemyNodes(int x, int y)
{
    int i = 0;
    for(std::vector<std::shared_ptr<Node>>::iterator it = enemyNodes.begin(); it != enemyNodes.end(); ++it)
    {
        if((*it)->getX() == x && (*it)->getY() == y)
        {
            enemyNodes.erase(enemyNodes.begin() + i);
            break;
        }
        i++;
    }
}

void GameEngine::checkHpackNodes(int x, int y)
{
    int i = 0;
    for(std::vector<std::shared_ptr<Tile>>::iterator it = (arenaview->_healthpacksInArena).begin(); it != (arenaview->_healthpacksInArena).end(); ++it)
    {
        if((*it)->getXPos() == x && (*it)->getYPos() == y)
        {
            ( arenaview->_healthpacksInArena).erase( ( arenaview->_healthpacksInArena).begin() + i);
            break;
        }
        i++;
    }
    create_HpackNodes();
}

void GameEngine::exe_moveOneStep(int x, int y)
{
    float e = arenamodel->getHeroModel()->getHero()->getEnergy();
    float h = arenamodel->getHeroModel()->getHero()->getHealth();

    if(checkBeforeMove(x, y))
    {
        if(auto e_ptr = checkWhetherEnemy(x, y))
        {
            if(!e_ptr->getDefeated())
            {
                QString typeInformation= typeid(*e_ptr).name();
                if(typeInformation == "5Enemy")
                {
                    h = h - e_ptr->getValue();
                    if(h > 0)
                    {
                        arenamodel->getHeroModel()->getHero()->setPos(x, y);
                        e = e - (1 - (arenamodel->getTiles().at(x + y * arenamodel->getCols()))->getValue());
                        if(e > 0)
                        {
                            arenamodel->getHeroModel()->setEnergyLevel(e);

                            emit sig_enemyErased(x, y);
                            e_ptr->setDefeated(true);
                            tiles.at(x + y * arenamodel->getCols())->setValue(std::numeric_limits<float>::infinity());

                            checkEnemyNodes(x, y);

                            if(!(count = count - 1))
                            {
                                messageDialog("Congratulationns, You Won!");
                            }

                            arenamodel->getHeroModel()->setHealthLevel(h);
                            arenamodel->getHeroModel()->setEnergyLevel(100);
                        }
                        else
                        {
                            messageDialog("Not enough Energy.");
                        }
                    }
                    else
                    {
                        messageDialog("Not enough Health.");
                    }

                }
                else
                {
                    arenamodel->getHeroModel()->getHero()->setPos(x, y);
                    e = e - (1 - (arenamodel->getTiles().at(x + y * arenamodel->getCols()))->getValue());
                    if(e > 0)
                    {
                        arenamodel->getHeroModel()->setEnergyLevel(e);
                        PEnemy * pe = dynamic_cast<PEnemy *>(e_ptr.get());
                        QObject::connect(pe, &PEnemy::poisonLevelUpdated,
                                         this, &GameEngine::renderPoisonArea);
                        QObject::connect(pe, &PEnemy::dead,
                                         arenaview.get(), &ArenaView::removePoisonedArea);
                        QObject::connect(pe, &PEnemy::poisonLevelUpdated,
                                         this, &GameEngine::executePoisonEffect);

                        enemy_temp = e_ptr;
                        Pnemy_point = new QPoint(x, y);
                        pe->poison();
                        tiles.at(x + y * arenamodel->getCols())->setValue(std::numeric_limits<float>::infinity());

                        checkEnemyNodes(x, y);

                        emit sig_enemyErased(enemy_temp->getXPos(), enemy_temp->getYPos());
                        enemy_temp->setDefeated(true);
                        arenamodel->getHeroModel()->setEnergyLevel(100);
                        if(!(count = count - 1))
                        {
                            messageDialog("Congratulationns, You Won!");
                        }
                    }
                    else
                    {
                        messageDialog("Not enough Energy For Pnemy Part.You Lose");
                    }
                }
            }
        }
        else
        {
            arenamodel->getHeroModel()->getHero()->setPos(x, y);
            e = e - (1 - (arenamodel->getTiles().at(x + y * arenamodel->getCols()))->getValue());
            if(e > 0)
            {
                arenamodel->getHeroModel()->setEnergyLevel(e);
            }
            else
            {
                arenamodel->getHeroModel()->setEnergyLevel(0);
                messageDialog("Not enough Energy.You Lose");
            }
        }
    }
}






