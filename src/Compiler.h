#pragma once

#include "NodePool.h"
#include "Program.h"
#include "StringPool.h"
#include "TItemBuffer.h"
#include "Token.h"
#include "TObjectPool.h"
#include "TObjectList.h"

class ISourceStream;

class Compiler
{
public:
    Compiler();
    ~Compiler();

    void run(ISourceStream& source);

private:
    TObjectPool<Token> mTokenPool;
    TObjectList<Token> mTokens;
    StringPool mStringPool;
    NodePool mNodePool;
    TItemBuffer<VmWord> mBytecode;
};
