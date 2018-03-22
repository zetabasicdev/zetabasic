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

#include "TypeConversionExpressionNode.h"
#include "Translator.h"

TypeConversionExpressionNode::TypeConversionExpressionNode(Typename type, ExpressionNode* rhs)
    :
    ExpressionNode(),
    mRhs(rhs)
{
    assert(rhs);
    mType = type;
}

TypeConversionExpressionNode::~TypeConversionExpressionNode()
{
    // intentionally left blank
}

void TypeConversionExpressionNode::parse(Parser& parser)
{
    // intentionally left blank
}

void TypeConversionExpressionNode::analyze(Analyzer& analyzer)
{
    auto subType = mRhs->getType();
    assert((mType == Typename::Integer && subType == Typename::Real) ||
           (mType == Typename::Real && subType == Typename::Integer));
}

void TypeConversionExpressionNode::translate(Translator& translator)
{
    mRhs->translate(translator);
    if (mType == Typename::Real && mRhs->getType() == Typename::Integer)
        mResultIndex = translator.intToReal(mRhs->getResultIndex());
    else if (mType == Typename::Integer && mRhs->getType() == Typename::Real)
        mResultIndex = translator.realToInt(mRhs->getResultIndex());
}
