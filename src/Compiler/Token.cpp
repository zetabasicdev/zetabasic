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

#include "Token.h"

const char* ToString(TokenId id)
{
    switch (id) {
    case TokenId::Unknown: return "unknown";
    case TokenId::EndOfSource: return "end-of-source";
    case TokenId::EndOfLine: return "end-of-line";
    case TokenId::Name: return "name";
    case TokenId::Integer: return "integer";
    case TokenId::StringPiece: return "string";
    case TokenId::Symbol: return "symbol";
    case TokenId::Label: return "label";
    default:
        break;
    }
    return "???";
}

const char* ToString(TokenTag tag)
{
    switch (tag) {
    case TokenTag::None: return "none";
    case TokenTag::Key_End: return "END";
    case TokenTag::Key_For: return "FOR";
    case TokenTag::Key_If: return "IF";
    case TokenTag::Key_Input: return "INPUT";
    case TokenTag::Key_Goto: return "GOTO";
    case TokenTag::Key_Let: return "LET";
    case TokenTag::Key_LeftS: return "LEFT$";
    case TokenTag::Key_Len: return "LEN";
    case TokenTag::Key_Next: return "NEXT";
    case TokenTag::Key_Or: return "OR";
    case TokenTag::Key_Print: return "PRINT";
    case TokenTag::Key_Then: return "THEN";
    case TokenTag::Key_To: return "TO";
    case TokenTag::Sym_Add: return "+";
    case TokenTag::Sym_Equals: return "=";
    case TokenTag::Sym_OpenParen: return "(";
    case TokenTag::Sym_CloseParen: return ")";
    case TokenTag::Sym_Comma: return ",";
    case TokenTag::Sym_Semicolon: return ";";
    default:
        break;
    }
    return "???";
}
