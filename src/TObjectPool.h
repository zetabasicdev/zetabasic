#pragma once

#include <cstddef>
#include <utility>

#include "MemoryPool.h"

template<typename T>
class TObjectPool
    :
    public MemoryPool
{
public:
    TObjectPool(int blockSize)
        :
        MemoryPool(alignof(T), blockSize)
    {
        // intentionally left blank
    }

    ~TObjectPool()
    {
        // intentionally left blank
    }

    template<typename... Args>
    T* alloc(Args&&... args)
    {
        void* memory = allocItems(1);
        return new(memory) T(std::forward<Args>(args)...);
    }
};
