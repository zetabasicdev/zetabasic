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

#include "Analyzer.h"
#include "BinaryExpressionNode.h"
#include "Parser.h"
#include "Translator.h"
#include "TypeConversionExpressionNode.h"

BinaryExpressionNode::BinaryExpressionNode(ExpressionNode* lhs)
    :
    mOp(Operator::Unknown),
    mOpRange(),
    mLhs(lhs),
    mRhs(nullptr)
{
    // intentionally left blank
}

BinaryExpressionNode::~BinaryExpressionNode()
{
    // intentionally left blank
}

void BinaryExpressionNode::parse(Parser& parser)
{
    int precedence = ExpressionNode::getPrecedence(parser.getToken().getTag());
    switch (parser.getToken().getTag()) {
    case TokenTag::Sym_Add:
        mOp = Operator::Addition;
        break;
    case TokenTag::Sym_Equals:
        mOp = Operator::Equals;
        break;
    case TokenTag::Key_Or:
        mOp = Operator::BitwiseOr;
        break;
    default:
        assert(false);
        break;
    }
    mOpRange = parser.getToken().getRange();
    parser.eatToken();

    // parse sub-expressions on the right-hand side
    mRhs = ExpressionNode::parseExpression(parser, precedence);
    if (!mRhs)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Expression");

    mRange = Range(mLhs->getRange(), mRhs->getRange());
}

void BinaryExpressionNode::analyze(Analyzer& analyzer)
{
    mLhs->analyze(analyzer);
    mRhs->analyze(analyzer);

    Typename leftType = mLhs->getType();
    Typename rightType = mRhs->getType();

    assert(leftType != Typename::Unknown);
    assert(rightType != Typename::Unknown);

    // try converting up to real if one side is int and the other is real
    if (leftType == Typename::Integer && rightType == Typename::Real) {
        mLhs = analyzer.getNodePool().alloc<TypeConversionExpressionNode>(Typename::Real, mLhs);
        mLhs->analyze(analyzer);
        leftType = mLhs->getType();
    } else if (leftType == Typename::Real && rightType == Typename::Integer) {
        mRhs = analyzer.getNodePool().alloc<TypeConversionExpressionNode>(Typename::Real, mRhs);
        mRhs->analyze(analyzer);
        rightType = mRhs->getType();
    }

    if (leftType != rightType)
        throw CompileError(CompileErrorId::TypeError, mRange, "Mis-matched Expression Types");

    // check type based on operator
    switch (mOp) {
    case Operator::Addition:
        if (leftType != Typename::Integer && leftType != Typename::Real && leftType != Typename::String)
            throw CompileError(CompileErrorId::TypeError, mOpRange, "Unknown Operation For Types");
        mType = leftType;
        break;
    case Operator::Equals:
        if (leftType != Typename::Integer && leftType != Typename::Real && leftType != Typename::String)
            throw CompileError(CompileErrorId::TypeError, mOpRange, "Unknown Operation For Types");
        mType = Typename::Boolean;
        break;
    case Operator::BitwiseOr:
        if (leftType != Typename::Boolean && leftType != Typename::Integer)
            throw CompileError(CompileErrorId::TypeError, mOpRange, "Unknown Operation For Types");
        mType = leftType;
        break;
    default:
        assert(false);
        break;
    }
}

void BinaryExpressionNode::translate(Translator& translator)
{
    mLhs->translate(translator);
    mRhs->translate(translator);

    // need to base this on operand type, not result type
    auto opType = mLhs->getType();

    mResultIndex = translator.binaryOperator(mOp, opType, mLhs->getResultIndex(), mRhs->getResultIndex());
}
