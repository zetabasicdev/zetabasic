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

#include "GotoStatementNode.h"
#include "Opcodes.h"
#include "Parser.h"
#include "Translator.h"

GotoStatementNode::GotoStatementNode()
    :
    StatementNode(),
    mName()
{
    // intentionally left blank
}

GotoStatementNode::~GotoStatementNode()
{
    // intentionally left blank
}

void GotoStatementNode::parse(Parser& parser)
{
    assert(parser.getToken().getTag() == TokenTag::Key_Goto);
    mRange = parser.getToken().getRange();
    parser.eatToken();

    if (parser.getToken().getId() != TokenId::Name || parser.getToken().getTag() != TokenTag::None)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Label Name");

    mName = parser.getToken().getText();
    parser.eatToken();

    parser.eatEndOfLine();
}

void GotoStatementNode::analyze(Analyzer& analyzer)
{
    // intentionally left blank
}

void GotoStatementNode::translate(Translator& translator)
{
    translator.jump(mName);
}
