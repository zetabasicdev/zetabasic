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

    String(const char* text)
        :
        mText(text),
        mLength(0)
    {
        assert(mText);
        mLength = (int)strlen(mText);
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
#ifdef _WIN32
        return _strnicmp(mText, text, mLength) == 0;
#else
        return strncasecmp(mText, text, mLength) == 0;
#endif
    }

    bool operator==(const String& str) const
    {
        if (str.mLength == mLength)
#ifdef _WIN32
            return _strnicmp(mText, str.mText, mLength) == 0;
#else
            return strncasecmp(mText, str.mText, mLength) == 0;
#endif
        return false;
    }

    bool operator!=(const char* text) const
    {
        return !operator==(text);
    }

    bool operator!=(const String& str) const
    {
        return !operator==(str);
    }

    char operator[](int index) const
    {
        assert(index >= 0 && index < mLength);
        return mText[index];
    }

    int compareWithCase(const String& str) const
    {
        int len = mLength;
        if (str.mLength < len)
            len = str.mLength;
        return strncmp(mText, str.mText, len);
    }

private:
    const char* mText;
    int mLength;
};
