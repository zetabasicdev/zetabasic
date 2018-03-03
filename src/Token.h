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
#include "String.h"

enum class TokenId
{
    Unknown,
    EndOfSource,
    EndOfLine,
    Name,
    Integer,
    String,
    Symbol,
    Label
};
const char* ToString(TokenId id);

enum class TokenTag
{
    None,
    Key_End,
    Key_For,
    Key_Goto,
    Key_If,
    Key_Len,
    Key_LeftS,
    Key_Let,
    Key_Next,
    Key_Or,
    Key_Print,
    Key_Then,
    Key_To,
    Sym_Add,
    Sym_Equals,
    Sym_OpenParen,
    Sym_CloseParen,
    Sym_Comma,
    Sym_Semicolon
};
const char* ToString(TokenTag tag);

class Token
{
public:
    Token(TokenId id, TokenTag tag, const String& text, const Range& range)
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

    const String& getText() const
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
    String mText;
    Range mRange;
};
