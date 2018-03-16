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
    if (mDelegate)
        mDelegate->onCursorChanged(mCurRow, mCurCol);
}

bool EditView::handleKey(int key)
{
    bool handled = true;

    int oldRow = mCurRow;
    int oldCol = mCurCol;

    switch (key) {
    case RIGHT:
        if (mCurCol < 80) {
            ++mCurCol;
            drawCursor();
        }
        break;
    case LEFT:
        if (mCurCol > 1) {
            --mCurCol;
            drawCursor();
        }
        break;
    case DOWN:
        if (mCurRow < mBottomRow) {
            mCurLine = mCurLine->next;
            ++mCurRow;
            drawCursor();
        } else if (mBottomLine->next) {
            mTopLine = mTopLine->next;
            mBottomLine = mBottomLine->next;
            mCurLine = mCurLine->next;
            ++mTopRow;
            ++mBottomRow;
            ++mCurRow;
            drawAllLines();
            drawCursor();
        }
        break;
    case UP:
        if (mCurRow > mTopRow) {
            mCurLine = mCurLine->prev;
            --mCurRow;
            drawCursor();
        } else if (mTopLine->prev) {
            mTopLine = mTopLine->prev;
            mBottomLine = mBottomLine->prev;
            mCurLine = mCurLine->prev;
            --mTopRow;
            --mBottomRow;
            --mCurRow;
            drawAllLines();
            drawCursor();
        }
        break;
    case HOME:
        if (mCurCol > 1) {
            mCurCol = 1;
            drawCursor();
        }
        break;
    case END:
        if (mCurCol != mCurLine->len + 1) {
            int col = mCurLine->len + 1;
            if (col > 80)
                col = 80;
            if (mCurCol != col) {
                mCurCol = col;
                drawCursor();
            }
        }
        break;
    case ENTER:
        insertBreak();
        break;
    case BACKSPACE:
        backspace();
        break;
    case DEL:
        deleteChar();
        break;
    case PAGE_DOWN:
        if (mBottomLine->next) {
            for (int i = 0; i < 24 && mBottomLine->next; ++i) {
                mTopLine = mTopLine->next;
                mCurLine = mCurLine->next;
                mBottomLine = mBottomLine->next;
                ++mTopRow;
                ++mCurRow;
                ++mBottomRow;
            }
            drawAllLines();
            drawCursor();
        } else if (mCurRow < mBottomRow) {
            mCurLine = mBottomLine;
            mCurRow = mBottomRow;
            drawCursor();
        }
        break;
    case PAGE_UP:
        if (mTopLine->prev) {
            for (int i = 0; i < 24 && mTopLine->prev; ++i) {
                mTopLine = mTopLine->prev;
                mCurLine = mCurLine->prev;
                mBottomLine = mBottomLine->prev;
                --mTopRow;
                --mCurRow;
                --mBottomRow;
            }
            drawAllLines();
            drawCursor();
        } else if (mCurRow > mTopRow) {
            mCurLine = mTopLine;
            mCurRow = mTopRow;
            drawCursor();
        }
        break;
    default:
        if (key >= 32 && key <= 126)
            insertChar((char)key);
        else
            handled = false;
        break;
    }

    if ((mCurRow != oldRow || mCurCol != oldCol) && mDelegate)
        mDelegate->onCursorChanged(mCurRow, mCurCol);

    return handled;
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

void EditView::insertBreak()
{
    EditLine* newLine = mBuffer.insertBreak(mCurLine, mCurCol);

    // determine view scenario after line is inserted
    if (mBottomRow - mTopRow + 1 < 24) {
        // not a full view, so simply expand it
        ++mBottomRow;
        if (newLine->prev == mBottomLine)
            // new line is at bottom, so move that
            mBottomLine = newLine;
    } else {
        // full view
        if (mCurRow < mBottomRow) {
            // not at the bottom, so adjust new bottom accordingly
            mBottomLine = mBottomLine->prev;
        } else {
            // at the bottom, so scrolling is required
            ++mTopRow;
            ++mBottomRow;
            mTopLine = mTopLine->next;
            mBottomLine = mBottomLine->next;
        }
    }

    // and update the cursor to new line
    ++mCurRow;
    mCurCol = 1;
    mCurLine = mCurLine->next;

    drawAllLines();
    drawCursor();
}

void EditView::insertChar(char ch)
{
    mBuffer.insertChar(mCurLine, mCurCol, ch);

    ++mCurCol;
    if (mCurCol > 80)
        mCurCol = 80;

    drawLine(mCurLine, mCurRow);
    drawCursor();
}

void EditView::backspace()
{
    if (mCurCol > 1 || mCurRow > 1) {
        auto prevLine = mCurLine->prev;
        auto prevLen = 0;
        if (prevLine)
            prevLen = prevLine->len;
        auto lineRemoved = mBuffer.backspace(mCurLine, mCurCol);

        if (!lineRemoved) {
            // just need to redraw line
            --mCurCol;
            drawLine(mCurLine, mCurRow);
            drawCursor();
        } else {
            // adjust the view accordingly
            if (mCurLine == mBottomLine) {
                mBottomLine = prevLine;
            } else if (mCurLine == mTopLine) {
                mTopLine = prevLine;
                --mTopRow;
            }
            mCurLine = prevLine;
            if (mBottomLine->next)
                mBottomLine = mBottomLine->next;
            else
                --mBottomRow;
            --mCurRow;

            // move cursor to where appending took place on previous line
            mCurCol = prevLen + 1;
            if (mCurCol > 80)
                mCurCol = 80;

            // ensure view is full when possible
            if (mBottomRow - mTopRow + 1 < 24 && mTopRow > 1) {
                --mTopRow;
                mTopLine = mTopLine->prev;
            }

            drawAllLines();
            if (mBottomRow - mTopRow + 1 < 24) {
                // need to erase the last line due to upward movement
                mWindow.locate(mBottomRow - mTopRow + 2, 1);
                mWindow.printn("", 80);
            }
            drawCursor();
        }
    }
}

void EditView::deleteChar()
{
    if (mCurCol - 1 < mCurLine->len || mCurLine->next) {
        auto nextLine = mCurLine->next;
        auto lineRemoved = mBuffer.deleteChar(mCurLine, mCurCol);

        if (!lineRemoved) {
            // just need to redraw line
            drawLine(mCurLine, mCurRow);
            drawCursor();
        } else {
            // adjust view as necessary
            if (nextLine == mBottomLine) {
                // last line removed, so adjust view
                mBottomLine = mCurLine;
                --mBottomRow;
                if (mTopLine->prev) {
                    mTopLine = mTopLine->prev;
                    --mTopRow;
                }
            } else {
                // not last line, so try to bring up next as new bottom line
                if (mBottomLine->next) {
                    mBottomLine = mBottomLine->next;
                }  else if (mTopLine->prev) {
                    mTopLine = mTopLine->prev;
                    --mTopRow;
                    --mBottomRow;
                } else {
                    --mBottomRow;
                }
            }

            drawAllLines();
            if (mBottomRow - mTopRow + 1 < 24) {
                // need to erase the last line due to upward movement
                mWindow.locate(mBottomRow - mTopRow + 2, 1);
                mWindow.printn("", 80);
            }
            drawCursor();
        }
    }
}

void EditView::setDelegate(Delegate* delegate)
{
    assert(delegate);
    mDelegate = delegate;
}
