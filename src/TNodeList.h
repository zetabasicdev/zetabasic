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
        mLast(nullptr)
    {
        // intentionally left blank
    }

    ~TNodeList()
    {
        // intentionally left blank
    }

    void push(T* item)
    {
        if (mLast)
            mLast->next = item;
        else
            mFirst = item;
        mLast = item;
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
};
