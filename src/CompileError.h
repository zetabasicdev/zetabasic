#pragma once

#include <stdexcept>
#include <string>
#include "Range.h"

enum class CompileErrorId
{
    Unknown,
    SyntaxError
};

class CompileError
    :
    public std::runtime_error
{
public:
    CompileError(CompileErrorId id, const Range& range, const std::string& message)
        :
        std::runtime_error(""),
        mId(id),
        mRange(range),
        mMessage(message)
    {
        // intentionally left blank
    }

    virtual ~CompileError()
    {
        // intentionally left blank
    }

    CompileErrorId getId() const
    {
        return mId;
    }

    const Range& getRange() const
    {
        return mRange;
    }

    virtual const char* what() const noexcept
    {
        return mMessage.c_str();
    }

private:
    CompileErrorId mId;
    Range mRange;
    std::string mMessage;
};
