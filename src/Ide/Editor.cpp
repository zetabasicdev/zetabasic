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
#include "Editor.h"
#include "EditView.h"
#include "Window.h"

Editor::Editor(Window& window, const std::string& filename)
    :
    mWindow(window),
    mBuffer(nullptr),
    mView(nullptr)
{
    if (!filename.empty())
        mBuffer = new EditBuffer(filename);
    else
        mBuffer = new EditBuffer();
    mView = new EditView(window, *mBuffer);
    mView->setDelegate(this);
}

Editor::~Editor()
{
    delete mView;
    delete mBuffer;
}

void Editor::draw()
{
    mView->draw();
}

bool Editor::handleKey(int key)
{
    return mView->handleKey(key);
}

void Editor::newFile()
{
    delete mView;
    delete mBuffer;

    mBuffer = new EditBuffer();
    mView = new EditView(mWindow, *mBuffer);
    mView->setDelegate(this);

    mView->draw();
    onCursorChanged(1, 1);
}

void Editor::loadFile(const std::string& filename)
{
    auto mNewBuffer = new EditBuffer(filename);

    delete mView;
    delete mBuffer;

    mBuffer = mNewBuffer;
    mView = new EditView(mWindow, *mBuffer);
    mView->setDelegate(this);

    mView->draw();
    onCursorChanged(1, 1);
}

void Editor::saveFile(const std::string& filename)
{
    mBuffer->save(filename);
}

void Editor::onCursorChanged(int row, int col)
{
    if (mDelegate)
        mDelegate->onCursorChanged(row, col);
}

void Editor::setDelegate(Delegate* delegate)
{
    assert(delegate);
    mDelegate = delegate;
}

void Editor::getCode(std::string& code)
{
    mBuffer->getContents(code);
}
