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

#pragma once

#include "Range.h"
#include "StringPiece.h"

enum class TokenId
{
    Unknown,
    EndOfSource,
    EndOfLine,
    Name,
    Integer,
    Real,
    StringPiece,
    Symbol,
    Label
};
const char* ToString(TokenId id);

enum class TokenTag
{
    None,
    Key_And,
    Key_As,
    Key_Boolean,
    Key_Dim,
    Key_End,
    Key_False,
    Key_For,
    Key_Goto,
    Key_If,
    Key_Input,
    Key_Integer,
    Key_Len,
    Key_LeftS,
    Key_Let,
    Key_Mod,
    Key_Next,
    Key_Not,
    Key_Or,
    Key_Print,
    Key_Real,
    Key_String,
    Key_Then,
    Key_To,
    Key_True,
    Key_Type,
    Sym_Add,
    Sym_Subtract,
    Sym_Multiply,
    Sym_Divide,
    Sym_Equals,
    Sym_NotEquals,
    Sym_Less,
    Sym_Greater,
    Sym_LessEquals,
    Sym_GreaterEquals,
    Sym_OpenParen,
    Sym_CloseParen,
    Sym_Comma,
    Sym_Semicolon,
    Sym_Period
};
const char* ToString(TokenTag tag);

class Token
{
public:
    Token(TokenId id, TokenTag tag, const StringPiece& text, const Range& range)
        :
        mId(id),
        mTag(tag),
        mText(text),
        mRange(range)
    {
        // intentionally left blank
    }

    ~Token()
    {
        // intentionally left blank
    }

    TokenId getId() const
    {
        return mId;
    }

    TokenTag getTag() const
    {
        return mTag;
    }

    const StringPiece& getText() const
    {
        return mText;
    }

    const Range& getRange() const
    {
        return mRange;
    }

private:
    TokenId mId;
    TokenTag mTag;
    StringPiece mText;
    Range mRange;
};
