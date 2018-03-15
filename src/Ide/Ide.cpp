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

#include "Ide.h"

Ide::Ide(const std::string& filename)
    :
    mWindow(),
    mStatusBar(mWindow),
    mEditor(mWindow, filename)
{
    mEditor.setDelegate(this);
}

Ide::~Ide()
{
    // intentionally left blank
}

void Ide::run()
{
    Palette idePalette;
    idePalette.setColor(1, Color(0x00, 0x2b, 0x36));
    idePalette.setColor(3, Color(0x2a, 0xa1, 0x98));
    idePalette.setColor(4, Color(0xdc, 0x32, 0x2f));
    idePalette.setColor(7, Color(0x83, 0x94, 0x96));
    idePalette.setColor(8, Color(0x6c, 0x71, 0xc4));
    mWindow.setPalette(idePalette);
    mStatusBar.draw();
    mEditor.draw();

    int evt = 0;
    do {
        evt = mWindow.runOnce();
        if (!mEditor.handleKey(evt)) {
            switch (evt) {
            case F1:
                mEditor.newFile();
                break;
            case F2:
                loadFile();
                break;
            case F3:
                saveFile();
                break;
            case F4:
                break;
            case F5:
                break;
            default:
                break;
            }
        }
    } while (evt != QUIT && evt != F10);
}

void Ide::onCursorChanged(int row, int col)
{
    int oldRow = 0, oldCol = 0;
    mWindow.getCursorLocation(oldRow, oldCol);

    mStatusBar.setCursorPostion(row, col);

    mWindow.color(7, 1);
    mWindow.locate(oldRow, oldCol);
}

const std::string& Ide::getFilename()
{
    int row = 0, col = 0;
    mWindow.getCursorLocation(row, col);

    mWindow.color(1, 8);
    mWindow.locate(25, 1);
    mWindow.printn("", 80);
    mWindow.locate(25, 1);

    auto& filename = mWindow.input(80, true, false);

    mStatusBar.draw();
    mWindow.locate(row, col);

    return filename;
}

void Ide::loadFile()
{
    try {
        auto& filename = getFilename();
        mEditor.loadFile(filename);
    }
    catch (std::runtime_error) {
        int row = 0, col = 0;
        mWindow.getCursorLocation(row, col);

        mWindow.color(1, 4);
        mWindow.locate(25, 1);
        mWindow.printn("", 80);
        mWindow.locate(25, 1);

        mWindow.hideCursor();
        mWindow.print("Unable to load specified file!");
        (void)mWindow.runOnce();

        mStatusBar.draw();
        mWindow.locate(row, col);
        mWindow.showCursor();
    }
}

void Ide::saveFile()
{
    try {
        auto& filename = getFilename();
        mEditor.saveFile(filename);
    }
    catch (std::runtime_error) {
        int row = 0, col = 0;
        mWindow.getCursorLocation(row, col);

        mWindow.color(1, 4);
        mWindow.locate(25, 1);
        mWindow.printn("", 80);
        mWindow.locate(25, 1);

        mWindow.hideCursor();
        mWindow.print("Unable to load specified file!");
        (void)mWindow.runOnce();

        mStatusBar.draw();
        mWindow.locate(row, col);
        mWindow.showCursor();
    }
}
