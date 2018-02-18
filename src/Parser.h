#pragma once

#include "NodePool.h"
#include "StringPool.h"
#include "TObjectList.h"
#include "Token.h"

class Node;

class Parser
{
public:
    Parser(NodePool& nodePool, StringPool& stringPool, const TObjectList<Token>& tokens);
    ~Parser();

    Node& run();

    NodePool& getNodePool()
    {
        return mNodePool;
    }

    StringPool& getStringPool()
    {
        return mStringPool;
    }

private:
    NodePool& mNodePool;
    StringPool& mStringPool;

    const TObjectList<Token>& mTokens;
    int mTokenIndex;
};
