#include "Translator.h"

Translator::Translator(TItemBuffer<VmWord>& bytecode, Node& root)
    :
    mBytecode(bytecode),
    mRoot(root)
{
    // intentionally left blank
}

Translator::~Translator()
{
    // intentionally left blank
}

void Translator::run()
{
    mRoot.translate(*this);
}
