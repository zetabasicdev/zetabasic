#pragma once

#include <cassert>
#include <cstring>

class String
{
public:
    String()
        :
        mText(""),
        mLength(0)
    {
        // intentionally left blank
    }

    String(const char* text, int length)
        :
        mText(text),
        mLength(length)
    {
        assert(mText);
        assert(mLength >= 0);
    }

    ~String()
    {
        // intentionally left blank
    }

    const char* getText() const
    {
        return mText;
    }

    int getLength() const
    {
        return mLength;
    }

    bool operator==(const char* text) const
    {
        assert(text);
        return _strnicmp(mText, text, mLength) == 0;
    }

    bool operator==(const String& str) const
    {
        if (str.mLength == mLength)
            return _strnicmp(mText, str.mText, mLength) == 0;
        return false;
    }

private:
    const char* mText;
    int mLength;
};
