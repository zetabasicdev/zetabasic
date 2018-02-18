#include "Parser.h"
#include "ModuleNode.h"
#include "NodePool.h"

Parser::Parser(NodePool& nodePool, StringPool& stringPool, const TObjectList<Token>& tokens)
    :
    mNodePool(nodePool),
    mStringPool(stringPool),
    mTokens(tokens),
    mTokenIndex(0)
{
    // intentionally left blank
}

Parser::~Parser()
{
    // intentionally left blank
}

Node& Parser::run()
{
    ModuleNode* module = mNodePool.alloc<ModuleNode>();
    module->parse(*this);
    return *module;
}
