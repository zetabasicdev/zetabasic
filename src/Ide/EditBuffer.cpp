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

#include <cstdio>
#include <sstream>
#include <stdexcept>

#include "EditBuffer.h"

EditBuffer::EditBuffer()
    :
    mFirstLine(new EditLine),
    mLastLine(mFirstLine),
    mLineCount(1)
{
    mFirstLine->len = 0;
    mFirstLine->next = mFirstLine->prev = nullptr;
    mFirstLine->text[0] = 0;
}

EditBuffer::EditBuffer(const std::string& filename)
{
#ifdef _WIN32
    FILE* file = nullptr;
    (void)fopen_s(&file, "name", "r");
#else
    FILE* file = fopen(name, "r");
#endif
    if (!file) {
        std::stringstream msg;
        msg << "Failed to open file: " << filename;
        throw std::runtime_error(msg.str());
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = new char[size];
    size = fread(data, sizeof(char), size, file);
    fclose(file);

    // scan file for bad characters
    bool valid = true;
    for (size_t ix = 0; ix < size && valid; ++ix)
        if (data[ix] != '\n' && (data[ix] < 32 || data[ix] > 126))
            valid = false;

    if (!valid) {
        std::stringstream msg;
        msg << "Malformed file: " << filename;
        throw std::runtime_error(msg.str());
    }

    EditLine* line = nullptr;
    EditLine* lastLine = nullptr;
    char* ptr = data;
    while (size > 0) {
        // find next end-of-line
        int len = 0;
        bool hasEol = false;
        char* start = ptr;
        while (size > 0 && len < 80 && *ptr != '\n') {
            ++ptr;
            ++len;
            --size;
        }

        line = new EditLine;
        line->prev = lastLine;
        line->next = nullptr;
        line->len = len;
        if (len > 0)
            memcpy(line->text, start, len);
        line->text[len] = 0;
        if (!lastLine)
            mFirstLine = line;
        else
            lastLine->next = line;
        lastLine = line;

        // was there a newline?
        if (size > 0) {
            while (size > 0 && *ptr != '\n') {
                ++ptr;
                --size;
            }
            if (size > 0) {
                // skip the new line now
                ++ptr;
                --size;
            }
        }
        ++mLineCount;
    }

    if (line) {
        mLastLine = line;
    } else {
        // no data
        mFirstLine = mLastLine = new EditLine;
        mFirstLine->next = mFirstLine->prev = nullptr;
        mFirstLine->len = 0;
        mFirstLine->text[0] = 0;
        mLineCount = 1;
    }

    delete[] data;
}

EditBuffer::~EditBuffer()
{
    auto line = mFirstLine;
    while (line) {
        auto next = line->next;
        delete line;
        line = next;
    }
}
