#pragma once

#include "Node.h"
#include "TNodeList.h"

class StatementNode
    :
    public Node
{
public:
    StatementNode();
    virtual ~StatementNode();

    static StatementNode* parseStatement(Parser& parser);

    friend class TNodeList<StatementNode>;
private:
    StatementNode* mNext;
};
