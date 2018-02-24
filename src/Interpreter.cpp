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

#include <iostream>

#include "Interpreter.h"
#include "Opcodes.h"
#include "Program.h"
#include "SystemCalls.h"
#include "Window.h"

Interpreter::Interpreter(Window& window, const Program& program)
    :
    mWindow(window),
    mProgram(program),
    mStringStack()
{
    // intentionally left blank
}

Interpreter::~Interpreter()
{
    // intentionally left blank
}

static void dumpBytecode(const uint8_t* code, int length)
{
    const uint8_t* end = code + length;
    while (code < end) {
        switch (*code) {
        case Op_end:
            std::cout << "end" << std::endl;
            ++code;
            break;
        case Op_load_cstr:
            std::cout << "load.cstr           #" << (int)code[1] << std::endl;
            code += 2;
            break;
        case Op_syscall:
            std::cout << "syscall             #" << (int)code[1] << std::endl;
            code += 2;
            break;
        case Op_add_str:
            std::cout << "add.string" << std::endl;
            ++code;
            break;
        default:
            break;
        }
    }
}

InterpreterResult Interpreter::run()
{
    auto code = mProgram.getBytecode();
    dumpBytecode(code, mProgram.getLength());
    int ip = 0;
    while (ip < mProgram.getLength()) {
        switch (code[ip]) {
        case Op_end:
            mWindow.locate(25, 1);
            mWindow.print("Press any key to continue");
            (void)mWindow.runOnce();
            ip = mProgram.getLength();
            break;
        case Op_load_cstr:
            mStringStack.pushConstant(mProgram.getString(code[ip + 1]));
            ip += 2;
            break;
        case Op_syscall:
            DoSysCall(code[ip + 1]);
            ip += 2;
            break;
        case Op_add_str:
            mStringStack.add();
            ++ip;
            break;
        default:
            return InterpreterResult::BadOpcode;
        }
    }
    
    return InterpreterResult::ExecutionComplete;
}

void Interpreter::DoSysCall(uint8_t ix)
{
    switch (ix) {
    case Syscall_printstr:
    {
        String text = mStringStack.pop();
        mWindow.printn(text.getText(), text.getLength());
        break;
    }
    default:
        break;
    }
}
