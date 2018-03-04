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
VmWord* ExecuteDup(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLoadConstStr(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLoadConst(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLoadLocalStr(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteLoadLocal(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteStoreLocalStr(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteStoreLocal(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteSyscall(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddStr(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteAddInt(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqStr(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteEqInt(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteOrInt(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteJmp(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteJmpZero(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteJmpNeq(ExecutionContext* context, VmWord* ip);
VmWord* ExecuteJmpLt(ExecutionContext* context, VmWord* ip);
