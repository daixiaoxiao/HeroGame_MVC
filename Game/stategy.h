#ifndef STATEGY_H
#define STATEGY_H

#include <memory>
#include <queue>
#include <vector>
#include <algorithm>
#include "arenamodel.h"
#include "node.h"

class CheckObject2{
public:
    bool operator()(std::shared_ptr<Node> a,std::shared_ptr<Node> b){
        return a->getFinalCost() > b->getFinalCost();
    }
};

class Stategy
{
public:
    Stategy(std::shared_ptr<ArenaModel> arena);
    void findCost(const int &x0,const  int &y0,const int &x,const int & y, std::shared_ptr<Node> enemyNode);
    bool checkBoundary(int &x, int &y);

    float getHeuristicWeight() const;
    void setHeuristicWeight(QString value);

private:
    std::vector<std::shared_ptr<Tile>> tiles;
    int cols;
    int rows;

    int diagonal_penalty = sqrt(2.0);
    float heuristicCost;
    float givenCost;
    float finalCost ;
    float heuristicWeight{50};
    float givenWeight{1000000};
};


#endif // STATEGY_H
