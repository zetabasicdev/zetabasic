#pragma once

class ISourceStream
{
public:
    enum : char
    {
        EndOfStream = 0
    };

    virtual int getRow() const = 0;
    virtual int getCol() const = 0;
    virtual char read() = 0;
};
