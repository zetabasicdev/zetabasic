// BSD 3-Clause License
//
// Copyright (c) 2018, Jason Hoyt
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
    mState(State::Start),
    mChar(0),
    mId(TokenId::Unknown),
    mRow(0),
    mCol(0),
    mStartRow(0),
    mStartCol(0),
    mText()
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

    mTokens.push(mTokenPool.alloc(TokenId::EndOfSource, TokenTag::None, String(), Range()));
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
    assert(mId > TokenId::Unknown);

    auto text = mStringPool.alloc(mText.data(), (int)mText.length());
    auto tag = TokenTag::None;

    if (mId == TokenId::Name) {
        // try to match a potential keyword
        static struct {
            const char* keyword;
            TokenTag tag;
        } keywords[] = {
            { "END", TokenTag::Key_End },
            { nullptr, TokenTag::None }
        };
        for (int i = 0; keywords[i].tag != TokenTag::None; ++i) {
            if (text == keywords[i].keyword) {
                tag = keywords[i].tag;
                break;
            }
        }
    }

    auto token = mTokenPool.alloc(mId, tag, text, Range(mStartRow, mStartCol, mRow, mCol));
    mTokens.push(token);

    // reset state
    mState = State::Start;
    mId = TokenId::Unknown;
    mText.clear();
    return false;
}

bool Lexer::runNameState()
{
    if (!(mChar >= 'a' && mChar <= 'z') && !(mChar >= 'A' && mChar <= 'Z') && !(mChar >= '0' && mChar <= '9')) {
        mId = TokenId::Name;
        mState = State::End;
        return false;
    }
    return true;
}
