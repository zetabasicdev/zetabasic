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

#include <cassert>

#include <sstream>
#include <stdexcept>

#include <SDL.h>

#include "Font.h"
#include "Window.h"

Window::Window()
    :
    mWindow(nullptr),
    mScreen(nullptr),
    mCells(nullptr),
    mCursorRow(1),
    mCursorCol(1),
    mCursorVisible(false),
    mFg(7),
    mBg(0),
    mDirty(true),
    mPalette()
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) {
        std::stringstream msg;
        msg << "Failed to initialize SDL : " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }

    mWindow = SDL_CreateWindow("ZetaBASIC", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 400, 0);
    if (!mWindow) {
        std::stringstream msg;
        msg << "Failed to create window : " << SDL_GetError();
        SDL_Quit();
        throw std::runtime_error(msg.str());
    }
    mScreen = SDL_GetWindowSurface((SDL_Window*)mWindow);
    assert(mScreen);

    SDL_FillRect((SDL_Surface*)mScreen, NULL, SDL_MapRGB(((SDL_Surface*)mScreen)->format, 0, 0, 0));
    SDL_UpdateWindowSurface((SDL_Window*)mWindow);

    mCells = new Cell[80 * 25];
    for (int i = 0; i < 80 * 25; ++i) {
        mCells[i].ch = 0;
        mCells[i].color = 7;
    }
}

Window::~Window()
{
    delete[] mCells;

    SDL_DestroyWindow((SDL_Window*)mWindow);
    SDL_Quit();
}

static int mapKey(int sym)
{
    static struct
    {
        int sym;
        int key;
    } mapping[] = {
        { SDLK_UP, UP },
        { SDLK_DOWN, DOWN },
        { SDLK_LEFT, LEFT },
        { SDLK_RIGHT, RIGHT },
        { SDLK_PAGEUP, PAGE_UP },
        { SDLK_PAGEDOWN, PAGE_DOWN },
        { SDLK_HOME, HOME },
        { SDLK_END, END },
        { SDLK_INSERT, INS },
        { SDLK_DELETE, DEL },
        { SDLK_BACKSPACE, BACKSPACE },
        { SDLK_RETURN, ENTER },
        { SDLK_ESCAPE, ESCAPE },
        { SDLK_TAB, TAB },
        { SDLK_F1, F1 },
        { SDLK_F2, F2 },
        { SDLK_F3, F3 },
        { SDLK_F4, F4 },
        { SDLK_F5, F5 },
        { SDLK_F6, F6 },
        { SDLK_F7, F7 },
        { SDLK_F8, F8 },
        { SDLK_F9, F9 },
        { SDLK_F10, F10 },
        { SDLK_F11, F11 },
        { SDLK_F12, F12 },
        { 0, 0 }
    };
    for (int ix = 0; mapping[ix].key > 0; ++ix)
        if (mapping[ix].sym == sym)
            return mapping[ix].key;
    return 0;
}

int Window::runOnce()
{
    SDL_Event evt;
    int e = 0;

    if (mDirty) {
        SDL_UpdateWindowSurface((SDL_Window*)mWindow);
        mDirty = 0;
    }

    while (e == 0 && SDL_WaitEvent(&evt)) {
        switch (evt.type) {
        case SDL_QUIT:
            e = QUIT;
            break;
        case SDL_KEYDOWN:
            e = mapKey(evt.key.keysym.sym);
            break;
        case SDL_TEXTINPUT:
            if (evt.text.text[0] >= 32 && evt.text.text[0] <= 126)
                e = evt.text.text[0];
            break;
        default:
            break;
        }
    }

    return e;
}

void Window::renderCell(char ch, int row, int col, int fg, int bg)
{
    assert(row >= 1 && row <= 25);
    assert(col >= 1 && col <= 80);
    assert(fg >= 0 && fg <= 15);
    assert(bg >= 0 && bg <= 15);

    Uint32 pitch = ((SDL_Surface*)mScreen)->pitch / sizeof(Uint32);
    Uint32* pixel = (Uint32*)((SDL_Surface*)mScreen)->pixels + (((row - 1) * 16) * pitch) + ((col - 1) * 8);
    uint8_t* chr = (uint8_t*)font8x16 + (ch * 16);
    Uint32 colr[2] = { mPalette[bg].getValue(), mPalette[fg].getValue() };

    for (int y = 0; y < 16; ++y) {
        *(pixel) = colr[(*chr & 0x80) >> 7];
        *(pixel + 1) = colr[(*chr & 0x40) >> 6];
        *(pixel + 2) = colr[(*chr & 0x20) >> 5];
        *(pixel + 3) = colr[(*chr & 0x10) >> 4];
        *(pixel + 4) = colr[(*chr & 0x8) >> 3];
        *(pixel + 5) = colr[(*chr & 0x4) >> 2];
        *(pixel + 6) = colr[(*chr & 0x2) >> 1];
        *(pixel + 7) = colr[*chr & 0x1];
        ++chr;
        pixel += pitch;
    }
}

void Window::scroll()
{
    for (int y = 1; y < 25; ++y)
        memcpy(mCells + ((y - 1) * 80), mCells + (y * 80), sizeof(Cell) * 80);

    for (int i = 80 * 25 - 80; i < 80 * 25; ++i) {
        mCells[i].ch = 0;
        mCells[i].color = (mBg << 4) | mFg;
    }

    for (int row = 1; row <= 25; ++row) {
        for (int col = 1; col <= 80; ++col) {
            Cell* cell = mCells + ((row - 1) * 80) + (col - 1);
            renderCell(cell->ch, row, col, cell->color & 0xf, cell->color >> 4);
        }
    }
}

void Window::print(const char* text)
{
    printn(text, (int)strlen(text));
}

void Window::printf(const char* format, ...)
{
    char buf[1024];
    va_list ap;

    va_start(ap, format);
#ifdef _WIN32
    int len = vsprintf_s(buf, sizeof(buf), format, ap);
#else
    int len = vsnprintf(buf, sizeof(buf), format, ap);
#endif
    va_end(ap);

    printn(buf, len);
}

void Window::drawCursor()
{
    Cell* cell = mCells + ((mCursorRow - 1) * 80) + (mCursorCol - 1);
    SDL_Rect dst = { (mCursorCol - 1) * 8, ((mCursorRow - 1) * 16) + 12, 8, 4 };
    SDL_FillRect((SDL_Surface*)mScreen, &dst, mPalette[cell->color & 0xf].getValue());
}

void Window::eraseCursor()
{
    Cell* cell = mCells + ((mCursorRow - 1) * 80) + (mCursorCol - 1);
    renderCell(cell->ch, mCursorRow, mCursorCol, cell->color & 0xf, cell->color >> 4);
}

void Window::printn(const char* text, int len)
{
    if (mCursorVisible)
        eraseCursor();

    while (len > 0) {
        char ch = *text;
        if (!ch)
            ch = ' ';
        else
            ++text;
        --len;

        if (ch == '\n') {
            mCursorCol = 1;
            ++mCursorRow;
            if (mCursorRow > 25) {
                --mCursorRow;
                scroll();
            }
        } else {
            Cell* cell = mCells + ((mCursorRow - 1) * 80) + (mCursorCol - 1);
            cell->ch = ch;
            cell->color = (mBg << 4) | mFg;
            renderCell(ch, mCursorRow, mCursorCol, mFg, mBg);

            if (!(len == 0 && (mCursorRow == 25 && mCursorCol == 80))) {
                ++mCursorCol;
                if (mCursorCol > 80) {
                    mCursorCol = 1;
                    ++mCursorRow;
                    if (mCursorRow > 25) {
                        --mCursorRow;
                        scroll();
                    }
                }
            }
        }
    }

    if (mCursorVisible)
        drawCursor();

    mDirty = true;
}

const std::string& Window::input()
{
    static std::string text;
    text.clear();

    bool rehideCursor = false;

    if (!mCursorVisible) {
        showCursor();
        rehideCursor = true;
    }

    bool done = false;
    while (!done) {
        int evt = runOnce();
        switch (evt) {
        case ENTER:
            done = true;
            break;
        case BACKSPACE:
            if (!text.empty()) {
                text.erase(text.length() - 1, 1);

                eraseCursor();
                --mCursorCol;
                if (mCursorCol < 1) {
                    mCursorCol = 80;
                    --mCursorRow;
                }
                Cell* cell = mCells + ((mCursorRow - 1) * 80) + (mCursorCol - 1);
                cell->ch = ' ';
                cell->color = (mBg << 4) | mFg;
                renderCell(' ', mCursorRow, mCursorCol, mFg, mBg);
                drawCursor();

                mDirty = true;
            }
            break;
        default:
            if (evt >= 32 && evt <= 126) {
                if (text.length() < 255) {
                    text.push_back((char)evt);
                    Cell* cell = mCells + ((mCursorRow - 1) * 80) + (mCursorCol - 1);
                    cell->ch = (char)evt;
                    cell->color = (mBg << 4) | mFg;
                    renderCell((char)evt, mCursorRow, mCursorCol, mFg, mBg);

                    ++mCursorCol;
                    if (mCursorCol > 80) {
                        mCursorCol = 1;
                        ++mCursorRow;
                        if (mCursorRow > 25) {
                            eraseCursor();
                            scroll();
                            drawCursor();
                        }
                    }
                    drawCursor();

                    mDirty = true;
                }
            }
            break;
        }
    }

    hideCursor();

    // move to next line
    ++mCursorRow;
    mCursorCol = 1;
    if (mCursorRow > 25) {
        --mCursorRow;
        scroll();
    }

    if (!rehideCursor)
        showCursor();

    return text;
}

void Window::locate(int row, int col)
{
    if (row == -1)
        row = mCursorRow;
    if (col == -1)
        col = mCursorCol;

    assert(row >= 1 && row <= 25);
    assert(col >= 1 && col <= 80);

    if (row != mCursorRow || col != mCursorCol) {
        if (mCursorVisible)
            eraseCursor();

        mCursorRow = row;
        mCursorCol = col;

        if (mCursorVisible)
            drawCursor();

        mDirty = true;
    }
}

void Window::color(int fg, int bg)
{
    if (fg == -1)
        fg = mFg;
    if (bg == -1)
        bg = mBg;

    assert(fg >= 0 && fg <= 15);
    assert(bg >= 0 && bg <= 15);

    mFg = fg;
    mBg = bg;
}

void Window::showCursor()
{
    if (!mCursorVisible) {
        drawCursor();
        mCursorVisible = true;
        mDirty = true;
    }
}

void Window::hideCursor()
{
    if (mCursorVisible) {
        eraseCursor();
        mCursorVisible = false;
        mDirty = true;
    }
}

void Window::setPalette(const Palette& palette)
{
    mPalette = palette;
}
