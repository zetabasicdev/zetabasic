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

#include "Opcodes.h"
#include "Parser.h"
#include "Translator.h"
#include "UnaryExpressionNode.h"

UnaryExpressionNode::UnaryExpressionNode()
    :
    mOp(Operator::Unknown),
    mOpRange(),
    mRhs(nullptr)
{
    // intentionally left blank
}

UnaryExpressionNode::~UnaryExpressionNode()
{
    // intentionally left blank
}

void UnaryExpressionNode::parse(Parser& parser)
{
    switch (parser.getToken().getTag()) {
    case TokenTag::Sym_Add:
        mOp = Operator::Plus;
        break;
    case TokenTag::Sym_Subtract:
        mOp = Operator::Negate;
        break;
    case TokenTag::Key_Not:
        mOp = Operator::BitwiseNot;
        break;
    default:
        assert(false);
        break;
    }
    mOpRange = parser.getToken().getRange();
    parser.eatToken();

    // parse sub-expressions on the right-hand side
    mRhs = ExpressionNode::parseExpression(parser, 7);
    if (!mRhs)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Expression");

    mRange = Range(mOpRange, mRhs->getRange());
}

void UnaryExpressionNode::analyze(Analyzer& analyzer)
{
    mRhs->analyze(analyzer);

    Typename rightType = mRhs->getType();

    assert(rightType != Typename::Unknown);

    // check type based on operator
    switch (mOp) {
    case Operator::Plus:
    case Operator::Negate:
        if (rightType != Typename::Integer && rightType != Typename::Real)
            throw CompileError(CompileErrorId::TypeError, mOpRange, "Unknown Operation For Types");
        mType = rightType;
        break;
    case Operator::BitwiseNot:
        if (rightType != Typename::Boolean && rightType != Typename::Integer)
            throw CompileError(CompileErrorId::TypeError, mOpRange, "Unknown Operation For Types");
        mType = rightType;
        break;
    default:
        assert(false);
        break;
    }
}

void UnaryExpressionNode::translate(Translator& translator)
{
    mRhs->translate(translator);

    // need to base this on operand type, not result type
    auto opType = mRhs->getType();

    // currently unary operators using reals assume reals are always stored in locals/temporaries
    assert(mType != Typename::Real || mRhs->getResultIndex().getType() == ResultIndexType::Local);

    uint64_t opcode = 0;
    switch (mOp) {
    case Operator::Plus:
        // effectively do nothing, pass value through
        mResultIndex = mRhs->getResultIndex();
        return;
    case Operator::Negate:
        if (opType == Typename::Integer)
            opcode = Op_neg_integer0;
        else if (opType == Typename::Real)
            opcode = Op_neg_real1;
        break;
    case Operator::BitwiseNot:
        if (opType == Typename::Boolean || opType == Typename::Integer)
            opcode = Op_not_integer0;
        break;
    default:
        break;
    }

    assert(opcode != 0);
    mResultIndex = translator.unaryOperator(opcode, mRhs->getResultIndex(), mType != Typename::Real);
}
