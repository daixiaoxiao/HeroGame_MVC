#ifndef PATHFINDING2_H
#define PATHFINDING2_H

#include <memory>
#include <queue>
#include <vector>
#include <algorithm>
#include "arenamodel.h"
#include "node.h"

class CheckObject{
public:
    bool operator()(std::shared_ptr<Node> a,std::shared_ptr<Node> b){
        return a->getFinalCost() > b->getFinalCost();
    }
};

class Pathfinding2
{
public:
    Pathfinding2(std::shared_ptr<ArenaModel> arena);

    void findPath(const int &x0,const int &y0,const int &x,const int &y, std::vector<std::shared_ptr<Node>> &path);

    void findAlternatePath(const int &x0, const int &y0, const int &x, const int &y, std::vector<std::shared_ptr<Node>> &path, std::vector<std::shared_ptr<Node>> &trackNodes);

    bool checkBoundary(int &x, int &y);
    void createPath(std::vector<std::shared_ptr<Node>> &path, std::shared_ptr<Node> finalNode);

    float getHeuristicWeight() const;
    void setHeuristicWeight(QString value);

private:
    std::vector<std::shared_ptr<Tile>> tiles;
    std::vector<std::shared_ptr<Enemy>> _enemies;

    int cols;
    int rows;

    int diagonal_penalty = sqrt(2.0);
    float heuristicCost;
    float givenCost;
    float finalCost;
    float heuristicWeight{50};
    float givenWeight{1000000};
};

#endif // PATHFINDING2_H
