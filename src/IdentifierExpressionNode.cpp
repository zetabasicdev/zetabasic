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

#include <cassert>

#include "Analyzer.h"
#include "IdentifierExpressionNode.h"
#include "Opcodes.h"
#include "Parser.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "Translator.h"

IdentifierExpressionNode::IdentifierExpressionNode()
    :
    mName(),
    mSymbol(nullptr)
{
    // intentionally left blank
}

IdentifierExpressionNode::~IdentifierExpressionNode()
{
    // intentionally left blank
}

void IdentifierExpressionNode::parse(Parser& parser)
{
    assert(parser.getToken().getId() == TokenId::Name);
    assert(parser.getToken().getTag() == TokenTag::None);

    mName = parser.getToken().getText();
    
    parser.eatToken();
}

void IdentifierExpressionNode::analyze(Analyzer& analyzer)
{
    char lastChar = mName.getText()[mName.getLength() - 1];
    if (lastChar == '$')
        mTypename = Typename::String;
    else
        mTypename = Typename::Integer;

    mSymbol = analyzer.getSymbolTable().getSymbol(mRange, mName, mTypename);
}

void IdentifierExpressionNode::translate(Translator& translator)
{
    assert(mSymbol->getLocation() < 256);

    if (mSymbol->getType() == Typename::String) {
        auto code = translator.getBytecode().alloc(2);
        code[0] = Op_load_local_str;
        code[1] = (uint8_t)mSymbol->getLocation();
    } else {
        auto code = translator.getBytecode().alloc(2);
        code[0] = Op_load_local;
        code[1] = (uint8_t)mSymbol->getLocation();
    }
}
