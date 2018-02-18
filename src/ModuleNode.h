#pragma once

#include "Node.h"
#include "TNodeList.h"

class StatementNode;

class ModuleNode
    :
    public Node
{
public:
    ModuleNode();
    virtual ~ModuleNode();

    void parse(Parser& parser);
    void analyze(Analyzer& analyzer);
    void translate(Translator& translator);

private:
    TNodeList<StatementNode> mStatements;
};
