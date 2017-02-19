#include "stategy.h"
#include <iostream>
#include <cassert>
#include <chrono>
#include <QDebug>
#include <utility>
#include <cmath>

Stategy::Stategy(std::shared_ptr<ArenaModel> arena)
{
    tiles= arena->getTiles();
    cols = arena->getCols();
    rows = arena->getRows();
}

void Stategy::findCost(const int &x0, const int &y0, const int &x, const int &y, std::shared_ptr<Node> enemyNode)
{
    //setting the timer1 .
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    std::priority_queue <std::shared_ptr<Node>,std::vector<std::shared_ptr<Node>>,CheckObject2> open;
    std::vector<std::shared_ptr<Node>> checkin;

    std::shared_ptr<Node> dummyNode = std::make_shared<Node>(cols+3,rows+3,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());
    checkin.assign(rows*cols,dummyNode);

    heuristicCost = pow((x-x0),2)+ pow((y-y0),2);
    givenCost =1-tiles.at(x0+y0*cols)->getValue();
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

            float gc = currentNode->getGivenCost();
            float fc = currentNode->getFinalCost();
            enemyNode->setGivenCost(gc);
            enemyNode->setFinalCost(fc);

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
                    float heuristicCost = pow((x-successorX),2)+pow((y-successorY),2);
                    float givenCost = 0;
                    float grayValue = tiles.at(successorX + successorY*cols)->getValue();
                    if(grayValue == std::numeric_limits<float>::infinity())
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

bool Stategy::checkBoundary(int &x, int &y)
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

float Stategy::getHeuristicWeight() const
{
    return heuristicWeight;
}

void Stategy::setHeuristicWeight(QString value)
{
    heuristicWeight = value.toInt();
}
