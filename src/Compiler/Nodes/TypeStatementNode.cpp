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
#include "Parser.h"
#include "TypeStatementNode.h"
#include "UserDefinedTypeTable.h"

TypeStatementNode::TypeStatementNode()
    :
    StatementNode(),
    mName(),
    mNameRange(),
    mFields()
{
    // intentionally left blank
}

TypeStatementNode::~TypeStatementNode()
{
    // intentionally left blank
}

void TypeStatementNode::parse(Parser& parser)
{
    assert(parser.getToken().getTag() == TokenTag::Key_Type);
    auto& startRange = parser.getToken().getRange();
    parser.eatToken();

    if (parser.getToken().getId() != TokenId::Name || parser.getToken().getTag() != TokenTag::None)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected TYPE Identifier");
    mName = parser.getToken().getText();
    mNameRange = parser.getToken().getRange();

    // can't be a typed name
    char lastChar = mName[mName.getLength() - 1];
    if (lastChar == '?' || lastChar == '%' || lastChar == '!' || lastChar == '$')
        parser.raiseError(CompileErrorId::SyntaxError, "Type Suffixed Name for TYPE Not Supported");
    parser.eatToken();
    parser.eatEndOfLine();
    
    while (parser.isEndOfLine())
        parser.eatEndOfLine();

    while (parser.getToken().getTag() != TokenTag::Key_End) {
        TypeFieldNode* typeField = parser.getNodePool().alloc<TypeFieldNode>();
        typeField->parse(parser);
        mFields.push(typeField);

        parser.eatEndOfLine();
        while (parser.isEndOfLine())
            parser.eatEndOfLine();
    }
    parser.eatToken();

    if (parser.getToken().getTag() != TokenTag::Key_Type)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected TYPE after END");
    mRange = Range(startRange, parser.getToken().getRange());
    parser.eatToken();

    parser.eatEndOfLine();
}

void TypeStatementNode::analyze(Analyzer& analyzer)
{
    // ensure a UDT of this name does not already exist
    if (analyzer.getUserDefinedTypeTable().findUdt(mName))
        throw CompileError(CompileErrorId::NameError, mNameRange, "TYPE Name Already Exists");

    // can't be empty
    if (mFields.getLength() == 0)
        throw CompileError(CompileErrorId::TypeError, mNameRange, "TYPE Cannot Be Empty");

    auto udt = analyzer.getUserDefinedTypeTable().newUdt();
    udt->name = mName;
    int offset = 0;
    UserDefinedTypeField* last = nullptr;
    for (auto fieldNode : mFields) {
        // ensure no existing field has this name
        for (UserDefinedTypeField* field = udt->fields; field; field = field->next)
            if (field->name == fieldNode.getName())
                throw CompileError(CompileErrorId::NameError, fieldNode.getNameRange(), "Duplicate TYPE Field Name");

        // validate the type
        int size = int(sizeof(int64_t));
        Typename type = fieldNode.getType();
        if (type == Type_Udt) {
            auto subUdt = analyzer.getUserDefinedTypeTable().findUdt(fieldNode.getTypeName());
            if (!subUdt)
                throw CompileError(CompileErrorId::NameError, fieldNode.getTypeRange(), "Bad TYPE Name For Field");
            if (subUdt->id == udt->id)
                throw CompileError(CompileErrorId::NameError, fieldNode.getTypeRange(), "Recursive TYPE Not Allowed");
            type = subUdt->id;
            size = subUdt->size;
        }

        auto* field = analyzer.getUserDefinedTypeTable().newUdtField();
        field->name = fieldNode.getName();
        field->offset = offset;
        field->type = type;
        if (!last)
            udt->fields = field;
        else
            last->next = field;
        last = field;
        ++udt->fieldCount;
        offset += size;
    }
    udt->size = offset;
}

void TypeStatementNode::translate(Translator& translator)
{
    // intentionally left blank
}

TypeStatementNode::TypeFieldNode::TypeFieldNode()
    :
    mNext(nullptr),
    mName(),
    mNameRange(),
    mType(),
    mTypeName(),
    mTypeRange()
{
    // intentionally left blank
}

TypeStatementNode::TypeFieldNode::~TypeFieldNode()
{
    // intentionally left blank
}

void TypeStatementNode::TypeFieldNode::parse(Parser& parser)
{
    if (parser.getToken().getId() != TokenId::Name || parser.getToken().getTag() != TokenTag::None)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Identifier for TYPE Field");
    mName = parser.getToken().getText();
    mNameRange = parser.getToken().getRange();

    // can't be typed field name
    char lastChar = mName[mName.getLength() - 1];
    if (lastChar == '?' || lastChar == '%' || lastChar == '!' || lastChar == '$')
        parser.raiseError(CompileErrorId::SyntaxError, "Type Suffixed Name for TYPE Field Not Supported");
    parser.eatToken();

    if (parser.getToken().getTag() != TokenTag::Key_As)
        parser.raiseError(CompileErrorId::SyntaxError, "Expected AS");
    parser.eatToken();

    mTypeName = parser.getToken().getText();
    if (parser.getToken().getTag() == TokenTag::Key_Boolean)
        mType = Type_Boolean;
    else if (parser.getToken().getTag() == TokenTag::Key_Integer)
        mType = Type_Integer;
    else if (parser.getToken().getTag() == TokenTag::Key_Real)
        mType = Type_Real;
    else if (parser.getToken().getTag() == TokenTag::Key_String)
        mType = Type_String;
    else if (parser.getToken().getId() == TokenId::Name && parser.getToken().getTag() == TokenTag::None)
        mType = Type_Udt;
    else
        parser.raiseError(CompileErrorId::SyntaxError, "Expected Typename");
    mTypeRange = parser.getToken().getRange();
    parser.eatToken();
}
