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

    static InstructionExecutor translationTable[] = {
        ExecuteNop,
        ExecuteEnd,
        ExecuteReserve,
        ExecuteJmp,
        ExecuteJmpZero,
        ExecuteJmpNotZero,
        ExecuteLoadConstant,
        ExecuteLoadString,
        ExecuteAddIntegers0,
        ExecuteAddIntegers1,
        ExecuteAddIntegers2,
        ExecuteAddIntegers3,
        ExecuteAddReals3,
        ExecuteAddStrings0,
        ExecuteAddStrings1,
        ExecuteAddStrings2,
        ExecuteAddStrings3,
        ExecuteEqIntegers0,
        ExecuteEqIntegers1,
        ExecuteEqIntegers2,
        ExecuteEqIntegers3,
        ExecuteEqReals3,
        ExecuteEqStrings0,
        ExecuteEqStrings1,
        ExecuteEqStrings2,
        ExecuteEqStrings3,
        ExecuteGrIntegers0,
        ExecuteGrIntegers1,
        ExecuteGrIntegers2,
        ExecuteGrIntegers3,
        ExecuteOrIntegers0,
        ExecuteOrIntegers1,
        ExecuteOrIntegers2,
        ExecuteOrIntegers3,
        ExecuteNegInteger0,
        ExecuteNegInteger1,
        ExecuteNegReal1,
        ExecuteNotInteger0,
        ExecuteNotInteger1,
        ExecuteIntToReal0,
        ExecuteIntToReal1,
        ExecuteRealToInt1,
        ExecuteMove0,
        ExecuteMove1,
        ExecutePrintBoolean0,
        ExecutePrintBoolean1,
        ExecutePrintBoolean0Newline,
        ExecutePrintBoolean1Newline,
        ExecutePrintInteger0,
        ExecutePrintInteger1,
        ExecutePrintInteger0Newline,
        ExecutePrintInteger1Newline,
        ExecutePrintReal0,
        ExecutePrintReal1,
        ExecutePrintReal0Newline,
        ExecutePrintReal1Newline,
        ExecutePrintString0,
        ExecutePrintString1,
        ExecutePrintString0Newline,
        ExecutePrintString1Newline,
        ExecuteInputInteger,
        ExecuteInputString,
        ExecuteFnLen,
        ExecuteFnLeft
    };
    // translate code into applicable function calls
    for (int ix = 0; ix < mCodeSize; ) {
        int count = getInstructionSize(mCode[ix]);
        mCode[ix] = (VmWord)translationTable[mCode[ix] >> 8];
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
        //printf("%06llX\n", (uint64_t)(ip - mCode));
        ip = ((InstructionExecutor)*ip)(&context, ip);
    } while (ip != nullptr);

    mWindow.locate(25, 1);
    mWindow.print("Press any key to continue");
    (void)mWindow.runOnce();

    return InterpreterResult::ExecutionComplete;
}
