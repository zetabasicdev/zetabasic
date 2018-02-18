#pragma once

#include "Range.h"
#include "String.h"

enum class TokenId
{
    Unknown,
    EndOfSource,
    Name
};
const char* ToString(TokenId id);

enum class TokenTag
{
    None,
    Key_End
};
const char* ToString(TokenTag tag);

class Token
{
public:
    Token(TokenId id, TokenTag tag, const String& text, const Range& range)
        :
        mId(id),
        mTag(tag),
        mText(text),
        mRange(range)
    {
        // intentionally left blank
    }

    ~Token()
    {
        // intentionally left blank
    }

    TokenId getId() const
    {
        return mId;
    }

    TokenTag getTag() const
    {
        return mTag;
    }

    const String& getText() const
    {
        return mText;
    }

    const Range& getRange() const
    {
        return mRange;
    }

private:
    TokenId mId;
    TokenTag mTag;
    String mText;
    Range mRange;
};
