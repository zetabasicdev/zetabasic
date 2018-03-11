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

#pragma once

#include <cstdint>
#include <unordered_map>
#include "ResultIndex.h"
#include "Node.h"
#include "StringPiece.h"
#include "SymbolTable.h"
#include "TItemBuffer.h"
#include "TNodeList.h"
#include "Typename.h"
#include "VirtualMachine.h"

typedef int Label;

class ConstantTable;
class ExpressionNode;
class StringTable;


class Translator
{
public:
    Translator(TItemBuffer<VmWord>& bytecode,
               StringTable& stringTable,
               ConstantTable& constantTable,
               SymbolTable& symbolTable,
               Node& root);
    ~Translator();

    void run();

    void startCodeBody();
    void endCodeBody();

    ResultIndex loadConstant(int64_t value);
    ResultIndex loadStringConstant(const StringPiece& value);
    ResultIndex loadIdentifier(Symbol* symbol);
    ResultIndex binaryOperator(uint64_t baseOpcode, const ResultIndex& lhs, const ResultIndex& rhs);

    void jump(const StringPiece& name);
    void jump(Label label);
    void jump(uint64_t opcode, Label label, const ResultIndex& result);

    void assign(Symbol* target, const ResultIndex& index);
    ResultIndex ensureLocal(Typename type, const ResultIndex& index);

    void print(Typename type, const ResultIndex& index, bool trailingSemicolon);
    void input(Symbol* target);

    void end();

    ResultIndex builtInFunction(uint64_t opcode, TNodeList<ExpressionNode>& arguments);

    void placeLabel(const StringPiece& name);
    void placeLabel(Label label);

    Label generateLabel();

    void fixupLabels();

    void clearTemporaries();

private:
    TItemBuffer<VmWord>& mCodeBuffer;
    StringTable& mStringTable;
    ConstantTable& mConstantTable;
    SymbolTable& mSymbolTable;
    Node& mRoot;

    int mReserveIndex;

    std::unordered_map<std::string, Label> mNamedLabels;
    std::vector<int> mLabelTargets;

    int mNextTemporary;
    int mMaxTemporaries;

    int getTemporary();
    Label getLabelByName(const StringPiece& name);
    void dumpCode();
};