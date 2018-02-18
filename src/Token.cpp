#include "Token.h"

const char* ToString(TokenId id)
{
    switch (id) {
    case TokenId::Unknown: return "unknown";
    case TokenId::EndOfSource: return "end-of-source";
    default:
        break;
    }
    return "???";
}

const char* ToString(TokenTag tag)
{
    switch (tag) {
    case TokenTag::None: return "none";
    default:
        break;
    }
    return "???";
}
