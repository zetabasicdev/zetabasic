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
#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "Translator.h"

Compiler::Compiler()
    :
    mTokenPool(256),
    mTokens(),
    mStringPool(),
    mNodePool(),
    mBytecode(256),
    mStringTable(mStringPool),
    mConstantTable(),
    mSymbolTable(),
    mUserDefinedTypeTable()
{
    // intentionally left blank
}

Compiler::~Compiler()
{
    // intentionally left blank
}

Program Compiler::run(ISourceStream& source)
{
    mTokenPool.reset();
    mTokens.reset();
    mStringPool.reset();
    mNodePool.reset();
    mBytecode.reset();
    mStringTable.reset();
    mConstantTable.reset();
    mSymbolTable.reset();
    mUserDefinedTypeTable.reset();

    Lexer lexer(mTokenPool, mTokens, mStringPool, source);
    lexer.run();

    Parser parser(mNodePool, mStringPool, mTokens);
    Node& root = parser.run();

    Analyzer analyzer(mNodePool, mSymbolTable, mUserDefinedTypeTable, root);
    analyzer.run();

    Translator translator(mBytecode, mStringTable, mConstantTable, mSymbolTable, root);
    translator.run();

    return Program(&mBytecode[0], mBytecode.getSize(), mStringTable, mConstantTable);
}
