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

#include "EditBuffer.h"
#include "EditView.h"
#include "Window.h"

EditView::EditView(Window& window, EditBuffer& buffer)
    :
    mWindow(window),
    mBuffer(buffer),
    mTopLine(buffer.getFirstLine()),
    mBottomLine(mTopLine),
    mCurLine(mTopLine),
    mTopRow(1),
    mBottomRow(1),
    mCurRow(1),
    mCurCol(1)
{
    // calculate bottom line/row
    for (auto j = 0; j < 23 && mBottomLine->next; ++j) {
        ++mBottomRow;
        mBottomLine = mBottomLine->next;
    }
}

EditView::~EditView()
{
    // intentionally left blank
}

void EditView::draw()
{
    mWindow.color(7, 1);
    drawAllLines();
    if (mBottomRow - mTopRow + 1 < 24) {
        for (auto i = 0; i < 24 - (mBottomRow - mTopRow + 1); ++i) {
            mWindow.locate(i + mBottomRow + 1, 1);
            mWindow.printn("", 80);
        }
    }
    drawCursor();
    mWindow.showCursor();
}

void EditView::drawAllLines()
{
    drawFromLine(mTopLine, mTopRow);
}

void EditView::drawFromLine(EditLine* line, int row)
{
    while (row <= mBottomRow) {
        drawLine(line, row);
        ++row;
        line = line->next;
    }
}

void EditView::drawLine(EditLine* line, int row)
{
    mWindow.locate(row - mTopRow + 1, 1);
    mWindow.printn(line->text, 80);
}

void EditView::drawCursor()
{
    mWindow.locate(mCurRow - mTopRow + 1, mCurCol);
}
