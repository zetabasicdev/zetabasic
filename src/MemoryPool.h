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
        mCurBlock += (mItemSize * count);

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
