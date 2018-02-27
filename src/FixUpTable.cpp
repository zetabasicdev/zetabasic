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

#include "CompileError.h"
#include "FixUpTable.h"

FixUpTable::FixUpTable(TItemBuffer<uint8_t>& bytecode, CodePositionTable& codePositionTable)
    :
    mBytecode(bytecode),
    mCodePositionTable(codePositionTable),
    mFixUpPool(32),
    mFixUps()
{
    // intentionally left blank
}

FixUpTable::~FixUpTable()
{
    // intentionally left blank
}

void FixUpTable::reset()
{
    mFixUpPool.reset();
    mFixUps.reset();
}

void FixUpTable::addFixUp(int index, CodePositionType type, const String& name, const Range& range)
{
    mFixUps.push(mFixUpPool.alloc(index, type, name, range));
}

void FixUpTable::doFixups()
{
    for (auto& fixup : mFixUps) {
        auto index = mCodePositionTable.getPosition(fixup.type, fixup.name);
        if (index == -1)
            throw CompileError(CompileErrorId::NameError, fixup.range, "Invalid Name");
        mBytecode[fixup.index] = (index >> 8) & 0xff;
        mBytecode[fixup.index + 1] = index & 0xff;
    }
}
