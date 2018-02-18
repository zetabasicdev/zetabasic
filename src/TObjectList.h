#pragma once

#include <cstring>
#include <stdexcept>

template<typename T>
class TObjectList
{
public:
    TObjectList()
        :
        mCapacity(32),
        mSize(0),
        mItems(new T*[mCapacity])
    {
        // intentionally left blank
    }

    ~TObjectList()
    {
        delete[] mItems;
    }

    void reset()
    {
        mSize = 0;
    }

    int getSize() const
    {
        return mSize;
    }

    void push(T* obj)
    {
        if (mSize == mCapacity) {
            mCapacity *= 2;
            T** newItems = new T*[mCapacity];
            memcpy(newItems, mItems, sizeof(T*) * mSize);
            delete[] mItems;
            mItems = newItems;
        }

        mItems[mSize++] = obj;
    }

    T& operator[](int index)
    {
        if (index < 0 || index >= mSize)
            throw std::runtime_error("out of bounds access");
        return *mItems[index];
    }

    const T& operator[](int index) const
    {
        if (index < 0 || index >= mSize)
            throw std::runtime_error("out of bounds access");
        return *mItems[index];
    }

private:
    int mCapacity;
    int mSize;
    T** mItems;
};
