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

#include "VirtualMachine.h"

class Program;
class Stack;
class StringManager;
class StringStack;
class Window;
struct ExecutionContext;

typedef VmWord*(*InstructionExecutor)(ExecutionContext* context, VmWord* ip);

struct ExecutionContext
{
    VmWord* code;
    Stack* stack;
    StringManager* stringManager;
    StringStack* stringStack;
    const Program* program;
    Window* window;
};

VmWord* ExecuteNop(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEnd(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteReserve(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteJmp(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteJmpZero(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteJmpNotZero(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteLoadConstant(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLoadString(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteAddIntegers0(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddIntegers1(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddIntegers2(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddIntegers3(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddReals3(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddStrings0(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddStrings1(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddStrings2(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddStrings3(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqIntegers0(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqIntegers1(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqIntegers2(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqIntegers3(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqReals3(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqStrings0(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqStrings1(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqStrings2(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqStrings3(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGrIntegers0(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGrIntegers1(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGrIntegers2(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGrIntegers3(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteOrIntegers0(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteOrIntegers1(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteOrIntegers2(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteOrIntegers3(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteIntToReal0(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteIntToReal1(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteRealToInt1(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteMove0(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteMove1(ExecutionContext* context, VmWord* ip);

VmWord* ExecutePrintBoolean0(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintBoolean1(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintBoolean0Newline(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintBoolean1Newline(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintInteger0(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintInteger1(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintInteger0Newline(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintInteger1Newline(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintReal0(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintReal1(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintReal0Newline(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintReal1Newline(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintString0(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintString1(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintString0Newline(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintString1Newline(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteInputInteger(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteInputString(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteFnLen(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteFnLeft(ExecutionContext* context, VmWord* ip);
