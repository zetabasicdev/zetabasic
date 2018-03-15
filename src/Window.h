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
#include <string>
#include "Palette.h"

enum
{
    UP = 256,
    DOWN,
    LEFT,
    RIGHT,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    INS,
    DEL,
    BACKSPACE,
    ENTER,
    ESCAPE,
    TAB,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    QUIT = 1024
};

class Window
{
public:
    Window();
    ~Window();

    int runOnce();

    void print(const char* text);
    void printf(const char* format, ...);
    void printn(const char* text, int len);

    const std::string& input(int maxLength = -1, bool allowEscape = false, bool moveToNextLine = true);

    void locate(int row, int col);
    void color(int fg, int bg);

    void showCursor();
    void hideCursor();

    void getCursorLocation(int& row, int& col);

    void setPalette(const Palette& palette);

private:
    void* mWindow;
    void* mScreen;

    struct Cell
    {
        char ch;
        uint8_t color;
    };
    Cell* mCells;

    int mCursorRow;
    int mCursorCol;
    bool mCursorVisible;
    int mFg;
    int mBg;
    bool mDirty;

    Palette mPalette;

    void renderCell(char ch, int row, int col, int fg, int bg);
    void scroll();
    void drawCursor();
    void eraseCursor();
};
