#include "pathfinding2.h"
#include <QDebug>
#include <cmath>
#include <utility>
#include <iostream>
#include <cassert>
#include <chrono>

Pathfinding2::Pathfinding2(std::shared_ptr<ArenaModel> arena)
{
    tiles= arena->getTiles();
    cols = arena->getCols();
    rows = arena->getRows();

    _enemies = arena->getEnemyModel()->getEnemies();
    for(auto e : _enemies)
    {
        if(e->getDefeated())
        {
            tiles.at(e->getXPos() + e->getYPos() * cols)->setValue(std::numeric_limits<float>::infinity());
        }
    }
}

void Pathfinding2::findPath(const int &x0, const int &y0, const int &x, const int &y, std::vector<std::shared_ptr<Node> > &path)
{
    //setting the timer1 .
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::priority_queue <std::shared_ptr<Node>,std::vector<std::shared_ptr<Node>>,CheckObject> open;
    std::vector<std::shared_ptr<Node>> checkin;

    std::shared_ptr<Node> dummyNode = std::make_shared<Node>(cols+3,rows+3,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());
    checkin.assign(rows*cols,dummyNode);

    heuristicCost = pow((x-x0),2)+ pow((y-y0),2);

    givenCost = 1 - tiles.at(x0+y0*cols)->getValue();

    finalCost = heuristicWeight*heuristicCost + givenWeight*givenCost;

    std::shared_ptr<Node> rootNode = std::make_shared<Node>(x0,y0,givenCost,finalCost);
    open.push(rootNode);
    checkin.at(x0+y0*cols) = rootNode;

    while(!open.empty()){

        std::shared_ptr<Node> currentNode = open.top();
        open.pop();

        if(currentNode->getX() == x && currentNode->getY() == y){
            // set second timer.
            std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
            std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            qDebug()<< (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000) << "ms";

            createPath(path,currentNode);
            break;
        }

        for (int i = -1; i != 2 ; ++i)
        {
            for(int j = -1; j != 2 ; ++j)
            {
                if( i == 0 && j == 0)
                {
                    continue;
                }
                int successorX = currentNode->getX() + i;
                int successorY = currentNode->getY() + j;

                if(checkBoundary(successorX,successorY)){

                    float heuristicCost =  pow((x-successorX),2)+pow((y-successorY),2);
                    float givenCost = 0;
                    float grayValue = tiles.at(successorX + successorY*cols)->getValue();

                    if(!std::isfinite(grayValue))
                    {
                        continue;
                    }
                    else
                    {
                        givenCost = 1 - grayValue;
                    }

                    if(pow(currentNode->getX() - successorX + currentNode->getY() - successorY, 2.0) == 1)
                    {
                        diagonal_penalty = 1.0;
                    }

                    givenCost = givenCost + currentNode->getGivenCost();
                    float finalCost = heuristicWeight * heuristicCost + givenWeight * givenCost;

                    std::shared_ptr<Node> nodeCheck = checkin.at(successorX + successorY * cols);
                    float nodeGivenCost = nodeCheck->getGivenCost();
                    if(nodeCheck->getX() == successorX && nodeCheck->getY() == successorY){

                        if( !( floorf(givenCost*10)/10 >= floorf(nodeGivenCost*10)/10 ) && std::isfinite(nodeGivenCost)){

                            nodeCheck->setParent(currentNode);
                            nodeCheck->setGivenCost(givenCost);
                            nodeCheck->setFinalCost(finalCost);

                            open.push(nodeCheck);
                        }

                        continue;
                    }

                    std::shared_ptr<Node> successorNode = std::make_shared<Node>(successorX,successorY,givenCost,finalCost);
                    successorNode->setParent(currentNode);

                    open.push(successorNode);
                    checkin.at(successorX +successorY * cols) = successorNode;
                }
            }
        }
    }
}

void Pathfinding2::findAlternatePath(const int &x0, const int &y0, const int &x, const int &y, std::vector<std::shared_ptr<Node> > &path, std::vector<std::shared_ptr<Node> > & trackNodes)
{
    path.clear();
    //setting the timer1 .
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::priority_queue <std::shared_ptr<Node>,std::vector<std::shared_ptr<Node>>,CheckObject> open;
    std::vector<std::shared_ptr<Node>> checkin;

    std::shared_ptr<Node> dummyNode = std::make_shared<Node>(cols+3,rows+3,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());
    checkin.assign(rows*cols,dummyNode);

    heuristicCost = pow((x-x0),2)+ pow((y-y0),2);
    givenCost = 1- tiles.at(x0+y0*cols)->getValue();
    finalCost = heuristicWeight*heuristicCost + givenWeight*givenCost;

    std::shared_ptr<Node> rootNode = std::make_shared<Node>(x0,y0,givenCost,finalCost);
    open.push(rootNode);
    checkin.at(x0+y0*cols) = rootNode;

    while(!open.empty()){

        std::shared_ptr<Node> currentNode = open.top();
        open.pop();

        if(currentNode->getX() == x && currentNode->getY() == y){
            // set second timer.
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
            qDebug()<< (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1000) << "ms";

            createPath(path,currentNode);
            break;
        }

        for (int i = -1; i != 2 ; ++i)
        {
            for(int j = -1; j != 2 ; ++j)
            {
                if( i == 0 && j == 0)
                {
                    continue;
                }
                bool go;
                int successorX = currentNode->getX() + i;
                int successorY = currentNode->getY() + j;

                // checking if successor node is present in trackNode.
                auto compPos = [&successorX,&successorY](std::shared_ptr<Node> a){

                    return  (a->getX() == successorX && a->getY() == successorY);

                };
                auto avoidNode = std::find_if(std::begin(trackNodes),std::end(trackNodes),compPos);
                if( avoidNode != std::end(trackNodes)){
                    go = false;
                }else{
                    go = true;
                }
                if(checkBoundary(successorX,successorY) && go){

                    float heuristicCost = pow((x-successorX),2)+pow((y-successorY),2);
                    float givenCost = 0;
                    float grayValue = tiles.at(successorX + successorY*cols)->getValue();
                    if(!std::isfinite(grayValue))
                    {
                        continue;
                    }
                    else
                    {
                        givenCost = 1 - grayValue;
                    }

                    if(pow(currentNode->getX() - successorX + currentNode->getY() - successorY, 2.0) == 1)
                    {
                        diagonal_penalty = 1.0;
                    }

                    givenCost = givenCost + currentNode->getGivenCost();
                    float finalCost = heuristicWeight * heuristicCost + givenWeight * givenCost;

                    std::shared_ptr<Node> nodeCheck = checkin.at(successorX + successorY * cols);
                    float nodeGivenCost = nodeCheck->getGivenCost();
                    if(nodeCheck->getX() == successorX && nodeCheck->getY() == successorY){

                        if( !( floorf(givenCost*10)/10 >= floorf(nodeGivenCost*10)/10 ) && std::isfinite(nodeGivenCost)){

                            nodeCheck->setParent(currentNode);
                            nodeCheck->setGivenCost(givenCost);
                            nodeCheck->setFinalCost(finalCost);

                            open.push(nodeCheck);
                        }

                        continue;
                    }

                    std::shared_ptr<Node> successorNode = std::make_shared<Node>(successorX,successorY,givenCost,finalCost);
                    successorNode->setParent(currentNode);

                    open.push(successorNode);
                    checkin.at(successorX +successorY * cols) = successorNode;

                }
            }
        }
    }
}

bool Pathfinding2::checkBoundary(int &x, int &y)
{
    if( (x >= 0 && x < cols) && (y >= 0 && y < rows))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Pathfinding2::createPath(std::vector<std::shared_ptr<Node> > &path, std::shared_ptr<Node> finalNode)
{
    std::shared_ptr<Node> s = finalNode;
    path.push_back(s);
    while(s->getParent() != nullptr){
        path.push_back(s->getParent());
        s = s->getParent();
    }
}

float Pathfinding2::getHeuristicWeight() const
{
    return heuristicWeight;
}

void Pathfinding2::setHeuristicWeight(QString value)
{
    heuristicWeight = value.toInt();
}


