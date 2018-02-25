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

#ifdef _WIN32
#include <Windows.h>
#else
#include <cstdio>
#endif

#include <sstream>

#include "CompileError.h"
#include "Compiler.h"
#include "Interpreter.h"
#include "TextSourceStream.h"
#include "Window.h"

void fatalError(const std::string& title, const std::string& message)
{
#ifdef _WIN32
    (void)MessageBoxA(nullptr, message.c_str(), title.c_str(), MB_OK|MB_ICONERROR);
#else
    fprintf(stderr, "%s\n", msg.str().c_str());
#endif
    exit(-1);
}

int main(int argc, char* argv[])
{
    try {
        const char* code = "LET S$ = \"\"\nPRINT S$\nEND";

        Compiler compiler;
        TextSourceStream stream(code);

        auto program = compiler.run(stream);

        Window window;

        Interpreter interpreter(window, program);
        auto result = interpreter.run();
        if (result == InterpreterResult::BadOpcode)
            fatalError("Fatal Error", "Interpreter encountered bad opcode");
    }
    catch (const CompileError& err) {
        std::stringstream msg;
        msg << "[" << err.getRange().getStartRow() << ":" << err.getRange().getStartCol() << "] " << err.what();
        fatalError("Compile Error", msg.str());
    }

    return 0;
}
