#pragma once

#include "String.h"
#include "TItemPool.h"

const int kStringPoolBlockSize = 4096;

class StringPool
{
public:
    StringPool()
        :
        mCharPool()
    {
        // intentionally left blank
    }

    ~StringPool()
    {
        // intentionally left blank
    }

    void reset()
    {
        mCharPool.reset();
    }

    String alloc(const char* text, int length)
    {
        assert(text);
        assert(length >= 0);
        assert(length < kStringPoolBlockSize);
        char* buf = mCharPool.alloc(length + 1);
        memcpy(buf, text, length);
        buf[length] = 0;
        return String(buf, length);
    }

private:
    TItemPool<char, kStringPoolBlockSize> mCharPool;
};
