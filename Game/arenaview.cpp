#include "arenaview.h"
#include "protagonistmodel.h"
#include "arenamodel.h"
#include <QDebug>

ArenaView::ArenaView(std::shared_ptr<ArenaModel> arena, QString filename):
    _arena{arena}, ImageName{filename}
{

    arenaScene = new ArenaScene;
    arenaScene->setSceneRect(0, 0, arena->getCols() * _Unit, arena->getRows() * _Unit);

    _tiles = _arena->getTiles();
    _cols = _arena->getCols();
    _rows = _arena->getRows();
    _healthpacksInRepository = _arena->getHealthpackModel()->getHealthpacks();
    _enemies = _arena->getEnemyModel()->getEnemies();

    setAcceptDrops(true);
    this->renderAtBeginning();
    this->setScene(arenaScene);
    this->fitInView(0, 0, arena->getCols() * _Unit, arena->getRows() * _Unit, Qt::KeepAspectRatio);
}

void ArenaView::renderAtBeginning()
{
    renderTiles();

    renderHealthpack();
    renderEnemy();
    renderHero();
}

void ArenaView::renderTiles()
{
    QImage image(ImageName);
    for (int ii = 0; ii < image.height(); ii++) {
        QRgb *pixel = reinterpret_cast<QRgb*>(image.scanLine(ii));
        QRgb *end = pixel + image.width();
        for (; pixel != end; pixel++) {
            int gray = qGray(*pixel);
            *pixel = QColor(gray, gray, gray).rgb();
        }
    }
    QGraphicsPixmapItem * Image = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    Image->setScale(_Unit);
    Image->setPos(0, 0);
    arenaScene->addItem(Image);
}

void ArenaView::renderHero()
{
    std::shared_ptr<ProtagonistModel> h = _arena->getHeroModel();
    int x = h->getHero()->getXPos();
    int y = h->getHero()->getYPos();
    QPixmap * heroPix = new QPixmap(":images/hero.jpg");
    *heroPix = heroPix->scaled(_Unit - 3, _Unit - 3);
    heroPixItem = new QGraphicsPixmapItem(*heroPix);
    heroPixItem->setFocus();
    heroPixItem->setZValue(1);
    heroPixItem->setPos(x * _Unit + 1.5, y * _Unit + 1.5);
    arenaScene->addItem(heroPixItem);

    healthBar = new QGraphicsRectItem;
    healthBar->setRect(x * _Unit, y * _Unit, (h->getHero()->getHealth() / 100) * _Unit, 2);
    healthBar->setZValue(1);
    healthBar->setBrush(QBrush(QColor(0 , 255, 0)));
    healthBar->setTransform(QTransform().translate(0, -4));
    arenaScene->addItem(healthBar);

    energyBar = new QGraphicsRectItem;
    energyBar->setRect(x * _Unit, y * _Unit, (h->getHero()->getEnergy() / 100) * _Unit, 2);
    energyBar->setZValue(1);
    energyBar->setBrush(QBrush(QColor(255, 255, 0)));
    energyBar->setTransform(QTransform().translate(0, -(_Unit/15)+1));
    arenaScene->addItem(energyBar);
}

void ArenaView::renderHealthpack()
{
    while(_healthpacksInRepository.size() > ((_healthpacksInRepository.size()-3)))
    {
        auto hpack = _healthpacksInRepository.back();
        QPixmap * hpackPix = new QPixmap(":/images/health.jpeg");
        *hpackPix = hpackPix->scaled(_Unit - 3, _Unit - 3);
        QGraphicsPixmapItem * hpackPixItem = new QGraphicsPixmapItem(*hpackPix);
        hpackPixItem->setPos(hpack->getXPos() * _Unit + 1.5, hpack->getYPos() * _Unit + 1.5);
        hpackItems.push_back(hpackPixItem);
        arenaScene->addItem(hpackPixItem);

        QGraphicsRectItem * hpackBar = new QGraphicsRectItem;

        hpackBar->setRect(0, 0, (hpack->getValue() / 100) * _Unit, 2);
        hpackBar->setPos(hpack->getXPos() * _Unit, hpack->getYPos() * _Unit);
        hpackBar->setBrush(QBrush(QColor(0 , 255, 0)));
        hpackBar->setTransform(QTransform().translate(0, -1));
        hBarItems.push_back(hpackBar);

        arenaScene->addItem(hpackBar);

        _healthpacksInArena.push_back(_healthpacksInRepository.back()); // push the rendered health pack in to this vector i.e arena.

        _healthpacksInRepository.pop_back();
    }
}
void ArenaView::renderDropedHealthpack(int x, int y)
{
    if(_healthpacksInRepository.size() > 0){

        auto hpack = _healthpacksInRepository.back();

        hpack->setXPos(x);
        hpack->setYPos(y);
        QPixmap * hpackPix = new QPixmap(":/images/health.jpeg");
        *hpackPix = hpackPix->scaled(_Unit - 3, _Unit - 3);
        QGraphicsPixmapItem * hpackPixItem = new QGraphicsPixmapItem(*hpackPix);
        hpackPixItem->setPos(hpack->getXPos() * _Unit + 1.5, hpack->getYPos() * _Unit + 1.5);
        hpackItems.push_back(hpackPixItem);
        arenaScene->addItem(hpackPixItem);

        QGraphicsRectItem * hpackBar = new QGraphicsRectItem;
        hpackBar->setRect(0, 0, (hpack->getValue() / 100) * _Unit, 2);
        hpackBar->setPos(hpack->getXPos() * _Unit, hpack->getYPos() * _Unit);
        hpackBar->setBrush(QBrush(QColor(0 , 255, 0)));
        hpackBar->setTransform(QTransform().translate(0, -1));
        hBarItems.push_back(hpackBar);
        arenaScene->addItem(hpackBar);

        _healthpacksInArena.push_back(_healthpacksInRepository.back());
        _healthpacksInRepository.pop_back();
        emit timeToUpdate();
        emit updateNoOfHealthRemaing(_healthpacksInRepository.size());
        if( _healthpacksInRepository.size() == 0){
            emit updateHealthPic();
        }
    }
}

std::vector<std::shared_ptr<Tile> > ArenaView::getHealthpacksInArena() const
{
    return _healthpacksInArena;
}

void ArenaView::removeHpack(int x, int y)
{
    for(std::vector<QGraphicsPixmapItem *>::iterator it = hpackItems.begin(); it != hpackItems.end(); ++it)
    {
        if((*it)->x() == (x * _Unit + 1.5) && (*it)->y() == (y * _Unit + 1.5))
        {
            arenaScene->removeItem(*it);
            break;
        }
    }
    for(std::vector<QGraphicsRectItem *>::iterator it = hBarItems.begin(); it != hBarItems.end(); ++it)
    {
        if((*it)->x() == (x * _Unit) && (*it)->y() == (y * _Unit))
        {
            arenaScene->removeItem(*it);
            break;
        }
    }
}

void ArenaView::defeatEnemy(int x, int y)
{
    std::cout << x << " " << y << std::endl;
    for(std::vector<QGraphicsPixmapItem *>::iterator it = enemyItems.begin(); it != enemyItems.end(); ++it)
    {
        if((*it)->x() == (x * _Unit + 1.5) && (*it)->y() == (y * _Unit + 1.5))
        {
            QPixmap * defeatedEnemyPix = new QPixmap(":/images/DefeatedEnemy.jpeg");
            *defeatedEnemyPix = defeatedEnemyPix->scaled(_Unit - 3, _Unit - 3);
            (*it)->setPixmap(*defeatedEnemyPix);
            break;
        }
    }
}

void ArenaView::renderEnemy()
{
    for(auto enemy : _enemies)
    {
        QString typeInformation= typeid(*enemy).name();

        QGraphicsRectItem * enemyBar = new QGraphicsRectItem;
        enemyBar->setRect(enemy->getXPos() * _Unit, enemy->getYPos() * _Unit, (enemy->getValue() / 100) * _Unit, 2);
        enemyBar->setTransform(QTransform().translate(0, -1));
        enemyBar->setBrush(QBrush(QColor(255, 0, 0)));
        arenaScene->addItem(enemyBar);

        QPixmap * enemyPix;
        if(typeInformation == "5Enemy"){
            if(enemy->getDefeated())
            {
                enemyPix = new QPixmap(":/images/DefeatedEnemy.jpeg");
            }
            else
            {
                enemyPix = new QPixmap(":/images/Enemy.jpeg");
            }
        }else{
            enemyPix = new QPixmap(":/images/penemy.jpeg");
        }

        *enemyPix = enemyPix->scaled(_Unit - 3, _Unit - 3);
        QGraphicsPixmapItem * enemyPixItem = new QGraphicsPixmapItem(*enemyPix);
        enemyPixItem->setPos(enemy->getXPos() * _Unit + 1.5, enemy->getYPos() * _Unit + 1.5);

        enemyItems.push_back(enemyPixItem);
        arenaScene->addItem(enemyPixItem);

    }
}


void ArenaView::renderHeroItem()
{
    std::shared_ptr<ProtagonistModel> h = _arena->getHeroModel();
    int x = h->getHero()->getXPos();
    int y = h->getHero()->getYPos();
    heroPixItem->setPos(x * _Unit + 1.5, y * _Unit + 1.5);
    healthBar->setRect(x * _Unit, y * _Unit, (h->getHero()->getHealth() / 100) * _Unit, 2);
    energyBar->setRect(x * _Unit, y * _Unit, (h->getHero()->getEnergy() / 100) * _Unit, 2);
}

void ArenaView::renderThePath(std::vector<std::shared_ptr<Node>> & path)
{
    std::reverse(path.begin(), path.end());

    for(auto node : path)
    {
        QGraphicsRectItem * node_rect = new QGraphicsRectItem;
        node_rect->setRect(node->getX() * _Unit + 2.5, node->getY() * _Unit + 2.5, _Unit - 5, _Unit - 5);
        node_rect->setBrush(QColor(0,0,255,100));
        node_rect->setZValue(0);
        pathNodeInView.push_back(node_rect);
        arenaScene->addItem(node_rect);
    }
}

void ArenaView::removeThePath()
{
    for(auto node : pathNodeInView)
    {
        arenaScene->removeItem(node);
    }
    pathNodeInView.clear();
}

void ArenaView::renderPoisonedArea(float poisonlevel, int x, int y)
{
    for(int i = -1; i != 2; i++)
    {
        for(int j = -1; j != 2; j++)
        {
            if(i == 0 && j == 0)
            {
                continue;
            }
            int poison_x = x + i;
            int poison_y = y + j;
            if((poison_x >= 0 && poison_x < _cols) && (poison_y >= 0 && poison_y < _rows))
            {
                QGraphicsRectItem * poison_rect = new QGraphicsRectItem(poison_x * _Unit + 2.5, poison_y * _Unit + 2.5, _Unit - 5, _Unit - 5);
                poison_rect->setBrush(QBrush(QColor(89, 2, 117, poisonlevel * 2.5), Qt::Dense1Pattern));
                poisonAreaItems.push_back(poison_rect);
                arenaScene->addItem(poison_rect);
            }
        }
    }
}

void ArenaView::removePoisonedArea()
{
    qDebug() << "pnemy dead!";
    for(auto item : poisonAreaItems)
    {
        arenaScene->removeItem(item);
    }
    poisonAreaItems.clear();
}


void ArenaView::dragMoveEvent(QDragMoveEvent *event)
{
    QGraphicsView::dragMoveEvent(event);
}

void ArenaView::dropEvent(QDropEvent *event)
{
    QGraphicsView::dropEvent(event);
}

int ArenaView::getSizeOfHpacks()
{
    return _healthpacksInRepository.size();
}

ArenaScene *ArenaView::getArenaScene() const
{
    return arenaScene;
}
