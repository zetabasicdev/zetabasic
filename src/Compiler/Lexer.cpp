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
    mStringPool(stringPool),
    mTokenPool(tokenPool),
    mTokens(tokens),
    mSource(source),
    mState(State::Start),
    mChar(0),
    mId(TokenId::Unknown),
    mRow(0),
    mCol(0),
    mStartRow(0),
    mStartCol(0),
    mText(),
    mSkip(false)
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
        case State::StringPiece:
            advanceChar = runStringState();
            break;
        case State::Whitespace:
            advanceChar = runWhitespaceState();
            break;
        case State::Symbol:
            advanceChar = runSymbolState();
            break;
        case State::Integer:
            advanceChar = runIntegerState();
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

    if (mTokens.getSize() > 0)
        mTokens.push(mTokenPool.alloc(TokenId::EndOfLine, TokenTag::None, StringPiece(), Range()));
    mTokens.push(mTokenPool.alloc(TokenId::EndOfSource, TokenTag::None, StringPiece(), Range()));

#ifdef DUMP_INTERNALS
    for (int ix = 0; ix < mTokens.getSize(); ++ix) {
        auto& token = mTokens[ix];
        printf("%04d [%02d:%02d] %s (%s) \"%s\"\n",
               ix, token.getRange().getStartRow(), token.getRange().getStartCol(),
               ToString(token.getId()), ToString(token.getTag()),
               (token.getId() != TokenId::EndOfLine) ? token.getText().getText() : "");
    }
    printf("\n");
#endif
}

static bool isSymbolStart(char ch)
{
    const char* chars = "+=(),;";
    for (auto ix = 0; chars[ix]; ++ix)
        if (chars[ix] == ch)
            return true;
    return false;
}

bool Lexer::runStartState()
{
    mStartRow = mRow;
    mStartCol = mCol;
    if ((mChar >= 'a' && mChar <= 'z') || (mChar >= 'A' && mChar <= 'Z')) {
        mState = State::Name;
        return true;
    } else if (mChar == '"') {
        mState = State::StringPiece;
        return true;
    } else if (mChar == ' ' || mChar == '\t') {
        mState = State::Whitespace;
        return true;
    } else if (mChar == '\n') {
        mState = State::End;
        mId = TokenId::EndOfLine;
        return true;
    } else if (isSymbolStart(mChar)) {
        mState = State::Symbol;
        return true;
    } else if (mChar >= '0' && mChar <= '9') {
        mState = State::Integer;
        return true;
    }

    throw CompileError(CompileErrorId::SyntaxError, Range(mRow, mCol), "Syntax Error");
}

bool Lexer::runEndState()
{
    if (!mSkip) {
        assert(mId > TokenId::Unknown);
    
        auto text = mStringPool.alloc(mText.data(), (int)mText.length());
        auto tag = TokenTag::None;

        if (mId == TokenId::Name) {
            // try to match a potential keyword
            static struct
            {
                StringPiece text;
                TokenTag tag;
            } keywords[] = {
                { "END", TokenTag::Key_End },
                { "FALSE", TokenTag::Key_False },
                { "FOR", TokenTag::Key_For },
                { "IF", TokenTag::Key_If },
                { "INPUT", TokenTag::Key_Input },
                { "GOTO", TokenTag::Key_Goto },
                { "LEN", TokenTag::Key_Len },
                { "LEFT$", TokenTag::Key_LeftS },
                { "LET", TokenTag::Key_Let },
                { "NEXT", TokenTag::Key_Next },
                { "OR", TokenTag::Key_Or },
                { "PRINT", TokenTag::Key_Print },
                { "THEN", TokenTag::Key_Then },
                { "TO", TokenTag::Key_To },
                { "TRUE", TokenTag::Key_True },
                { "", TokenTag::None }
            };
            for (int i = 0; keywords[i].tag != TokenTag::None; ++i) {
                if (text == keywords[i].text) {
                    tag = keywords[i].tag;
                    break;
                }
            }
        } else if (mId == TokenId::Symbol) {
            // try to match the symbol
            static struct
            {
                const char* text;
                TokenTag tag;
            } symbols[] = {
                { "+", TokenTag::Sym_Add },
                { "=", TokenTag::Sym_Equals },
                { "(", TokenTag::Sym_OpenParen },
                { ")", TokenTag::Sym_CloseParen },
                { ",", TokenTag::Sym_Comma },
                { ";", TokenTag::Sym_Semicolon },
                { nullptr, TokenTag::None }
            };
            for (int i = 0; symbols[i].tag != TokenTag::None; ++i) {
                if (text == symbols[i].text) {
                    tag = symbols[i].tag;
                    break;
                }
            }
            assert(tag != TokenTag::None);
        }

        auto token = mTokenPool.alloc(mId, tag, text, Range(mStartRow, mStartCol, mRow, mCol));
        mTokens.push(token);
    }

    // reset state
    mSkip = false;
    mState = State::Start;
    mId = TokenId::Unknown;
    mText.clear();
    return false;
}

bool Lexer::runNameState()
{
    if ((mChar >= 'a' && mChar <= 'z') || (mChar >= 'A' && mChar <= 'Z') || (mChar >= '0' && mChar <= '9')) {
        return true;
    } else if (mChar == '$' || mChar == '?' || mChar == '%') {
        mId = TokenId::Name;
        mState = State::End;
        return true;
    } else if (mChar == ':') {
        mId = TokenId::Label;
        mState = State::End;
        return true;
    }

    mId = TokenId::Name;
    mState = State::End;
    return false;
}

bool Lexer::runStringState()
{
    if (mChar == '\n') {
        throw CompileError(CompileErrorId::SyntaxError, Range(mRow, mCol), "Unterminated StringPiece Literal");
    } else if (mChar == '"') {
        mId = TokenId::StringPiece;
        mState = State::End;
    }

    return true;
}

bool Lexer::runWhitespaceState()
{
    if (mChar != ' ' && mChar != '\t') {
        mSkip = true;
        mState = State::End;
        return false;
    }

    return true;
}

bool Lexer::runSymbolState()
{
    mId = TokenId::Symbol;
    mState = State::End;
    return false;
}

bool Lexer::runIntegerState()
{
    if (!(mChar >= '0' && mChar <= '9')) {
        mId = TokenId::Integer;
        mState = State::End;
        return false;
    }

    return true;
}
