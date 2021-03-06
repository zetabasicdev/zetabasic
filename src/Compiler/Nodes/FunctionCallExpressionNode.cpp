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
#include "FunctionCallExpressionNode.h"
#include "Parser.h"
#include "Translator.h"

static struct {
    TokenTag tag;
    StringPiece name;
    StringPiece arguments;
    Typename returnType;
} builtinFunctions[] = {
    { TokenTag::Key_Len, "LEN", "S", Type_Integer },
    { TokenTag::Key_LeftS, "LEFT$", "SI", Type_String },
    { TokenTag::None, "", "", Type_Unknown }
};

FunctionCallExpressionNode::FunctionCallExpressionNode()
    :
    mName(),
    mArguments()
{
    // intentionally left blank
}

FunctionCallExpressionNode::~FunctionCallExpressionNode()
{
    // intentionally left blank
}

bool FunctionCallExpressionNode::isBuiltin(TokenTag tag)
{
    for (int ix = 0; builtinFunctions[ix].tag != TokenTag::None; ++ix)
        if (builtinFunctions[ix].tag == tag)
            return true;
    return false;
}

void FunctionCallExpressionNode::parse(Parser& parser)
{
    assert(parser.getToken().getId() == TokenId::Name);
    mName = parser.getToken().getText();
    mRange = parser.getToken().getRange();
    parser.eatToken();

    if (parser.getToken().getTag() == TokenTag::Sym_OpenParen) {
        parser.eatToken();

        while (true) {
            auto expression = ExpressionNode::parseExpression(parser);
            if (!expression)
                parser.raiseError(CompileErrorId::SyntaxError, "Expected Expression");
            mArguments.push(expression);

            if (parser.getToken().getTag() != TokenTag::Sym_Comma)
                break;
            parser.eatToken();
        }

        if (parser.getToken().getTag() != TokenTag::Sym_CloseParen)
            parser.raiseError(CompileErrorId::SyntaxError, "Expected Closing Parenthesis");
        parser.eatToken();
    }
}

void FunctionCallExpressionNode::analyze(Analyzer& analyzer)
{
    for (int ix = 0; builtinFunctions[ix].tag != TokenTag::None; ++ix) {
        if (builtinFunctions[ix].name == mName) {
            // ensure arguments match
            int argumentCount = builtinFunctions[ix].arguments.getLength();
            if (argumentCount != mArguments.getLength())
                throw CompileError(CompileErrorId::NameError, mRange, "Argument Count Mismatch");

            int i = 0;
            for (auto& arg : mArguments) {
                arg.analyze(analyzer);

                switch (builtinFunctions[ix].arguments[i]) {
                case 'S':
                    if (arg.getType() != Type_String)
                        throw CompileError(CompileErrorId::TypeError, arg.getRange(), "Expected StringPiece Expression");
                    break;
                case 'I':
                    if (arg.getType() != Type_Integer)
                        throw CompileError(CompileErrorId::TypeError, arg.getRange(), "Expected Integer Expression");
                    break;
                default:
                    assert(false);
                    break;
                }
                ++i;
            }

            mType = builtinFunctions[ix].returnType;
            return;
        }
    }

    assert(false);
}

void FunctionCallExpressionNode::translate(Translator& translator)
{
    for (auto& arg : mArguments)
        arg.translate(translator);

    uint64_t op = 0;
    for (int ix = 0; builtinFunctions[ix].tag != TokenTag::None && op == 0; ++ix) {
        if (builtinFunctions[ix].name == mName) {
            mResultIndex = translator.builtInFunction(builtinFunctions[ix].name, mArguments);
            return;
        }
    }
    assert(false);
}
