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

#include "Symbol.h"
#include "SymbolTable.h"

SymbolTable::SymbolTable()
    :
    mSymbolPool(64),
    mSymbols()
{
    // intentionally left blank
}

SymbolTable::~SymbolTable()
{
    // intentionally left blank
}

void SymbolTable::reset()
{
    mSymbolPool.reset();
    mSymbols.clear();
}

bool SymbolTable::doesSymbolExist(const StringPiece& name) const
{
    for (auto symbol : mSymbols)
        if (symbol->getName() == name)
            return true;
    return false;
}

Symbol* SymbolTable::getSymbol(const Range& range, const StringPiece& name, Typename type, bool mustExist)
{
    // first determine if symbol by this name already exists
    for (auto symbol : mSymbols)
        if (symbol->getName() == name)
            // simply use this symbol
            return symbol;

    assert(!mustExist);

    // need to instance a new symbol
    if (type == Type_Unknown) {
        // deduce type with any suffix
        type = Type_Integer;
        if (name[name.getLength() - 1] == '$')
            type = Type_String;
    }
    auto symbol = mSymbolPool.alloc((int)mSymbols.size(), range, name, type);
    mSymbols.push_back(symbol);

    return symbol;
}
