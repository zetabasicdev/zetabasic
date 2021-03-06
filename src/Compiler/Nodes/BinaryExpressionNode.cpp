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
    case TokenTag::Sym_Subtract:
        mOp = Operator::Subtraction;
        break;
    case TokenTag::Sym_Multiply:
        mOp = Operator::Multiplication;
        break;
    case TokenTag::Sym_Divide:
        mOp = Operator::Division;
        break;
    case TokenTag::Key_Mod:
        mOp = Operator::Modulus;
        break;
    case TokenTag::Sym_Equals:
        mOp = Operator::Equals;
        break;
    case TokenTag::Sym_NotEquals:
        mOp = Operator::NotEquals;
        break;
    case TokenTag::Sym_Less:
        mOp = Operator::Less;
        break;
    case TokenTag::Sym_Greater:
        mOp = Operator::Greater;
        break;
    case TokenTag::Sym_LessEquals:
        mOp = Operator::LessEquals;
        break;
    case TokenTag::Sym_GreaterEquals:
        mOp = Operator::GreaterEquals;
        break;
    case TokenTag::Key_Or:
        mOp = Operator::BitwiseOr;
        break;
    case TokenTag::Key_And:
        mOp = Operator::BitwiseAnd;
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

    assert(leftType != Type_Unknown);
    assert(rightType != Type_Unknown);

    // try converting up to real if one side is int and the other is real
    if (leftType == Type_Integer && rightType == Type_Real) {
        mLhs = analyzer.getNodePool().alloc<TypeConversionExpressionNode>(Type_Real, mLhs);
        mLhs->analyze(analyzer);
        leftType = mLhs->getType();
    } else if (leftType == Type_Real && rightType == Type_Integer) {
        mRhs = analyzer.getNodePool().alloc<TypeConversionExpressionNode>(Type_Real, mRhs);
        mRhs->analyze(analyzer);
        rightType = mRhs->getType();
    }

    if (leftType != rightType)
        throw CompileError(CompileErrorId::TypeError, mRange, "Mis-matched Expression Types");

    // check type based on operator
    switch (mOp) {
    case Operator::Addition:
        if (leftType != Type_Integer && leftType != Type_Real && leftType != Type_String)
            throw CompileError(CompileErrorId::TypeError, mOpRange, "Unknown Operation For Types");
        mType = leftType;
        break;
    case Operator::Subtraction:
    case Operator::Multiplication:
    case Operator::Division:
    case Operator::Modulus:
        if (leftType != Type_Integer && leftType != Type_Real)
            throw CompileError(CompileErrorId::TypeError, mOpRange, "Unknown Operation For Types");
        mType = leftType;
        break;
    case Operator::Equals:
    case Operator::NotEquals:
    case Operator::Less:
    case Operator::Greater:
    case Operator::LessEquals:
    case Operator::GreaterEquals:
        if (leftType != Type_Integer && leftType != Type_Real && leftType != Type_String)
            throw CompileError(CompileErrorId::TypeError, mOpRange, "Unknown Operation For Types");
        mType = Type_Boolean;
        break;
    case Operator::BitwiseOr:
    case Operator::BitwiseAnd:
        if (leftType != Type_Boolean && leftType != Type_Integer)
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
