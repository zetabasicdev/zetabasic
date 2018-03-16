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

#pragma once

#include <string>

struct EditLine
{
    char text[81];
    int len;
    EditLine* prev;
    EditLine* next;
};

class EditBuffer
{
public:
    EditBuffer();
    EditBuffer(const std::string& filename);
    ~EditBuffer();

    void save(const std::string& filename);

    void getContents(std::string& contents);

    EditLine* insertBreak(EditLine* line, int col);
    void insertChar(EditLine* line, int col, char ch);
    bool backspace(EditLine* line, int col);
    bool deleteChar(EditLine* line, int col);

    EditLine* getFirstLine()
    {
        return mFirstLine;
    }

    EditLine* getLastLine()
    {
        return mLastLine;
    }

    int getLineCount()
    {
        return mLineCount;
    }

private:
    EditLine* mFirstLine;
    EditLine* mLastLine;
    int mLineCount;
};
