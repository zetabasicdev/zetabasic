#pragma once

#include "ISourceStream.h"

class TextSourceStream
    :
    public ISourceStream
{
public:
    TextSourceStream(const char* text, int length = -1);
    virtual ~TextSourceStream();

    virtual int getRow() const;
    virtual int getCol() const;
    virtual char read();

private:
    int mRow;
    int mCol;
    int mLength;
    int mPosition;
    const char* mText;
};
