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
#include "ExpressionNode.h"
#include "InputStatementNode.h"
#include "Opcodes.h"
#include "Parser.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "Translator.h"

InputStatementNode::InputStatementNode()
    :
    mPromptExpression(nullptr),
    mIdentifier()
{
    // intentionally left blank
}

InputStatementNode::~InputStatementNode()
{
    // intentionally left blank
}

void InputStatementNode::parse(Parser& parser)
{
    assert(parser.getToken().getTag() == TokenTag::Key_Input);
    mRange = parser.getToken().getRange();
    parser.eatToken();

    mPromptExpression = ExpressionNode::parseExpression(parser);
    if (!mPromptExpression)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Expression");

    if (parser.getToken().getTag() != TokenTag::Sym_Semicolon)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Semicolon");
    parser.eatToken();

    mIdentifier.parse(parser);

    parser.eatEndOfLine();
}

void InputStatementNode::analyze(Analyzer& analyzer)
{
    mPromptExpression->analyze(analyzer);
    mIdentifier.analyze(analyzer);
}

void InputStatementNode::translate(Translator& translator)
{
    mPromptExpression->translate(translator);

    translator.print(mPromptExpression->getType(), mPromptExpression->getResultIndex(), true);
    translator.input(mIdentifier.getSymbol());
}
