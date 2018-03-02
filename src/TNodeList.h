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

#include <stdexcept>

template<typename T>
class TNodeList
{
public:
    class Iterator
    {
    public:
        Iterator(T* item)
            :
            mItem(item)
        {
            // intentionally left blank
        }

        ~Iterator()
        {
            // intentionally left blank
        }

        bool operator==(const Iterator& iter) const
        {
            return iter.mItem == mItem;
        }

        bool operator!=(const Iterator& iter) const
        {
            return iter.mItem != mItem;
        }

        T& operator*()
        {
            if (!mItem)
                throw std::runtime_error("iterator out of range");
            return *mItem;
        }

        Iterator& operator++()
        {
            if (mItem)
                mItem = mItem->mNext;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator clone(mItem);
            if (mItem)
                mItem = mItem->mNext;
            return clone;
        }

    private:
        T* mItem;
    };

    TNodeList()
        :
        mFirst(nullptr),
        mLast(nullptr),
        mLength(0)
    {
        // intentionally left blank
    }

    ~TNodeList()
    {
        // intentionally left blank
    }

    int getLength() const
    {
        return mLength;
    }

    void push(T* item)
    {
        if (mLast)
            mLast->mNext = item;
        else
            mFirst = item;
        mLast = item;
        item->mNext = nullptr;
        ++mLength;
    }

    Iterator begin()
    {
        return Iterator(mFirst);
    }

    Iterator end()
    {
        return Iterator(nullptr);
    }

private:
    T* mFirst;
    T* mLast;
    int mLength;
};
