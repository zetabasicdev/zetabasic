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
    // setup assignment first
    mStartExpression->translate(translator);
    assert(mSymbol->getLocation() < 256);

    auto code = translator.getCodeBuffer().alloc(2);
    code[0] = Op_store_local;
    code[1] = (uint8_t)mSymbol->getLocation();

    // need to jump past loop increment
    int jump1;
    code = translator.getCodeBuffer().alloc(2, jump1);
    code[0] = Op_jmp;
    
    // mark position for jump
    int target = translator.getCodeBuffer().getSize();

    // check if counter is past end

    // perform modification of counter
    code = translator.getCodeBuffer().alloc(2);
    code[0] = Op_load_local;
    code[1] = (uint8_t)mSymbol->getLocation();

    code = translator.getCodeBuffer().alloc(2);
    code[0] = Op_load_const;
    code[1] = translator.getConstantTable().addInteger(1);

    *translator.getCodeBuffer().alloc(1) = Op_add_int;

    code = translator.getCodeBuffer().alloc(2);
    code[0] = Op_store_local;
    code[1] = (uint8_t)mSymbol->getLocation();

    int jump1Target = translator.getCodeBuffer().getSize();
    translator.getCodeBuffer()[jump1 + 1] = jump1Target;

    for (auto& stm : mStatements)
        stm.translate(translator);

    // and do comparison
    code = translator.getCodeBuffer().alloc(2);
    code[0] = Op_load_local;
    code[1] = (uint8_t)mSymbol->getLocation();

    mStopExpression->translate(translator);

    code = translator.getCodeBuffer().alloc(2);
    code[0] = Op_jmp_lt;
    code[1] = target;
}
