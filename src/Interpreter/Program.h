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

#include <cassert>
#include <cstdint>
#include <vector>
#include "Bytecode.h"
#include "ConstantTable.h"
#include "StringTable.h"

class Program
{
public:
    Program(const BytecodeWord* bytecode, int length, const StringTable& stringTable, const ConstantTable& constantTable)
        :
        mBytecode(bytecode),
        mLength(length),
        mStringTable(stringTable),
        mConstantTable(constantTable)
    {
        assert(mBytecode);
        assert(length > 0);
    }

    ~Program()
    {
        // intentionally left blank
    }

    const BytecodeWord* getBytecode() const
    {
        return mBytecode;
    }

    int getLength() const
    {
        return mLength;
    }

    const StringPiece& getString(int index) const
    {
        return mStringTable.getString(index);
    }

    int64_t getIntegerConstant(int index) const
    {
        return mConstantTable.getIntegerConstant(index);
    }

    void dumpStrings() const
    {
        mStringTable.dump();
    }

private:
    const BytecodeWord* mBytecode;
    int mLength;

    const StringTable& mStringTable;
    const ConstantTable& mConstantTable;
};
