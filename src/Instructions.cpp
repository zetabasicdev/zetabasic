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

#include "Instructions.h"
#include "Program.h"
#include "Stack.h"
#include "StringManager.h"
#include "StringStack.h"
#include "SystemCalls.h"
#include "Window.h"

VmWord* ExecuteNop(ExecutionContext* context, VmWord* ip)
{
    // do nothing
    return ++ip;
}

VmWord* ExecuteEnd(ExecutionContext* context, VmWord* ip)
{
    // return NULL, signifying end of execution
    return nullptr;
}

VmWord* ExecuteReserve(ExecutionContext* context, VmWord* ip)
{
    context->stack->reserve(ip[1] & 0xff);  // todo
    return ip + 2;
}

VmWord* ExecuteDup(ExecutionContext* context, VmWord* ip)
{
    context->stack->dup();
    return ++ip;
}

VmWord* ExecuteLoadConstStr(ExecutionContext* context, VmWord* ip)
{
    context->stringStack->push(context->program->getString((int)ip[1]));
    return ip + 2;
}

VmWord* ExecuteLoadConst(ExecutionContext* context, VmWord* ip)
{
    context->stack->push(context->program->getIntegerConstant((int)ip[1]));
    return ip + 2;
}

VmWord* ExecuteLoadLocalStr(ExecutionContext* context, VmWord* ip)
{
    context->stringManager->loadString(context->stack->getLocal((int)ip[1]));
    return ip + 2;
}

VmWord* ExecuteLoadLocal(ExecutionContext* context, VmWord* ip)
{
    context->stack->push(context->stack->getLocal((int)ip[1]));
    return ip + 2;
}

VmWord* ExecuteStoreLocalStr(ExecutionContext* context, VmWord* ip)
{
    context->stack->setLocal((int)ip[1], context->stringManager->storeString(context->stack->getLocal((int)ip[1])));
    return ip + 2;
}

VmWord* ExecuteStoreLocal(ExecutionContext* context, VmWord* ip)
{
    context->stack->setLocal((int)ip[1], context->stack->pop());
    return ip + 2;
}

VmWord* ExecuteSyscall(ExecutionContext* context, VmWord* ip)
{
    switch ((int)ip[1]) {
    case Syscall_printstr:
    {
        StringPiece text = context->stringStack->pop();
        context->window->printn(text.getText(), text.getLength());
        break;
    }
    case Syscall_printi:
    {
        int64_t value = context->stack->pop();
        context->window->printf("%lld", value);
        break;
    }
    case Syscall_printnl:
        context->window->print("\n");
        break;
    case Syscall_inputi:
        context->stack->push((int64_t)atoi(context->window->input().c_str()));
        break;
    case Syscall_inputstr:
        context->stringStack->push(StringPiece(context->window->input()));
        break;
    case Syscall_len:
        context->stack->push((int64_t)context->stringStack->len());
        break;
    case Syscall_left_str:
        context->stringStack->left((int)context->stack->pop());
        break;
    default:
        assert(false);
    }
    return ip + 2;
}

VmWord* ExecuteAddStr(ExecutionContext* context, VmWord* ip)
{
    context->stringStack->add();
    return ++ip;
}

VmWord* ExecuteAddInt(ExecutionContext* context, VmWord* ip)
{
    int64_t rhs = context->stack->pop();
    int64_t lhs = context->stack->pop();
    context->stack->push(lhs + rhs);
    return ++ip;
}

VmWord* ExecuteEqStr(ExecutionContext* context, VmWord* ip)
{
    context->stack->push((context->stringStack->compare() == 0) ? 1 : 0);
    return ++ip;
}

VmWord* ExecuteEqInt(ExecutionContext* context, VmWord* ip)
{
    int64_t rhs = context->stack->pop();
    int64_t lhs = context->stack->pop();
    context->stack->push(lhs == rhs);
    return ++ip;
}

VmWord* ExecuteOrInt(ExecutionContext* context, VmWord* ip)
{
    int64_t rhs = context->stack->pop();
    int64_t lhs = context->stack->pop();
    context->stack->push(lhs | rhs);
    return ++ip;
}

VmWord* ExecuteJmp(ExecutionContext* context, VmWord* ip)
{
    return context->code + ip[1];
}

VmWord* ExecuteJmpZero(ExecutionContext* context, VmWord* ip)
{
    if (context->stack->pop() == 0)
        return context->code + ip[1];
    return ip + 2;
}

VmWord* ExecuteJmpNeq(ExecutionContext* context, VmWord* ip)
{
    int64_t rhs = context->stack->pop();
    int64_t lhs = context->stack->pop();
    if (lhs != rhs)
        return context->code + ip[1];
    return ip + 2;
}

VmWord* ExecuteJmpLt(ExecutionContext* context, VmWord* ip)
{
    int64_t rhs = context->stack->pop();
    int64_t lhs = context->stack->pop();
    if (lhs < rhs)
        return context->code + ip[1];
    return ip + 2;
}
