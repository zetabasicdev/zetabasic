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

#include <cstdint>

class Color
{
public:
    Color()
        :
        mRed(0),
        mGreen(0),
        mBlue(0),
        mAlpha(0),
        mValue(0)
    {
        // intentionally left blank
    }

    Color(uint32_t value)
        :
        mRed((value >> 16) & 0xff),
        mGreen((value >> 8) & 0xff),
        mBlue(value & 0xff),
        mAlpha((value >> 24) & 0xff),
        mValue(value)
    {
        // intentionally left blank
    }

    Color(int red, int green, int blue, int alpha = 255)
        :
        mRed(red),
        mGreen(green),
        mBlue(blue),
        mAlpha(alpha),
        mValue(0)
    {
        assert(mRed >= 0 && mRed < 256);
        assert(mGreen >= 0 && mGreen < 256);
        assert(mBlue >= 0 && mBlue < 256);
        assert(mAlpha >= 0 && mAlpha < 256);
        mValue = (mAlpha << 24) | (mRed << 16) | (mGreen << 8) | mBlue;
    }

    ~Color()
    {
        // intentionally left blank
    }

    int getRed() const
    {
        return mRed;
    }

    int getGreen() const
    {
        return mGreen;
    }

    int getBlue() const
    {
        return mBlue;
    }

    int getAlpha() const
    {
        return mAlpha;
    }

    uint32_t getValue() const
    {
        return mValue;
    }

private:
    int mRed;
    int mGreen;
    int mBlue;
    int mAlpha;
    uint32_t mValue;
};
