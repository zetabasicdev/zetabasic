#include "Analyzer.h"
#include "Node.h"

Analyzer::Analyzer(NodePool& nodePool, Node& root)
    :
    mNodePool(nodePool),
    mRoot(root)
{
    // intentionally left blank
}

Analyzer::~Analyzer()
{
    // intentionally left blank
}

void Analyzer::run()
{
    mRoot.analyze(*this);
}
