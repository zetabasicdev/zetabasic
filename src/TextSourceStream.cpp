#include <cassert>
#include <cstring>

#include "TextSourceStream.h"

TextSourceStream::TextSourceStream(const char* text, int length)
    :
    mRow(1),
    mCol(1),
    mLength(length),
    mPosition(0),
    mText(text)
{
    assert(mText);
    assert(mLength >= -1);

    if (mLength == -1)
        mLength = (int)strlen(text);
}

TextSourceStream::~TextSourceStream()
{
    // intentionally left blank
}

int TextSourceStream::getRow() const
{
    return mRow;
}

int TextSourceStream::getCol() const
{
    return mCol;
}

char TextSourceStream::read()
{
    if (mPosition == mLength)
        return ISourceStream::EndOfStream;
    ++mCol;
    if (mText[mPosition] == '\n') {
        ++mRow;
        mCol = 1;
    }
    return mText[mPosition++];
}
