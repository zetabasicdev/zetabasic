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
