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

#include <stdlib.h>

#include "IntegerLiteralExpressionNode.h"
#include "Parser.h"
#include "Translator.h"

IntegerLiteralExpressionNode::IntegerLiteralExpressionNode()
    :
    mValue(0)
{
    // intentionally left blank
}

IntegerLiteralExpressionNode::~IntegerLiteralExpressionNode()
{
    // intentionally left blank
}

void IntegerLiteralExpressionNode::parse(Parser& parser)
{
    assert(parser.getToken().getId() == TokenId::Integer);

    // todo : verify value of integer is not out-of-bounds
    mValue = strtoll(parser.getToken().getText().getText(), nullptr, 10);
    mRange = parser.getToken().getRange();

    parser.eatToken();
}

void IntegerLiteralExpressionNode::analyze(Analyzer& analyzer)
{
    mType = Typename::Integer;
}

void IntegerLiteralExpressionNode::translate(Translator& translator)
{
    mResultIndex = translator.loadConstant(mValue);
}
