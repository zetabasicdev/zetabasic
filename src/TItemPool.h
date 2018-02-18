#pragma once

#include <cassert>

template<typename T, int tBlockSize>
class TItemPool
{
public:
    TItemPool()
        :
        mFirstBlock(new Block),
        mCurBlock(mFirstBlock)
    {
        // intentionally left blank
    }

    ~TItemPool()
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

    T* alloc(int count = 1)
    {
        assert(count < tBlockSize);
        if (mCurBlock->used + count > tBlockSize) {
            if (!mCurBlock->next)
                mCurBlock->next = new Block;
            mCurBlock = mCurBlock->next;
        }

        T* items = mCurBlock->items + mCurBlock->used;
        mCurBlock->used += count;
        return items;
    }

private:
    struct Block
    {
        int used;
        T items[tBlockSize];
        Block* next;

        Block()
            :
            used(0),
            items(),
            next(nullptr)
        {
            // intentionally left blank
        }
    };

    Block* mFirstBlock;
    Block* mCurBlock;
};
