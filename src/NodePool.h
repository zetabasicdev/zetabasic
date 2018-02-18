#pragma once

#include <unordered_map>
#include "MemoryPool.h"

class NodePool
{
public:
    NodePool()
        :
        mMemoryPools()
    {
        // intentionally left blank
    }

    ~NodePool()
    {
        for (auto kv : mMemoryPools)
            delete kv.second;
    }

    void reset()
    {
        for (auto kv : mMemoryPools)
            kv.second->reset();
    }

    template<typename T>
    T* alloc()
    {
        auto iter = mMemoryPools.find(sizeof(T));
        if (iter == mMemoryPools.end())
            mMemoryPools[sizeof(T)] = new MemoryPool(sizeof(T), 32);
        return new(mMemoryPools[sizeof(T)]->allocItems(1)) T();
    }

private:
    std::unordered_map<size_t, MemoryPool*> mMemoryPools;
};
