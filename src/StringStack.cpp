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

#include "StringStack.h"

static const auto kDefaultDataCapacity = 2048;
static const auto kDefaultLengthCapacity = 64;
static const auto kMaximumStringLength = 2048;

StringStack::StringStack()
    :
    mCount(0),
    mDataCapacity(kDefaultDataCapacity),
    mDataUsed(0),
    mData(new char[mDataCapacity]),
    mLengthCapacity(kDefaultLengthCapacity),
    mLengths(new int[mLengthCapacity])
{
    // intentionally left blank
}

StringStack::~StringStack()
{
    delete[] mLengths;
    delete[] mData;
}

void StringStack::pushConstant(const String& value)
{
    int len = value.getLength();
    assert(len <= kMaximumStringLength);

    if (mCount == mLengthCapacity) {
        mLengthCapacity *= 2;
        int* newLengths = new int[mLengthCapacity];
        memcpy(newLengths, mLengths, sizeof(int) * mCount);
        delete[] mLengths;
        mLengths = newLengths;
    }

    if (mDataUsed + len > mDataCapacity) {
        mDataCapacity *= 2;
        char* newData = new char[mDataCapacity];
        memcpy(newData, mData, sizeof(char) * mDataUsed);
        delete[] mData;
        mData = newData;
    }

    mLengths[mCount] = len;
    memcpy(mData + mDataUsed, value.getText(), len);
    mDataUsed += len;

    ++mCount;
}

void StringStack::add()
{
    assert(mCount >= 2);

    // simply combine top two strings into a single string
    mLengths[mCount - 2] += mLengths[mCount - 1];
    --mCount;
}

String StringStack::pop()
{
    assert(mCount > 0);

    const char* text = mData + mDataUsed - mLengths[mCount - 1];
    int len = mLengths[mCount - 1];

    --mCount;
    mDataUsed -= mLengths[mCount];

    return String(text, len);
}
