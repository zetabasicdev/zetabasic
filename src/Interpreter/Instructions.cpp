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
    context->stack->reserve((int64_t)ip[1]);
    return ip + 2;
}

VmWord* ExecuteJmp(ExecutionContext* context, VmWord* ip)
{
    uint64_t target = (ip[1] >> JumpShift) & JumpSizeMask;
    assert(target < (uint64_t)context->program->getCodeSize());
    return context->code + target;
}

VmWord* ExecuteJmpZero(ExecutionContext* context, VmWord* ip)
{
    uint64_t target = (ip[1] >> JumpShift) & JumpSizeMask;
    assert(target < (uint64_t)context->program->getCodeSize());

    int64_t value = context->stack->getLocal((int)(ip[1] & OperandSizeMask));
    if (value == 0)
        return context->code + target;
    return ip + 2;
}

VmWord* ExecuteJmpNotZero(ExecutionContext* context, VmWord* ip)
{
    uint64_t target = (ip[1] >> JumpShift) & JumpSizeMask;
    assert(target < (uint64_t)context->program->getCodeSize());

    int64_t value = context->stack->getLocal((int)(ip[1] & OperandSizeMask));
    if (value != 0)
        return context->code + target;
    return ip + 2;
}

VmWord* ExecuteLoadConstant(ExecutionContext* context, VmWord* ip)
{
    context->stack->setLocal((int)(ip[1] & OperandSizeMask),
                             context->program->getIntegerConstant((int)((ip[1] >> Operand2Shift) & OperandSizeMask)));
    return ip + 2;
}

VmWord* ExecuteLoadString(ExecutionContext* context, VmWord* ip)
{
    context->stack->setLocal((int)(ip[1] & OperandSizeMask),
                             context->stringManager->newString(context->program->getString((int)((ip[1] >> Operand2Shift) & OperandSizeMask))));
    return ip + 2;
}

VmWord* ExecuteAddIntegers0(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = (int64_t)((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = (int64_t)((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs + rhs);
    return ip + 2;
}

VmWord* ExecuteAddIntegers1(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = (int64_t)((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs + rhs);
    return ip + 2;
}

VmWord* ExecuteAddIntegers2(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = (int64_t)((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = context->stack->getLocal((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs + rhs);
    return ip + 2;
}

VmWord* ExecuteAddIntegers3(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = context->stack->getLocal((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs + rhs);
    return ip + 2;
}

VmWord* ExecuteAddStrings0(ExecutionContext* context, VmWord* ip)
{
    auto& left = context->program->getString((int)((ip[1] >> Operand2Shift) & OperandSizeMask));
    auto& right = context->program->getString((int)((ip[1] >> Operand3Shift) & OperandSizeMask));
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), context->stringManager->addStrings(left.getText(), left.getLength(), right.getText(), right.getLength()));
    return ip + 2;
}

VmWord* ExecuteAddStrings1(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((int)((ip[1] >> Operand2Shift) & OperandSizeMask));
    const char* left = nullptr;
    int leftLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    auto& right = context->program->getString((int)((ip[1] >> Operand3Shift) & OperandSizeMask));
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), context->stringManager->addStrings(left, leftLen, right.getText(), right.getLength()));
    return ip + 2;
}

VmWord* ExecuteAddStrings2(ExecutionContext* context, VmWord* ip)
{
    auto& left = context->program->getString((int)((ip[1] >> Operand2Shift) & OperandSizeMask));
    int64_t rhs = context->stack->getLocal((int)((ip[1] >> Operand3Shift) & OperandSizeMask));
    const char* right = nullptr;
    int rightLen = 0;
    context->stringManager->getString(rhs, right, rightLen);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), context->stringManager->addStrings(left.getText(), left.getLength(), right, rightLen));
    return ip + 2;
}

VmWord* ExecuteAddStrings3(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((int)((ip[1] >> Operand2Shift) & OperandSizeMask));
    const char* left = nullptr;
    int leftLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    int64_t rhs = context->stack->getLocal((int)((ip[1] >> Operand3Shift) & OperandSizeMask));
    const char* right = nullptr;
    int rightLen = 0;
    context->stringManager->getString(rhs, right, rightLen);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), context->stringManager->addStrings(left, leftLen, right, rightLen));
    return ip + 2;
}

VmWord* ExecuteGrIntegers0(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = (int64_t)((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = (int64_t)((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs > rhs);
    return ip + 2;
}

VmWord* ExecuteGrIntegers1(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = (int64_t)((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs > rhs);
    return ip + 2;
}

VmWord* ExecuteGrIntegers2(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = (int64_t)((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = context->stack->getLocal((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs > rhs);
    return ip + 2;
}

VmWord* ExecuteGrIntegers3(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = context->stack->getLocal((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs > rhs);
    return ip + 2;
}

VmWord* ExecuteEqIntegers0(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = (int64_t)((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = (int64_t)((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs == rhs);
    return ip + 2;
}

VmWord* ExecuteEqIntegers1(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = (int64_t)((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs == rhs);
    return ip + 2;
}

VmWord* ExecuteEqIntegers2(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = (int64_t)((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = context->stack->getLocal((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs == rhs);
    return ip + 2;
}

VmWord* ExecuteEqIntegers3(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = context->stack->getLocal((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs == rhs);
    return ip + 2;
}

VmWord* ExecuteEqStrings0(ExecutionContext* context, VmWord* ip)
{
    auto& left = context->program->getString((int)((ip[1] >> Operand2Shift) & OperandSizeMask));
    auto& right = context->program->getString((int)((ip[1] >> Operand3Shift) & OperandSizeMask));
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), left.exactCompareWithCase(right));
    return ip + 2;
}

VmWord* ExecuteEqStrings1(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((int)((ip[1] >> Operand2Shift) & OperandSizeMask));
    const char* left = nullptr;
    int leftLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    auto& right = context->program->getString((int)((ip[1] >> Operand3Shift) & OperandSizeMask));
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), StringPiece(left, leftLen).exactCompareWithCase(right));
    return ip + 2;
}

VmWord* ExecuteEqStrings2(ExecutionContext* context, VmWord* ip)
{
    auto& left = context->program->getString((int)((ip[1] >> Operand2Shift) & OperandSizeMask));
    int64_t rhs = context->stack->getLocal((int)((ip[1] >> Operand3Shift) & OperandSizeMask));
    const char* right = nullptr;
    int rightLen = 0;
    context->stringManager->getString(rhs, right, rightLen);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), left.exactCompareWithCase(StringPiece(right, rightLen)));
    return ip + 2;
}

VmWord* ExecuteEqStrings3(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((int)((ip[1] >> Operand2Shift) & OperandSizeMask));
    const char* left = nullptr;
    int leftLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    int64_t rhs = context->stack->getLocal((int)((ip[1] >> Operand3Shift) & OperandSizeMask));
    const char* right = nullptr;
    int rightLen = 0;
    context->stringManager->getString(rhs, right, rightLen);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), StringPiece(left, leftLen).exactCompareWithCase(StringPiece(right, rightLen)));
    return ip + 2;
}

VmWord* ExecuteOrIntegers0(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = (int64_t)((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = (int64_t)((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs | rhs);
    return ip + 2;
}

VmWord* ExecuteOrIntegers1(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = (int64_t)((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs | rhs);
    return ip + 2;
}

VmWord* ExecuteOrIntegers2(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = (int64_t)((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = context->stack->getLocal((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs | rhs);
    return ip + 2;
}

VmWord* ExecuteOrIntegers3(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = context->stack->getLocal((ip[1] >> Operand2Shift) & OperandSizeMask);
    int64_t rhs = context->stack->getLocal((ip[1] >> Operand3Shift) & OperandSizeMask);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), lhs | rhs);
    return ip + 2;
}

VmWord* ExecuteMove0(ExecutionContext* context, VmWord* ip)
{
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), (int64_t)((ip[1] >> Operand2Shift) & OperandSizeMask));
    return ip + 2;
}

VmWord* ExecuteMove1(ExecutionContext* context, VmWord* ip)
{
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), context->stack->getLocal((int)((ip[1] >> Operand2Shift) & OperandSizeMask)));
    return ip + 2;
}

VmWord* ExecutePrintBoolean0(ExecutionContext* context, VmWord* ip)
{
    context->window->print(((int64_t)(ip[1] & OperandSizeMask) != 0) ? "True" : "False");
    return ip + 2;
}

VmWord* ExecutePrintBoolean1(ExecutionContext* context, VmWord* ip)
{
    int index = ip[1] & OperandSizeMask;
    context->window->print((context->stack->getLocal(index) != 0) ? "True" : "False");
    return ip + 2;
}

VmWord* ExecutePrintBoolean0Newline(ExecutionContext* context, VmWord* ip)
{
    context->window->print(((int64_t)(ip[1] & OperandSizeMask) != 0) ? "True\n" : "False\n");
    return ip + 2;
}

VmWord* ExecutePrintBoolean1Newline(ExecutionContext* context, VmWord* ip)
{
    int index = ip[1] & OperandSizeMask;
    context->window->print((context->stack->getLocal(index) != 0) ? "True\n" : "False\n");
    return ip + 2;
}


VmWord* ExecutePrintInteger0(ExecutionContext* context, VmWord* ip)
{
    context->window->printf("%lld", (int64_t)(ip[1] & OperandSizeMask));
    return ip + 2;
}

VmWord* ExecutePrintInteger1(ExecutionContext* context, VmWord* ip)
{
    int index = ip[1] & OperandSizeMask;
    context->window->printf("%lld", context->stack->getLocal(index));
    return ip + 2;
}

VmWord* ExecutePrintInteger0Newline(ExecutionContext* context, VmWord* ip)
{
    context->window->printf("%lld\n", (int64_t)(ip[1] & OperandSizeMask));
    return ip + 2;
}

VmWord* ExecutePrintInteger1Newline(ExecutionContext* context, VmWord* ip)
{
    int index = ip[1] & OperandSizeMask;
    context->window->printf("%lld\n", context->stack->getLocal(index));
    return ip + 2;
}

VmWord* ExecutePrintString0(ExecutionContext* context, VmWord* ip)
{
    context->window->printf("%s", context->program->getString((int)(ip[1] & OperandSizeMask)).getText());
    return ip + 2;
}

VmWord* ExecutePrintString1(ExecutionContext* context, VmWord* ip)
{
    const char* text = nullptr;
    int length = 0;
    context->stringManager->getString(context->stack->getLocal((int)(ip[1] & OperandSizeMask)), text, length);
    context->window->printn(text, length);
    return ip + 2;
}

VmWord* ExecutePrintString0Newline(ExecutionContext* context, VmWord* ip)
{
    context->window->printf("%s\n", context->program->getString((int)(ip[1] & OperandSizeMask)).getText());
    return ip + 2;
}

VmWord* ExecutePrintString1Newline(ExecutionContext* context, VmWord* ip)
{
    const char* text = nullptr;
    int length = 0;
    context->stringManager->getString(context->stack->getLocal((int)(ip[1] & OperandSizeMask)), text, length);
    context->window->printn(text, length);
    context->window->print("\n");
    return ip + 2;
}

VmWord* ExecuteInputInteger(ExecutionContext* context, VmWord* ip)
{
    const std::string& text = context->window->input();
    int64_t value = atoll(text.c_str());
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), value);
    return ip + 2;
}

VmWord* ExecuteInputString(ExecutionContext* context, VmWord* ip)
{
    const std::string& text = context->window->input();
    int64_t value = context->stringManager->newString(StringPiece(text.data(), (int)text.length()));
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), value);
    return ip + 2;
}

VmWord* ExecuteFnLen(ExecutionContext* context, VmWord* ip)
{
    const char* text = nullptr;
    int length = 0;
    context->stringManager->getString(context->stack->getLocal((int)((ip[1] >> Operand2Shift) & OperandSizeMask)), text, length);
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), (int64_t)length);
    return ip + 2;
}

VmWord* ExecuteFnLeft(ExecutionContext* context, VmWord* ip)
{
    const char* text = nullptr;
    int length = 0;
    context->stringManager->getString(context->stack->getLocal((int)((ip[1] >> Operand2Shift) & OperandSizeMask)), text, length);
    int64_t newLength = context->stack->getLocal((int)((ip[1] >> Operand3Shift) & OperandSizeMask));
    
    if (newLength > length)
        newLength = length;
    if (newLength < 0)
        newLength = 0;

    int64_t newString = context->stringManager->newString(StringPiece(text, (int)newLength));
    context->stack->setLocal((int)(ip[1] & OperandSizeMask), newString);

    return ip + 2;
}
