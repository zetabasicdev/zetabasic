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
#include "Opcodes.h"
#include "Parser.h"
#include "Translator.h"

BinaryExpressionNode::BinaryExpressionNode(ExpressionNode* lhs)
    :
    mOp(Operator::Unknown),
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
    mRange = parser.getToken().getRange();
    parser.eatToken();

    // parse sub-expressions on the right-hand side
    mRhs = ExpressionNode::parseExpression(parser, precedence);
    if (!mRhs)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Expression");
}

void BinaryExpressionNode::analyze(Analyzer& analyzer)
{
    mLhs->analyze(analyzer);
    mRhs->analyze(analyzer);

    Typename leftType = mLhs->getType();
    Typename rightType = mRhs->getType();

    assert(leftType != Typename::Unknown);
    assert(rightType != Typename::Unknown);

    if (leftType != rightType)
        throw CompileError(CompileErrorId::TypeError, mRange, "Mis-matched Expression Types");

    // todo - fix this...
    if (mOp == Operator::BitwiseOr && leftType != Typename::Integer)
        throw CompileError(CompileErrorId::TypeError, mRange, "Bad Type For Bitwise OR");

    // certain types of operators result in different types
    if (mOp == Operator::Equals)
        mType = Typename::Integer;
    else
        mType = leftType;
}

void BinaryExpressionNode::translate(Translator& translator)
{
    mLhs->translate(translator);
    mRhs->translate(translator);

    // need to base this on operand type, not result type
    auto opType = mLhs->getType();

    switch (mOp) {
    case Operator::Addition:
        if (opType == Typename::Integer)
            mResultIndex = translator.binaryOperator(Op_add_integers0, mLhs->getResultIndex(), mRhs->getResultIndex());
        else
            mResultIndex = translator.binaryOperator(Op_add_strings0, mLhs->getResultIndex(), mRhs->getResultIndex());
        break;
    case Operator::Equals:
        if (opType == Typename::Integer)
            mResultIndex = translator.binaryOperator(Op_eq_integers0, mLhs->getResultIndex(), mRhs->getResultIndex());
        else
            mResultIndex = translator.binaryOperator(Op_eq_strings0, mLhs->getResultIndex(), mRhs->getResultIndex());
        break;
    case Operator::BitwiseOr:
        mResultIndex = translator.binaryOperator(Op_or_integers0, mLhs->getResultIndex(), mRhs->getResultIndex());
        break;
    default:
        break;
    }
}
