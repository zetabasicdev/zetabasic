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
#include <vector>

enum
{
    MemoryType_Unknown,
    MemoryType_SmallString,
    MemoryType_String,
    MemoryType_Udt
};

// Class that manages all dynamic memory for the interpreter, including strings
// and user-defined types.
class MemoryManager
{
public:
    MemoryManager();
    ~MemoryManager();

    int64_t newString(const char* text, int length);
    void delString(int64_t desc);

    void getString(int64_t desc, const char*& text, int& length);
    int64_t addStrings(int64_t lhsDesc, int64_t rhsDesc);
    int compareStrings(int64_t lhsDesc, int64_t rhsDesc);

    int64_t newType(int size);
    void delType(int64_t desc);

    int64_t readFromType(int64_t desc, int offset);
    void writeToType(int64_t desc, int64_t value, int offset);

private:
    int64_t newDesc(int64_t descType);
    void* getDesc(int64_t desc);

    std::vector<void*> mDescriptors;
};
