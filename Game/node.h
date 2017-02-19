#ifndef NODE_H
#define NODE_H
#include <memory>

class Node
{
public:
    Node(const int &i, const int &j, const float &gc, const float &fc):
        x{i},y{j},givenCost{gc},finalCost{fc}
    {

    }

    inline int getX() const{ return x; }

    inline void setX(int &value){ x = value;}

    inline int getY() const{ return y; }

    inline void setY(int &value){ y = value; }

    inline float getFinalCost() const{ return finalCost; }

    inline void setFinalCost(const float &value){ finalCost = value; }

    inline float getGivenCost() const{ return givenCost; }

    inline void setGivenCost(const float &value){ givenCost = value;}

    inline std::shared_ptr<Node> getParent() const{ return parent;}

    inline void setParent(const std::shared_ptr<Node> &value){ parent = value; }

    inline int getFlag() const{ return flag;}

    inline void setFlag(const int &value){ flag = value;}

private:
    int x;
    int y;
    float givenCost;
    float finalCost;
    int flag = 0;

    std::shared_ptr<Node> parent = nullptr;
};

#endif // NODE_H
