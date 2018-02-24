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
#include <cstring>

#include "Stack.h"

static const auto kDefaultStackCapacity = 2048;

Stack::Stack()
    :
    mCapacity(kDefaultStackCapacity),
    mPointer(0),
    mData(new int64_t[mCapacity])
{
    // intentionally left blank
}

Stack::~Stack()
{
    delete[] mData;
}

void Stack::reserve(int count)
{
    assert(count < kDefaultStackCapacity);

    if (mPointer + count > mCapacity) {
        mCapacity *= 2;
        int64_t* newData = new int64_t[mCapacity];
        memcpy(newData, mData, sizeof(int64_t) * mPointer);
        delete[] mData;
        mData = newData;
    }

    memset(&mData[mPointer], 0, sizeof(int64_t) * count);
    mPointer += count;
}

void Stack::push(int64_t value)
{
    if (mPointer == mCapacity) {
        mCapacity *= 2;
        int64_t* newData = new int64_t[mCapacity];
        memcpy(newData, mData, sizeof(int64_t) * mPointer);
        delete[] mData;
        mData = newData;
    }

    mData[mPointer++] = value;
}

int64_t Stack::pop()
{
    assert(mPointer > 0);
    return mData[--mPointer];
}

int64_t Stack::getLocal(int index)
{
    assert(mPointer > index);
    return mData[index];
}

void Stack::setLocal(int index, int64_t value)
{
    assert(mPointer > index);
    mData[index] = value;
}
