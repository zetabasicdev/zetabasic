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
#include "DimStatementNode.h"
#include "Parser.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "UserDefinedTypeTable.h"

DimNode::DimNode()
    :
    mNext(nullptr),
    mRange(),
    mName(),
    mType(Type_Unknown),
    mTypeName(),
    mTypeRange(),
    mSymbol(nullptr)
{
    // intentionally left blank
}

DimNode::~DimNode()
{
    // intentionally left blank
}

void DimNode::parse(Parser& parser)
{
    if (parser.getToken().getId() != TokenId::Name || parser.getToken().getTag() != TokenTag::None)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Identifier");
    mName = parser.getToken().getText();
    mRange = parser.getToken().getRange();
    parser.eatToken();

    auto specifiedType = Type_Unknown;
    if (parser.getToken().getTag() == TokenTag::Key_As) {
        parser.eatToken();
        mTypeName = parser.getToken().getText();
        mTypeRange = parser.getToken().getRange();
        switch (parser.getToken().getTag()) {
        case TokenTag::Key_Boolean:
            specifiedType = Type_Boolean;
            break;
        case TokenTag::Key_Integer:
            specifiedType = Type_Integer;
            break;
        case TokenTag::Key_Real:
            specifiedType = Type_Real;
            break;
        case TokenTag::Key_String:
            specifiedType = Type_String;
            break;
        default:
            if (parser.getToken().getId() == TokenId::Name && parser.getToken().getTag() == TokenTag::None) {
                // assume a UDT for now
                specifiedType = Type_Udt;
            } else {
                parser.raiseError(CompileErrorId::SyntaxError, "Expected Typename");
            }
            break;
        }
        parser.eatToken();
    }

    auto nameType = Type_Unknown;
    switch (mName.getText()[mName.getLength() - 1]) {
    case '?':
        nameType = Type_Boolean;
        break;
    case '%':
        nameType = Type_Integer;
        break;
    case '!':
        nameType = Type_Real;
        break;
    case '$':
        nameType = Type_String;
        break;
    default:
        break;
    }

    // can't have a typed identifier name with a specified type
    if (nameType != Type_Unknown && specifiedType != Type_Unknown)
        parser.raiseError(CompileErrorId::SyntaxError, "Suffixed Identifier Already Has Type");
    else if (nameType == Type_Unknown && specifiedType == Type_Unknown)
        nameType = Type_Integer;

    if (nameType == Type_Unknown)
        mType = specifiedType;
    else
        mType = nameType;
}

void DimNode::analyze(Analyzer& analyzer)
{
    // ensure a local is not already defined with this name
    if (analyzer.getSymbolTable().doesSymbolExist(mName))
        throw CompileError(CompileErrorId::NameError, mRange, "Identifier Already Defined");

    // check for UDT
    if (mType == Type_Udt) {
        auto* udt = analyzer.getUserDefinedTypeTable().findUdt(mTypeName);
        if (!udt)
            throw CompileError(CompileErrorId::NameError, mTypeRange, "Expected Typename");
        mType = udt->id;
    }

    mSymbol = analyzer.getSymbolTable().getSymbol(mRange, mName, mType);
}

DimStatementNode::DimStatementNode()
    :
    StatementNode(),
    mNodes()
{
    // intentionally left blank
}

DimStatementNode::~DimStatementNode()
{
    // intentionally left blank
}

void DimStatementNode::parse(Parser& parser)
{
    assert(parser.getToken().getTag() == TokenTag::Key_Dim);
    auto& startRange = parser.getToken().getRange();
    parser.eatToken();

    bool first = true;
    do {
        if (!first)
            parser.eatToken();
        DimNode* node = parser.getNodePool().alloc<DimNode>();
        node->parse(parser);
        mNodes.push(node);
        first = false;
    } while (parser.getToken().getTag() == TokenTag::Sym_Comma);

    parser.eatEndOfLine();
}

void DimStatementNode::analyze(Analyzer& analyzer)
{
    for (auto& node : mNodes)
        node.analyze(analyzer);
}

void DimStatementNode::translate(Translator& translator)
{
    // intentionally left blank
}
