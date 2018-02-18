#include "CompileError.h"
#include "ISourceStream.h"
#include "Lexer.h"
#include "StringPool.h"

Lexer::Lexer(TObjectPool<Token>& tokenPool, TObjectList<Token>& tokens, StringPool& stringPool, ISourceStream& source)
    :
    mTokenPool(tokenPool),
    mTokens(tokens),
    mStringPool(stringPool),
    mSource(source),
    mState(State::Start)
{
    // intentionally left blank
}

Lexer::~Lexer()
{
    // intentionally left blank
}

void Lexer::run()
{
    mRow = mSource.getRow();
    mCol = mSource.getCol();
    mChar = mSource.read();
    while (!(mChar == 0 && mState == State::Start)) {
        bool advanceChar = false;
        switch (mState) {
        case State::Start:
            advanceChar = runStartState();
            break;
        case State::End:
            advanceChar = runEndState();
            break;
        case State::Name:
            advanceChar = runNameState();
            break;
        default:
            break;
        }

        if (advanceChar) {
            mText.push_back(mChar);
            mRow = mSource.getRow();
            mCol = mSource.getCol();
            mChar = mSource.read();
        }
    }
}

bool Lexer::runStartState()
{
    mStartRow = mRow;
    mStartCol = mCol;
    if ((mChar >= 'a' && mChar <= 'z') || (mChar >= 'A' && mChar <= 'Z')) {
        mState = State::Name;
        return true;
    }

    throw CompileError(CompileErrorId::SyntaxError, Range(mRow, mCol), "Syntax Error");
}

bool Lexer::runEndState()
{
    auto text = mStringPool.alloc(mText.data(), (int)mText.length());
    auto tag = TokenTag::None;

    if (mId == TokenId::Name) {
        // try to match a potential keyword
        static struct {
            const char* keyword;
            TokenTag tag;
        } keywords[] = {
            { nullptr, TokenTag::None }
        };
        for (int i = 0; keywords[i].tag != TokenTag::None; ++i) {
            if (text == keywords[i].keyword) {
                tag = keywords[i].tag;
                break;
            }
        }
    }

    auto token = mTokenPool.alloc(mId, TokenTag::None, text, Range(mStartRow, mStartCol, mRow, mCol));
    mTokens.push(token);

    // reset state
    mState = State::Start;
    mText.clear();
    return false;
}

bool Lexer::runNameState()
{
    if (!(mChar >= 'a' && mChar <= 'z') && !(mChar >= 'A' && mChar <= 'Z') && !(mChar >= '0' && mChar <= '9')) {
        mState = State::End;
        return false;
    }
    return true;
}
