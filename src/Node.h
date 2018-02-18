#pragma once

#include "Range.h"

class Analyzer;
class Parser;
class Translator;

class Node
{
public:
    Node();
    virtual ~Node();

    virtual void parse(Parser& parser) = 0;
    virtual void analyze(Analyzer& analyzer) = 0;
    virtual void translate(Translator& translator) = 0;

protected:
    Range mRange;
};
