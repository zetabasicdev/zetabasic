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
#include "MemoryManager.h"
#include "StringPiece.h"

MemoryManager::MemoryManager()
    :
    mDescriptors(),
    mNextDescId(1)
{
    // intentionally left blank
}

MemoryManager::~MemoryManager()
{
    // intentionally left blank
}

void MemoryManager::delMemory(int64_t desc, bool removeFromMap)
{
    void* mem = mDescriptors[desc];

    switch (desc & 0xff) {
    case MemoryType_String:
        delete[] (char*)mem;
        break;
    case MemoryType_Udt:
        delete[] (char*)mem;
        break;
    case MemoryType_Array:
    {
        ArrayDesc* array = (ArrayDesc*)mem;
        delete[] array->data;
        delete array;
        break;
    }
    case MemoryType_Unknown:
    case MemoryType_SmallString:
        assert(false);
    default:
        break;
    }

    if (removeFromMap)
        mDescriptors.erase(desc);
}

int64_t MemoryManager::newString(const char* text, int length)
{
    if (length == 0)
        return 0;

    char* mem = new char[length + sizeof(int)];
    *(int*)mem = length;
    memcpy(mem + sizeof(length), text, length);

    return newDesc(MemoryType_String, mem);
}

void MemoryManager::getString(int64_t desc, const char*& text, int& length)
{
    if (desc == 0) {
        text = "";
        length = 0;
        return;
    }

    char* mem = (char*)getDesc(desc);
    length = *(int*)mem;
    text = mem + sizeof(int);
}

int64_t MemoryManager::addStrings(int64_t lhsDesc, int64_t rhsDesc)
{
    const char* left = "";
    int leftLen = 0;
    const char* right = "";
    int rightLen = 0;

    if (lhsDesc != 0) {
        char* lhsMem = (char*)getDesc(lhsDesc);
        leftLen = *(int*)lhsMem;
        left = lhsMem + sizeof(int);
    }
    if (rhsDesc != 0) {
        char* rhsMem = (char*)getDesc(rhsDesc);
        rightLen = *(int*)rhsMem;
        right = rhsMem + sizeof(int);
    }

    static std::vector<char> buf;
    buf.resize(leftLen + rightLen);
    if (leftLen > 0)
        memcpy(&buf[0], left, leftLen);
    if (rightLen > 0)
        memcpy(&buf[leftLen], right, rightLen);
    return newString(&buf[0], leftLen + rightLen);
}

int MemoryManager::compareStrings(int64_t lhsDesc, int64_t rhsDesc)
{
    const char* left = "";
    int leftLen = 0;
    const char* right = "";
    int rightLen = 0;

    if (lhsDesc != 0) {
        char* lhsMem = (char*)getDesc(lhsDesc);
        leftLen = *(int*)lhsMem;
        left = lhsMem + sizeof(int);
    }
    if (rhsDesc != 0) {
        char* rhsMem = (char*)getDesc(rhsDesc);
        rightLen = *(int*)rhsMem;
        right = rhsMem + sizeof(int);
    }

    return StringPiece(left, leftLen).exactCompareWithCaseInt(StringPiece(right, rightLen));
}

int64_t MemoryManager::newType(int size)
{
    char* mem = new char[size];
    memset(mem, 0, size);
    return newDesc(MemoryType_Udt, mem);
}

int64_t MemoryManager::readFromType(int64_t desc, int offset)
{
    int64_t* mem = (int64_t*)getDesc(desc);
    return mem[offset / 8];
}

void MemoryManager::writeToType(int64_t desc, int64_t value, int offset)
{
    int64_t* mem = (int64_t*)getDesc(desc);
    mem[offset / 8] = value;
}

int64_t MemoryManager::newArray(int64_t lower, int64_t upper, int elementSize)
{
    assert(upper >= lower);
    assert(elementSize > 0);

    ArrayDesc* array = new ArrayDesc;
    array->lowerBound = lower;
    array->upperBound = upper;
    array->elementSize = elementSize;
    if (upper > lower)
        array->data = new char[elementSize * (upper - lower + 1)];
    else
        array->data = nullptr;

    return newDesc(MemoryType_Array, array);
}

int64_t MemoryManager::newDesc(int64_t descType, void* mem)
{
    int64_t count = int64_t(mDescriptors.size());
    int64_t desc = (mNextDescId << 8) | descType;
    mDescriptors[desc] = mem;
    return desc;
}

void* MemoryManager::getDesc(int64_t desc)
{
    return mDescriptors[desc];
}
