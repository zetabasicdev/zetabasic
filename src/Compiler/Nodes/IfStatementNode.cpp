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
#include "CompileError.h"
#include "ExpressionNode.h"
#include "IfStatementNode.h"
#include "Opcodes.h"
#include "Parser.h"
#include "Translator.h"

IfStatementNode::IfStatementNode()
    :
    StatementNode(),
    mExpression(nullptr),
    mStatement(nullptr)
{
    // intentionally left blank
}

IfStatementNode::~IfStatementNode()
{
    // intentionally left blank
}

void IfStatementNode::parse(Parser& parser)
{
    assert(parser.getToken().getTag() == TokenTag::Key_If);
    mRange = parser.getToken().getRange();
    parser.eatToken();

    mExpression = ExpressionNode::parseExpression(parser);
    if (!mExpression)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Expression");

    if (parser.getToken().getTag() != TokenTag::Key_Then)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected THEN");
    parser.eatToken();

    mStatement = StatementNode::parseStatement(parser);
    if (!mStatement)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Statement");
}

void IfStatementNode::analyze(Analyzer& analyzer)
{
    mExpression->analyze(analyzer);
    if (mExpression->getType() != Typename::Boolean)
        throw CompileError(CompileErrorId::TypeError, mExpression->getRange(), "IF Expression Must Be BOOLEAN");

    mStatement->analyze(analyzer);
}

void IfStatementNode::translate(Translator& translator)
{
    mExpression->translate(translator);

    auto label = translator.generateLabel();

    translator.jump(Op_jmp_zero, label, mExpression->getResultIndex());
    mStatement->translate(translator);
    translator.placeLabel(label);
    translator.clearTemporaries();
}
