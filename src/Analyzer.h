#pragma once

class NodePool;
class Node;

class Analyzer
{
public:
    Analyzer(NodePool& nodePool, Node& root);
    ~Analyzer();

    void run();

    NodePool& getNodePool()
    {
        return mNodePool;
    }

private:
    NodePool& mNodePool;
    Node& mRoot;
};
