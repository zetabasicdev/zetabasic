#pragma once

#include "TObjectList.h"
#include "TObjectPool.h"
#include "Token.h"

class ISourceStream;
class StringPool;

class Lexer
{
public:
    Lexer(TObjectPool<Token>& tokenPool, TObjectList<Token>& tokens, StringPool& stringPool, ISourceStream& source);
    ~Lexer();

    void run();

private:
    StringPool& mStringPool;
    TObjectPool<Token>& mTokenPool;
    TObjectList<Token>& mTokens;
    ISourceStream& mSource;

    enum class State {
        Start,
        End,
        Name
    };
    State mState;

    char mChar;
    TokenId mId;
    int mRow;
    int mCol;
    int mStartRow;
    int mStartCol;
    std::string mText;

    bool runStartState();
    bool runEndState();
    bool runNameState();
};
