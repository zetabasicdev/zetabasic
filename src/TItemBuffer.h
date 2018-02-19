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
#include <stdexcept>

template<typename T>
class TItemBuffer
{
public:
    TItemBuffer(int startCapacity)
        :
        mCapacity(startCapacity),
        mSize(0),
        mBuffer(new T[mCapacity])
    {
        assert(mCapacity > 0);
    }

    ~TItemBuffer()
    {
        delete[] mBuffer;
    }

    void reset()
    {
        mSize = 0;
    }

    T& operator[](int index)
    {
        if (index < 0 || index >= mSize)
            throw std::runtime_error("out of range");
        return mBuffer[index];
    }

    const T& operator[](int index) const
    {
        if (index < 0 || index >= mSize)
            throw std::runtime_error("out of range");
        return mBuffer[index];
    }

    int getSize() const
    {
        return mSize;
    }

    T* alloc(int count)
    {
        assert(count < mCapacity);
        if (mSize + count > mCapacity) {
            mCapacity *= 2;
            T* newBuffer = new T[mCapacity];
            memcpy(newBuffer, mBuffer, sizeof(T) * mSize);
            delete[] mBuffer;
            mBuffer = newBuffer;
        }

        T* items = mBuffer + mSize;
        mSize += count;
        return items;
    }

    T* alloc(int count, int& index)
    {
        index = mSize;
        return alloc(count);
    }

private:
    int mCapacity;
    int mSize;
    T* mBuffer;
};
