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
    mStacks(new Stack[4]),
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
        ExecuteInitMem,
        ExecuteFreeMem,
        ExecuteReadMem,
        ExecuteWriteMem,
        ExecuteJmp,
        ExecuteJmpZero,
        ExecuteJmpNotZero,
        ExecuteLoadConstant,
        ExecuteLoadString,
        ExecuteAddIntegers,
        ExecuteAddReals,
        ExecuteAddStrings,
        ExecuteSubIntegers,
        ExecuteSubReals,
        ExecuteMulIntegers,
        ExecuteMulReals,
        ExecuteDivIntegers,
        ExecuteDivReals,
        ExecuteModIntegers,
        ExecuteModReals,
        ExecuteEqualIntegers,
        ExecuteEqualReals,
        ExecuteEqualStrings,
        ExecuteNotEqualIntegers,
        ExecuteNotEqualReals,
        ExecuteNotEqualStrings,
        ExecuteLessIntegers,
        ExecuteLessReals,
        ExecuteLessStrings,
        ExecuteGreaterIntegers,
        ExecuteGreaterReals,
        ExecuteGreaterStrings,
        ExecuteLessEqualsIntegers,
        ExecuteLessEqualsReals,
        ExecuteLessEqualsStrings,
        ExecuteGreaterEqualsIntegers,
        ExecuteGreaterEqualsReals,
        ExecuteGreaterEqualsStrings,
        ExecuteOrIntegers,
        ExecuteAndIntegers,
        ExecuteNegateInteger,
        ExecuteNegateReal,
        ExecuteNotInteger,
        ExecuteIntegerToReal,
        ExecuteRealToInteger,
        ExecuteMove,
        ExecutePrintBoolean,
        ExecutePrintInteger,
        ExecutePrintReal,
        ExecutePrintString,
        ExecutePrintNewline,
        ExecuteInputInteger,
        ExecuteInputString,
        ExecuteFnLen,
        ExecuteFnLeft
    };
    // translate code into applicable function calls
    for (int ix = 0; ix < mCodeSize; ) {
        int count = getInstructionSize(mCode[ix]);
        mCode[ix] = (VmWord)translationTable[mCode[ix]];
        ix += count;
        assert(ix <= mCodeSize);
    }
}

Interpreter::~Interpreter()
{
    delete[] mStacks;
    delete[] mCode;
}

InterpreterResult Interpreter::run()
{
    ExecutionContext context;
    context.code = mCode;
    context.stacks = mStacks;
    context.stringManager = &mStringManager;
    context.stringStack = &mStringStack;
    context.typeManager = &mTypeManager;
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
