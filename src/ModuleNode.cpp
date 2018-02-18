#include "ModuleNode.h"
#include "Parser.h"
#include "StatementNode.h"

ModuleNode::ModuleNode()
    :
    mStatements()
{
    // intentionally left blank
}

ModuleNode::~ModuleNode()
{
    // intentionally left blank
}

void ModuleNode::parse(Parser& parser)
{

}

void ModuleNode::analyze(Analyzer& analyzer)
{
    for (auto& stm : mStatements)
        stm.analyze(analyzer);
}

void ModuleNode::translate(Translator& translator)
{
    for (auto& stm : mStatements)
        stm.translate(translator);
}
