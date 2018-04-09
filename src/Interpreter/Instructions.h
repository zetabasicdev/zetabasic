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
class TypeManager;
class Window;
struct ExecutionContext;

typedef VmWord*(*InstructionExecutor)(ExecutionContext* context, VmWord* ip);

struct ExecutionContext
{
    VmWord* code;
    Stack* stacks;
    StringManager* stringManager;
    StringStack* stringStack;
    TypeManager* typeManager;
    const Program* program;
    Window* window;
};

VmWord* ExecuteNop(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEnd(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteReserve(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteInitMem(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteFreeMem(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteReadMem(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteWriteMem(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteJmp(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteJmpZero(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteJmpNotZero(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteLoadConstant(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLoadString(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteAddIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddStrings(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteSubIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteSubReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteMulIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteMulReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteDivIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteDivReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteModIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteModReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqualIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqualReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqualStrings(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteNotEqualIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteNotEqualReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteNotEqualStrings(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLessIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLessReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLessStrings(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGreaterIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGreaterReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGreaterStrings(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLessEqualsIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLessEqualsReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLessEqualsStrings(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGreaterEqualsIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGreaterEqualsReals(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteGreaterEqualsStrings(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteOrIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAndIntegers(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteNegateInteger(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteNegateReal(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteNotInteger(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteIntegerToReal(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteRealToInteger(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteMove(ExecutionContext* context, VmWord* ip);

VmWord* ExecutePrintBoolean(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintInteger(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintReal(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintString(ExecutionContext* context, VmWord* ip);
VmWord* ExecutePrintNewline(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteInputInteger(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteInputString(ExecutionContext* context, VmWord* ip);

VmWord* ExecuteFnLen(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteFnLeft(ExecutionContext* context, VmWord* ip);
