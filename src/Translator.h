#pragma once

#include "Node.h"
#include "Program.h"
#include "TItemBuffer.h"

class Translator
{
public:
    Translator(TItemBuffer<VmWord>& bytecode, Node& root);
    ~Translator();

    void run();

private:
    TItemBuffer<VmWord>& mBytecode;
    Node& mRoot;
};
