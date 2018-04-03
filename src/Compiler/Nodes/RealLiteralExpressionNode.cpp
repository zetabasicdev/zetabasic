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

#include <cstdlib>

#include "Parser.h"
#include "RealLiteralExpressionNode.h"
#include "Translator.h"

RealLiteralExpressionNode::RealLiteralExpressionNode()
    :
    mValue(0.0)
{
    // intentionally left blank
}

RealLiteralExpressionNode::~RealLiteralExpressionNode()
{
    // intentionally left blank
}

void RealLiteralExpressionNode::parse(Parser& parser)
{
    assert(parser.getToken().getId() == TokenId::Real);
    mValue = strtod(parser.getToken().getText().getText(), nullptr);
    if ((mValue == HUGE_VAL || mValue == -HUGE_VAL) && errno == ERANGE)
        parser.raiseError(CompileErrorId::SyntaxError, "Real Value Out Of Range");
    parser.eatToken();
}

void RealLiteralExpressionNode::analyze(Analyzer& analyzer)
{
    mType = Type_Real;
}

void RealLiteralExpressionNode::translate(Translator& translator)
{
    int64_t value = *(int64_t*)(&mValue);
    mResultIndex = translator.loadConstant(value);
}
