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
#include "AssignmentStatementNode.h"
#include "ExpressionNode.h"
#include "Opcodes.h"
#include "Parser.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "Translator.h"

AssignmentStatementNode::AssignmentStatementNode()
    :
    mIdentifier()
{
    // intentionally left blank
}

AssignmentStatementNode::~AssignmentStatementNode()
{
    // intentionally left blank
}

void AssignmentStatementNode::parse(Parser& parser)
{
    assert(parser.getToken().getTag() == TokenTag::Key_Let);
    parser.eatToken();

    mIdentifier.parse(parser);

    if (parser.getToken().getTag() != TokenTag::Sym_Equals)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected =");
    parser.eatToken();

    mValue = ExpressionNode::parseExpression(parser);
    if (!mValue)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Expression");

    parser.eatEndOfLine();
}

void AssignmentStatementNode::analyze(Analyzer& analyzer)
{
    mIdentifier.analyze(analyzer);
    mValue->analyze(analyzer);

    if (mIdentifier.getSymbol()->getType() != mValue->getType())
        throw CompileError(CompileErrorId::TypeError, mRange, "Incompatible Types For Assignment");
}

void AssignmentStatementNode::translate(Translator& translator)
{
    mValue->translate(translator);
    translator.assign(mIdentifier.getSymbol(), mValue->getResultIndex());
    translator.clearTemporaries();
}
