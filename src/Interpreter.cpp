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

#include "Interpreter.h"
#include "Opcodes.h"
#include "Program.h"
#include "SystemCalls.h"
#include "Window.h"

Interpreter::Interpreter(Window& window, const Program& program)
    :
    mWindow(window),
    mProgram(program),
    mStack(),
    mStringStack(),
    mStringManager(mStringStack)
{
    // intentionally left blank
}

Interpreter::~Interpreter()
{
    // intentionally left blank
}

static void dumpBytecode(const Program& program)
{
    enum
    {
        Type_NoArgs,
        Type_OneInt,
        Type_OneIndex,
        Type_Offset,
        Type_Location
    };
    static struct {
        uint8_t op;
        const char* name;
        int size;
        int type;
    } mapping[] = {
        { Op_end, "end", 1, Type_NoArgs },
        { Op_reserve, "reserve", 2, Type_OneInt },
        { Op_dup, "dup", 1, Type_NoArgs },
        { Op_load_const_str, "load.const.str", 2, Type_OneIndex },
        { Op_load_const, "load.const", 2, Type_OneIndex },
        { Op_load_local_str, "load.local.str", 2, Type_OneIndex },
        { Op_load_local, "load.local", 2, Type_OneIndex },
        { Op_store_local_str, "store.local.str", 2, Type_OneIndex },
        { Op_store_local, "store.local", 2, Type_OneIndex },
        { Op_syscall, "syscall", 2, Type_OneIndex },
        { Op_add_str, "add.str", 1, Type_NoArgs },
        { Op_add_int, "add.int", 1, Type_NoArgs },
        { Op_eq_str, "equals.str", 1, Type_NoArgs },
        { Op_eq_int, "equals.int", 1, Type_NoArgs },
        { Op_or_int, "or.int", 1, Type_NoArgs },
        { Op_jmp, "jump", 3, Type_Location },
        { Op_jmp_zero, "jump.if.zero", 2, Type_Offset },
        { Op_jmp_neq, "jump.if.not.equal", 2, Type_Offset },
        { Op_jmp_lt, "jump.if.less", 2, Type_Offset },
        { 0, nullptr, 0, 0 }
    };

    program.dumpStrings();
    
    const uint8_t* code = program.getBytecode();
    int length = program.getLength();
    const uint8_t* start = code;
    const uint8_t* end = code + length;
    while (code < end) {
        bool found = false;
        for (auto ix = 0; mapping[ix].name; ++ix) {
            if (mapping[ix].op == *code) {
                if (mapping[ix].size == 1)
                    printf("%04X: %02X        ", (int)(code - start), (int)code[0]);
                else if (mapping[ix].size == 2)
                    printf("%04X: %02X %02X     ", (int)(code - start), (int)code[0], (int)code[1]);
                else if (mapping[ix].size == 3)
                    printf("%04X: %02X %02X %02X  ", (int)(code - start), (int)code[0], (int)code[1], (int)code[2]);
                switch (mapping[ix].type) {
                case Type_NoArgs:
                    printf("%s\n", mapping[ix].name);
                    break;
                case Type_OneInt:
                    printf("%s %d\n", mapping[ix].name, (int)code[1]);
                    break;
                case Type_OneIndex:
                    printf("%s #%d\n", mapping[ix].name, (int)code[1]);
                    break;
                case Type_Offset:
                    printf("%s %+d (%04X)\n", mapping[ix].name, (int)((int8_t)code[1]), (int)(code - start) + (int)((int8_t)code[1]));
                    break;
                case Type_Location:
                    printf("%s (%02X%02X)\n", mapping[ix].name, (int)code[1], (int)code[2]);
                    break;
                default:
                    break;
                }
                code += mapping[ix].size;
                found = true;
                break;
            }
        }
        if (!found) {
            printf("(%04X) unknown opcode : %02X\n", (int)(code - start), (int)*code);
            break;
        }
    }
}

InterpreterResult Interpreter::run()
{
    auto code = mProgram.getBytecode();
    dumpBytecode(mProgram);
    int ip = 0;
    while (ip < mProgram.getLength()) {
        switch (code[ip]) {
        case Op_end:
            mWindow.locate(25, 1);
            mWindow.print("Press any key to continue");
            (void)mWindow.runOnce();
            ip = mProgram.getLength();
            break;
        case Op_reserve:
            mStack.reserve(code[ip + 1]);
            ip += 2;
            break;
        case Op_dup:
            mStack.dup();
            ++ip;
            break;
        case Op_load_const_str:
            mStringStack.push(mProgram.getString(code[ip + 1]));
            ip += 2;
            break;
        case Op_load_const:
            mStack.push(mProgram.getIntegerConstant(code[ip + 1]));
            ip += 2;
            break;
        case Op_load_local_str:
            mStringManager.loadString(mStack.getLocal(code[ip + 1]));
            ip += 2;
            break;
        case Op_load_local:
            mStack.push(mStack.getLocal(code[ip + 1]));
            ip += 2;
            break;
        case Op_store_local_str:
            mStack.setLocal(code[ip + 1], mStringManager.storeString(mStack.getLocal(code[ip + 1])));
            ip += 2;
            break;
        case Op_store_local:
            mStack.setLocal(code[ip + 1], mStack.pop());
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
        case Op_add_int:
        {
            int64_t rhs = mStack.pop();
            int64_t lhs = mStack.pop();
            mStack.push(lhs + rhs);

            ++ip;
            break;
        }
        case Op_eq_str:
            mStack.push((mStringStack.compare() == 0) ? 1 : 0);
            ++ip;
            break;
        case Op_eq_int:
        {
            int64_t rhs = mStack.pop();
            int64_t lhs = mStack.pop();
            mStack.push(lhs == rhs);
            ++ip;
            break;
        }
        case Op_or_int:
        {
            int64_t rhs = mStack.pop();
            int64_t lhs = mStack.pop();
            mStack.push(lhs | rhs);
            ++ip;
            break;
        }
        case Op_jmp:
            ip = (code[ip + 1] << 8) + code[ip + 2];
            break;
        case Op_jmp_zero:
            if (mStack.pop() == 0)
                ip += (int8_t)code[ip + 1];
            else
                ip += 2;
            break;
        case Op_jmp_neq:
        {
            int64_t rhs = mStack.pop();
            int64_t lhs = mStack.pop();
            if (lhs != rhs)
                ip += (int8_t)code[ip + 1];
            else
                ip += 2;
            break;
        }
        case Op_jmp_lt:
        {
            int64_t rhs = mStack.pop();
            int64_t lhs = mStack.pop();
            if (lhs < rhs)
                ip += (int8_t)code[ip + 1];
            else
                ip += 2;
            break;
        }
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
    case Syscall_printi:
    {
        int64_t value = mStack.pop();
        mWindow.printf("%lld", value);
        break;
    }
    case Syscall_printnl:
        mWindow.print("\n");
        break;
    case Syscall_inputi:
        mStack.push((int64_t)atoi(mWindow.input().c_str()));
        break;
    case Syscall_inputstr:
        mStringStack.push(String(mWindow.input()));
        break;
    case Syscall_len:
        mStack.push((int64_t)mStringStack.len());
        break;
    case Syscall_left_str:
        mStringStack.left((int)mStack.pop());
        break;
    default:
        break;
    }
}
