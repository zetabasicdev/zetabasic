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
#include "CompileError.h"
#include "ConstantTable.h"
#include "ExpressionNode.h"
#include "ForStatementNode.h"
#include "Opcodes.h"
#include "Parser.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "Translator.h"

ForStatementNode::ForStatementNode()
    :
    StatementNode(),
    mName(),
    mSymbol(nullptr),
    mStartExpression(nullptr),
    mStopExpression(nullptr),
    mNextName(),
    mStatements()
{
    // intentionally left blank
}

ForStatementNode::~ForStatementNode()
{
    // intentionally left blank
}

void ForStatementNode::parse(Parser& parser)
{
    assert(parser.getToken().getTag() == TokenTag::Key_For);
    parser.eatToken();

    if (parser.getToken().getId() != TokenId::Name || parser.getToken().getTag() != TokenTag::None)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Identifier");
    mName = parser.getToken().getText();
    parser.eatToken();

    if (parser.getToken().getTag() != TokenTag::Sym_Equals)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected =");
    parser.eatToken();

    mStartExpression = ExpressionNode::parseExpression(parser);
    if (!mStartExpression)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Expression");

    if (parser.getToken().getTag() != TokenTag::Key_To)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected TO");
    parser.eatToken();

    mStopExpression = ExpressionNode::parseExpression(parser);
    if (!mStopExpression)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Expression");
    parser.eatEndOfLine();

    auto stm = StatementNode::parseStatement(parser);
    while (stm) {
        mStatements.push(stm);
        stm = StatementNode::parseStatement(parser);
    }

    if (parser.getToken().getTag() != TokenTag::Key_Next)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected NEXT");
    parser.eatToken();

    if (parser.getToken().getId() != TokenId::Name || parser.getToken().getTag() != TokenTag::None)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Identifier");
    mNextName = parser.getToken().getText();
    parser.eatToken();

    parser.eatEndOfLine();
}

void ForStatementNode::analyze(Analyzer& analyzer)
{
    if (mName != mNextName)
        throw CompileError(CompileErrorId::NameError, mRange, "Name Mismatch In NEXT");

    mStartExpression->analyze(analyzer);
    mStopExpression->analyze(analyzer);

    // check type of start/stop expressions
    if (mStartExpression->getType() != Typename::Integer)
        throw CompileError(CompileErrorId::TypeError, mStartExpression->getRange(), "Expected Integer Expression");
    if (mStopExpression->getType() != Typename::Integer)
        throw CompileError(CompileErrorId::TypeError, mStopExpression->getRange(), "Expected Integer Expression");
    if (mStartExpression->getType() != mStopExpression->getType())
        throw CompileError(CompileErrorId::TypeError, mRange, "Mismatched Start And Stop Types");

    // identifier expression must match
    if (mName[mName.getLength() - 1] == '$')
        throw CompileError(CompileErrorId::TypeError, mRange, "Expected Integer Identifier Type");

    mSymbol = analyzer.getSymbolTable().getSymbol(mRange, mName, Typename::Integer);

    for (auto& stm : mStatements)
        stm.analyze(analyzer);
}

void ForStatementNode::translate(Translator& translator)
{
    /*
       pseudo-code

       $var = $start
       if $var > $stop then jump [3]
       jump [1]
    [2]
       $var = $var + 1
    [1]
       <inner-statements>
       if $var != $stop then jump [2]
    [3]
    */

    // assign start first
    mStartExpression->translate(translator);
    translator.assign(mSymbol, mStartExpression->getResultIndex());
    translator.clearTemporaries();

    Label jump1 = translator.generateLabel();
    Label jump2 = translator.generateLabel();
    Label jump3 = translator.generateLabel();

    // do initial check if loop should be skipped
    mStopExpression->translate(translator);
    auto result = translator.binaryOperator(Op_gr_integers0,
                                            ResultIndex(ResultIndexType::Local, mSymbol->getLocation()),
                                            mStopExpression->getResultIndex());
    translator.jump(Op_jmp_not_zero, jump3, result);

    translator.jump(jump1);
    translator.placeLabel(jump2);
    translator.clearTemporaries();

    // perform increment of counter
    result = translator.binaryOperator(Op_add_integers0,
                                       ResultIndex(ResultIndexType::Local, mSymbol->getLocation()),
                                       ResultIndex(ResultIndexType::Literal, 1));
    translator.assign(mSymbol, result);

    translator.placeLabel(jump1);
    translator.clearTemporaries();

    // execute loop statements
    for (auto& stm : mStatements)
        stm.translate(translator);

    // perform exit check
    mStopExpression->translate(translator);
    result = translator.binaryOperator(Op_eq_integers0,
                                       ResultIndex(ResultIndexType::Local, mSymbol->getLocation()),
                                       mStopExpression->getResultIndex());
    translator.jump(Op_jmp_zero, jump2, result);

    translator.placeLabel(jump3);
    translator.clearTemporaries();
}
