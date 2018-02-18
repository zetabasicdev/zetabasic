#pragma once

#include "StatementNode.h"

class EndStatementNode
    :
    public StatementNode
{
public:
    EndStatementNode();
    virtual ~EndStatementNode();

    void parse(Parser& parser);
    void analyze(Analyzer& analyzer);
    void translate(Translator& translator);
};
