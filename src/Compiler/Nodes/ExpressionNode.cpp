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

#include "BinaryExpressionNode.h"
#include "BooleanLiteralExpressionNode.h"
#include "ExpressionNode.h"
#include "FunctionCallExpressionNode.h"
#include "IdentifierExpressionNode.h"
#include "IntegerLiteralExpressionNode.h"
#include "Parser.h"
#include "RealLiteralExpressionNode.h"
#include "StringLiteralExpressionNode.h"
#include "UnaryExpressionNode.h"

ExpressionNode::ExpressionNode()
    :
    Node(),
    mType(Typename::Unknown),
    mResultIndex()
{
    // intentionally left blank
}

ExpressionNode::~ExpressionNode()
{
    // intentionally left blank
}

int ExpressionNode::getPrecedence(TokenTag tag)
{
    if (tag == TokenTag::Sym_Add) return 6;
    if (tag == TokenTag::Sym_Equals) return 3;
    if (tag == TokenTag::Key_Or) return 1;
    return 0;
}

ExpressionNode* ExpressionNode::parseExpression(Parser& parser, int precedence)
{
    // first parse potential prefix expression
    ExpressionNode* expr = nullptr;
    if (parser.getToken().getTag() == TokenTag::Sym_OpenParen) {
        parser.eatToken();
        expr = parseExpression(parser);
        if (parser.getToken().getTag() != TokenTag::Sym_CloseParen)
            parser.raiseError(CompileErrorId::SyntaxError, "Expected Closing Parenthesis");
        parser.eatToken();
    } else {
        if (parser.isToken(TokenId::StringPiece))
            expr = parser.getNodePool().alloc<StringLiteralExpressionNode>();
        else if (parser.isToken(TokenId::Integer))
            expr = parser.getNodePool().alloc<IntegerLiteralExpressionNode>();
        else if (parser.isToken(TokenId::Real))
            expr = parser.getNodePool().alloc<RealLiteralExpressionNode>();
        else if (parser.getToken().getTag() == TokenTag::Key_True || parser.getToken().getTag() == TokenTag::Key_False)
            expr = parser.getNodePool().alloc<BooleanLiteralExpressionNode>();
        else if (parser.isToken(TokenId::Name) && parser.getToken().getTag() == TokenTag::None)
            expr = parser.getNodePool().alloc<IdentifierExpressionNode>();
        else if (parser.getToken().getTag() != TokenTag::None && FunctionCallExpressionNode::isBuiltin(parser.getToken().getTag()))
            expr = parser.getNodePool().alloc<FunctionCallExpressionNode>();
        else if (parser.getToken().getTag() == TokenTag::Sym_Add || parser.getToken().getTag() == TokenTag::Sym_Subtract || parser.getToken().getTag() == TokenTag::Key_Not)
            expr = parser.getNodePool().alloc<UnaryExpressionNode>();
        if (expr)
            expr->parse(parser);
    }

    // then search for infix expressions
    while (precedence < getPrecedence(parser.getToken().getTag())) {
        expr = parser.getNodePool().alloc<BinaryExpressionNode>(expr);
        expr->parse(parser);
    }

    return expr;
}
