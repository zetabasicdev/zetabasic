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
#include "Instructions.h"
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
    mStringManager(mStringStack),
    mCodeSize(mProgram.getCodeSize()),
    mCode(new VmWord[mCodeSize])
{
    memcpy(mCode, mProgram.getCode(), sizeof(VmWord) * mCodeSize);

    static struct {
        VmWord opcode;
        InstructionExecutor func;
        int arguments;
    } translationTable[] = {
        { Op_nop, ExecuteNop, 0 },
        { Op_end, ExecuteEnd, 0 },
        { Op_reserve, ExecuteReserve, 1 },
        { Op_dup, ExecuteDup, 0 },
        { Op_load_const_str, ExecuteLoadConstStr, 1 },
        { Op_load_const, ExecuteLoadConst, 1 },
        { Op_load_local_str, ExecuteLoadLocalStr, 1 },
        { Op_load_local, ExecuteLoadLocal, 1 },
        { Op_store_local_str, ExecuteStoreLocalStr, 1 },
        { Op_store_local, ExecuteStoreLocal, 1 },
        { Op_syscall, ExecuteSyscall, 1 },
        { Op_add_str, ExecuteAddStr, 0 },
        { Op_add_int, ExecuteAddInt, 0 },
        { Op_eq_str, ExecuteEqStr, 0 },
        { Op_eq_int, ExecuteEqInt, 0 },
        { Op_or_int, ExecuteOrInt, 0 },
        { Op_jmp, ExecuteJmp, 1 },
        { Op_jmp_zero, ExecuteJmpZero, 1 },
        { Op_jmp_neq, ExecuteJmpNeq, 1 },
        { Op_jmp_lt, ExecuteJmpLt, 1 },
        { 0, nullptr, 0 }
    };
    // translate code into applicable function calls
    for (int ix = 0; ix < mCodeSize; ++ix) {
        printf("[%04X] ", ix);
        switch (mCode[ix]) {
        case Op_nop:
            printf("nop\n");
        case Op_end:
            printf("end\n");
            break;
        case Op_reserve:
            printf("reserve %d\n", (int)mCode[ix + 1]);
            break;
        case Op_dup:
            printf("dup\n");
            break;
        case Op_load_const_str:
            printf("load.const.str #%d\n", (int)mCode[ix + 1]);
            break;
        case Op_load_const:
            printf("load.const #%d\n", (int)mCode[ix + 1]);
            break;
        case Op_load_local_str:
            printf("load.local.str #%d\n", (int)mCode[ix + 1]);
            break;
        case Op_load_local:
            printf("load.local #%d\n", (int)mCode[ix + 1]);
            break;
        case Op_store_local_str:
            printf("store.local.str #%d\n", (int)mCode[ix + 1]);
            break;
        case Op_store_local:
            printf("store.local #%d\n", (int)mCode[ix + 1]);
            break;
        case Op_syscall:
            printf("syscall #%d\n", (int)mCode[ix + 1]);
            break;
        case Op_add_str:
            printf("add.str\n");
            break;
        case Op_add_int:
            printf("add.int\n");
            break;
        case Op_eq_str:
            printf("equals.str\n");
            break;
        case Op_eq_int:
            printf("equals.int\n");
            break;
        case Op_or_int:
            printf("or.int\n");
            break;
        case Op_jmp:
            printf("jump [%04X]\n", (int)mCode[ix + 1]);
            break;
        case Op_jmp_zero:
            printf("jump.if.zero [%04X]\n", (int)mCode[ix + 1]);
            break;
        case Op_jmp_neq:
            printf("jump.if.neq [%04X]\n", (int)mCode[ix + 1]);
            break;
        case Op_jmp_lt:
            printf("jump.if.lt [%04X]\n", (int)mCode[ix + 1]);
            break;
        default:
            assert(false);
        }
        int count = translationTable[mCode[ix]].arguments;
        mCode[ix] = (VmWord)translationTable[mCode[ix]].func;
        ix += count;
        assert(ix <= mCodeSize);
    }
}

Interpreter::~Interpreter()
{
    delete[] mCode;
}

InterpreterResult Interpreter::run()
{
    ExecutionContext context;
    context.code = mCode;
    context.stack = &mStack;
    context.stringManager = &mStringManager;
    context.stringStack = &mStringStack;
    context.program = &mProgram;
    context.window = &mWindow;

    VmWord* ip = &mCode[0];

    do {
        ip = ((InstructionExecutor)*ip)(&context, ip);
    } while (ip != nullptr);

    mWindow.locate(25, 1);
    mWindow.print("Press any key to continue");
    (void)mWindow.runOnce();

    return InterpreterResult::ExecutionComplete;
}
