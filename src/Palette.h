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
#include "Color.h"

const int kPaletteSize = 16;

class Palette
{
public:
    Palette()
        :
        mColors()
    {
        // setup default palette
        mColors[0] = 0x00000000;
        mColors[1] = 0x000000aa;
        mColors[2] = 0x0000aa00;
        mColors[3] = 0x0000aaaa;
        mColors[4] = 0x00aa0000;
        mColors[5] = 0x00aa00aa;
        mColors[6] = 0x00aa5500;
        mColors[7] = 0x00aaaaaa;
        mColors[8] = 0x00555555;
        mColors[9] = 0x005555ff;
        mColors[10] = 0x0055ff55;
        mColors[11] = 0x0055ffff;
        mColors[12] = 0x00ff5555;
        mColors[13] = 0x00ff55ff;
        mColors[14] = 0x00ffff55;
        mColors[15] = 0x00ffffff;
    }

    Palette(const Palette& pal)
        :
        mColors()
    {
        for (int i = 0; i < kPaletteSize; ++i)
            mColors[i] = pal.mColors[i];
    }

    Palette(Palette&& pal)
        :
        mColors()
    {
        for (int i = 0; i < kPaletteSize; ++i)
            mColors[i] = pal.mColors[i];
    }

    ~Palette()
    {
        // intentionally left blank
    }

    Palette& operator=(const Palette& pal)
    {
        if (&pal != this) {
            for (int i = 0; i < kPaletteSize; ++i)
                mColors[i] = pal.mColors[i];
        }
        return *this;
    }

    Palette& operator=(Palette&& pal)
    {
        if (&pal != this) {
            for (int i = 0; i < kPaletteSize; ++i)
                mColors[i] = pal.mColors[i];
        }
        return *this;
    }

    const Color& operator[](int index) const
    {
        assert(index >= 0 && index < kPaletteSize);
        return mColors[index];
    }

    void setColor(int index, const Color& color)
    {
        assert(index >= 0 && index < kPaletteSize);
        mColors[index] = color;
    }

private:
    Color mColors[kPaletteSize];
};
