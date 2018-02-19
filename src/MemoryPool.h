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
#include <cstdlib>

class MemoryPool
{
public:
    MemoryPool(size_t itemSize, int blockSize)
        :
        mItemSize(itemSize),
        mBlockSize(blockSize),
        mFirstBlock(new Block(itemSize, itemSize, blockSize)),
        mCurBlock(mFirstBlock)
    {
        // intentionally left blank
    }

    ~MemoryPool()
    {
        Block* block = mFirstBlock;
        while (block) {
            Block* next = block->next;
            delete block;
            block = next;
        }
    }

    void reset()
    {
        Block* block = mFirstBlock;
        while (block) {
            block->used = 0;
            block = block->next;
        }
        mCurBlock = mFirstBlock;
    }

    void* allocItems(int count = 1)
    {
        assert(count < mBlockSize);

        if (mCurBlock->used + count > mBlockSize) {
            if (!mCurBlock->next)
                mCurBlock->next = new Block(mItemSize, mItemSize, mBlockSize);
            mCurBlock = mCurBlock->next;
        }

        void* memory = mCurBlock->items + (mCurBlock->used * mItemSize);
        mCurBlock->used += int(mItemSize * count);

        return memory;
    }

private:
    size_t mItemSize;
    int mBlockSize;

    struct Block
    {
        int used;
        char* rawPtr;
        char* items;
        Block* next;

        Block(size_t alignment, size_t size, int count)
            :
            used(0),
            rawPtr(new char[(size * count) + alignment]),
            items(nullptr),
            next(nullptr)
        {
            size_t offset = ((uintptr_t)rawPtr) % alignment;
            items = (offset == 0) ? rawPtr : (rawPtr + (alignment - offset));
        }

        ~Block()
        {
            delete[] rawPtr;
        }
    };

    Block* mFirstBlock;
    Block* mCurBlock;
};
