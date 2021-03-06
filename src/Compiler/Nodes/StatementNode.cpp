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

#include "AssignmentStatementNode.h"
#include "DimStatementNode.h"
#include "EndStatementNode.h"
#include "ForStatementNode.h"
#include "IfStatementNode.h"
#include "InputStatementNode.h"
#include "GotoStatementNode.h"
#include "LabelStatementNode.h"
#include "Parser.h"
#include "PrintStatementNode.h"
#include "StatementNode.h"
#include "TypeStatementNode.h"

StatementNode::StatementNode()
    :
    Node()
{
    // intentionally left blank
}

StatementNode::~StatementNode()
{
    // intentionally left blank
}

StatementNode* StatementNode::parseStatement(Parser& parser, StatementType type)
{
    assert(type >= StatementType::Simple && type <= StatementType::Module);

    while (parser.getToken().getId() == TokenId::EndOfLine)
        parser.eatToken();

    auto& token = parser.getToken();
    StatementNode* node = nullptr;
    if (token.getId() == TokenId::Name) {
        switch (token.getTag()) {
        case TokenTag::Key_Dim:
            node = parser.getNodePool().alloc<DimStatementNode>();
            break;
        case TokenTag::Key_End:
            node = parser.getNodePool().alloc<EndStatementNode>();
            break;
        case TokenTag::Key_For:
            node = parser.getNodePool().alloc<ForStatementNode>();
            break;
        case TokenTag::Key_If:
            node = parser.getNodePool().alloc<IfStatementNode>();
            break;
        case TokenTag::Key_Input:
            node = parser.getNodePool().alloc<InputStatementNode>();
            break;
        case TokenTag::Key_Goto:
            node = parser.getNodePool().alloc<GotoStatementNode>();
            break;
        case TokenTag::Key_Let:
        case TokenTag::None:
            // assume variable assignment
            node = parser.getNodePool().alloc<AssignmentStatementNode>();
            break;
        case TokenTag::Key_Print:
            node = parser.getNodePool().alloc<PrintStatementNode>();
            break;
        case TokenTag::Key_Type:
            if (type == StatementType::Module)
                node = parser.getNodePool().alloc<TypeStatementNode>();
            break;
        default:
            break;
        }
    } else if (token.getId() == TokenId::Label && type != StatementType::Simple) {
        node = parser.getNodePool().alloc<LabelStatementNode>();
    }

    if (node)
        node->parse(parser);
    return node;
}
