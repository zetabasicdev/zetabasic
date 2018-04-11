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
#include "IdentifierNode.h"
#include "Parser.h"
#include "Symbol.h"
#include "SymbolTable.h"
#include "Translator.h"
#include "UserDefinedTypeTable.h"

IdentifierNode::IdentifierNode()
    :
    mName(),
    mPieceType(IdentifierPieceType::TopLevel),
    mSymbol(nullptr),
    mSubNode(nullptr)
{
    // intentionally left blank
}

IdentifierNode::~IdentifierNode()
{
    // intentionally left blank
}

void IdentifierNode::parse(Parser& parser)
{
    if (parser.getToken().getId() != TokenId::Name || parser.getToken().getTag() != TokenTag::None)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Identifier");
    mName = parser.getToken().getText();
    mRange = parser.getToken().getRange();
    parser.eatToken();

    // parse any sub-nodes for TYPE structures
    IdentifierNode* parentNode = this;
    while (parser.getToken().getTag() == TokenTag::Sym_Period) {
        parser.eatToken();

        if (parser.getToken().getId() != TokenId::Name || parser.getToken().getTag() != TokenTag::None)
            parser.raiseError(CompileErrorId::SyntaxError, "Expected Identifier");

        IdentifierNode* subNode = parser.getNodePool().alloc<IdentifierNode>();
        subNode->mName = parser.getToken().getText();
        subNode->mRange = parser.getToken().getRange();
        subNode->mPieceType = IdentifierPieceType::SubField;
        parser.eatToken();

        parentNode->mSubNode = subNode;
        parentNode = subNode;
    }
}

void IdentifierNode::analyze(Analyzer& analyzer)
{
    if (mSubNode) {
        // must be a top-level TYPE structure
        mSymbol = analyzer.getSymbolTable().getSymbol(mRange, mName, Type_Unknown, true);

        IdentifierNode* subNode = mSubNode;
        Typename typeId = mSymbol->getType();
        while (subNode) {
            auto type = analyzer.getUserDefinedTypeTable().findUdt(typeId);
            assert(type);

            auto field = type->fields;
            while (field) {
                if (field->name == subNode->mName)
                    break;
                field = field->next;
            }
            if (!field)
                throw CompileError(CompileErrorId::NameError, subNode->mRange, "Bad Field Name For TYPE");
            subNode->mTypeField = field;

            if (field->type >= Type_Udt)
                typeId = field->type;
            subNode = subNode->mSubNode;
        }
    } else {
        Typename type = Type_Integer;

        switch (mName.getText()[mName.getLength() - 1]) {
        case '?':
            type = Type_Boolean;
            break;
        case '%':
            type = Type_Integer;
            break;
        case '!':
            type = Type_Real;
            break;
        case '$':
            type = Type_String;
            break;
        default:
            break;
        }

        mSymbol = analyzer.getSymbolTable().getSymbol(mRange, mName, type);
    }
}

void IdentifierNode::translate(Translator& translator)
{
    // intentionally left blank
}

Typename IdentifierNode::getFinalType()
{
    if (!mSubNode) {
        if (mPieceType == IdentifierPieceType::TopLevel)
            return mSymbol->getType();
        else
            return mTypeField->type;
    }
    return mSubNode->getFinalType();
}

void IdentifierNode::assign(Translator& translator, const ResultIndex& value)
{
    if (mSubNode) {
        ResultIndex target = ResultIndex(ResultIndexType::Local, mSymbol->getLocation());
        int offset = 0;
        IdentifierNode* node = mSubNode;
        Typename type = Type_Unknown;
        while (node) {
            offset += node->mTypeField->offset;
            type = node->mTypeField->type;
            node = node->mSubNode;
        }
        translator.writeMem(target, value, offset, type);
    } else {
        // simple variable
        translator.assign(mSymbol, value);
    }
}

ResultIndex IdentifierNode::retrieve(Translator& translator)
{
    if (mSubNode) {
        ResultIndex type = ResultIndex(ResultIndexType::Local, mSymbol->getLocation());
        int offset = 0;
        IdentifierNode* node = mSubNode;
        while (node) {
            offset += node->mTypeField->offset;
            node = node->mSubNode;
        }
        return translator.readMem(type, offset);
    }
    
    // simple variable
    return translator.loadIdentifier(mSymbol);
}
