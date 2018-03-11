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

#include "StringManager.h"
#include "StringPiece.h"
#include "StringStack.h"

StringManager::StringManager(StringStack& stringStack)
    :
    mStringStack(stringStack)
{
    // intentionally left blank
}

StringManager::~StringManager()
{
    for (auto instance : mStrings) {
        delete instance->data;
        delete instance;
    }
}

int64_t StringManager::newString(const StringPiece& string)
{
    return newInstance(string.getText(), string.getLength());
}

void StringManager::getString(int64_t desc, const char*& text, int& length)
{
    // check for uninitialized strings first
    if (desc == 0) {
        text = "";
        length = 0;
        return;
    }

    StringObject* instance = (StringObject*)desc;
    text = instance->data;
    length = instance->length;
}

int64_t StringManager::addStrings(const char* left, int leftLength, const char* right, int rightLength)
{
    int newLength = leftLength + rightLength;
    static std::vector<char> buf;
    buf.resize(newLength);
    if (leftLength > 0)
        memcpy(&buf[0], left, leftLength);
    if (rightLength > 0)
        memcpy(&buf[leftLength], right, rightLength);
    return newInstance(&buf[0], newLength);
}

int64_t StringManager::storeString(int64_t desc)
{
    StringPiece value = mStringStack.pop();
    
    if (desc != 0) {
        removeInstance(desc);
    }

    return newInstance(value.getText(), value.getLength());
}

void StringManager::loadString(int64_t desc)
{
    if (desc == 0) {
        // load empty string
        mStringStack.push(StringPiece());
    } else {
        StringObject* instance = (StringObject*)desc;
        mStringStack.push(StringPiece(instance->data ? instance->data : "", instance->length));
    }
}

void StringManager::removeInstance(int64_t desc)
{
    StringObject* instance = (StringObject*)desc;
    for (auto iter = mStrings.begin(); iter != mStrings.end(); ++iter) {
        if (*iter == instance) {
            delete[] instance->data;
            delete instance;
            mStrings.erase(iter);
            break;
        }
    }
}

int64_t StringManager::newInstance(const char* text, int length)
{
    StringObject* instance = new StringObject;
    instance->length = length;
    if (length > 0) {
        instance->data = new char[length];
        memcpy(instance->data, text, length);
    } else {
        instance->data = nullptr;
    }

    mStrings.push_back(instance);
    return (int64_t)instance;
}
