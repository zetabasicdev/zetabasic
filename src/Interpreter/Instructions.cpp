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
#include "TypeManager.h"
#include "Window.h"

static inline int64_t getStackValue0(ExecutionContext* context, VmWord* ip)
{
    int stackIndex = ip[1] & 0x3;
    int stackOffset = int((ip[1] & OperandSizeMask) >> 2);
    return context->stacks[stackIndex].getLocal(stackOffset);
}

static inline int64_t getStackValue1(ExecutionContext* context, VmWord* ip)
{
    int stackIndex = (ip[1] >> Operand1Shift) & 0x3;
    int stackOffset = int(((ip[1] >> Operand1Shift) & OperandSizeMask) >> 2);
    return context->stacks[stackIndex].getLocal(stackOffset);
}

static inline int64_t getStackValue2(ExecutionContext* context, VmWord* ip)
{
    int stackIndex = (ip[1] >> Operand2Shift) & 0x3;
    int stackOffset = int(((ip[1] >> Operand2Shift) & OperandSizeMask) >> 2);
    return context->stacks[stackIndex].getLocal(stackOffset);
}

static inline int64_t getStackValue3(ExecutionContext* context, VmWord* ip)
{
    int stackIndex = (ip[1] >> Operand3Shift) & 0x3;
    int stackOffset = int(((ip[1] >> Operand3Shift) & OperandSizeMask) >> 2);
    return context->stacks[stackIndex].getLocal(stackOffset);
}

static inline void setStackValue0(ExecutionContext* context, VmWord* ip, int64_t value)
{
    int stackIndex = ip[1] & 0x3;
    int stackOffset = int((ip[1] & OperandSizeMask) >> 2);
    context->stacks[stackIndex].setLocal(stackOffset, value);
}

VmWord* ExecuteNop(ExecutionContext* context, VmWord* ip)
{
    // do nothing
    return ++ip;
}

VmWord* ExecuteEnd(ExecutionContext* context, VmWord* ip)
{
    // return nothing, signifying end of execution
    return nullptr;
}

VmWord* ExecuteReserve(ExecutionContext* context, VmWord* ip)
{
    context->stacks[StackLocals].reserve(ip[1] & OperandSizeMask);
    context->stacks[StackTemporaries].reserve((ip[1] >> Operand1Shift) & OperandSizeMask);
    return ip + 2;
}

VmWord* ExecuteInitMem(ExecutionContext* context, VmWord* ip)
{
    int size = (int)((ip[1] >> MemShift) & MemSizeMask);
    setStackValue0(context, ip, context->typeManager->newInstance(size));
    return ip + 2;
}

VmWord* ExecuteFreeMem(ExecutionContext* context, VmWord* ip)
{
    context->typeManager->delInstance(getStackValue0(context, ip));
    return ip + 2;
}

VmWord* ExecuteReadMem(ExecutionContext* context, VmWord* ip)
{
    int offset = (int)((ip[1] >> MemShift) & MemSizeMask);
    setStackValue0(context, ip, context->typeManager->readData(getStackValue1(context, ip), offset));
    return ip + 2;
}

VmWord* ExecuteWriteMem(ExecutionContext* context, VmWord* ip)
{
    int offset = (int)((ip[1] >> MemShift) & MemSizeMask);
    context->typeManager->writeData(getStackValue0(context, ip), getStackValue1(context, ip), offset);
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

    int64_t value = getStackValue0(context, ip);
    if (value == 0)
        return context->code + target;
    return ip + 2;
}

VmWord* ExecuteJmpNotZero(ExecutionContext* context, VmWord* ip)
{
    uint64_t target = (ip[1] >> JumpShift) & JumpSizeMask;
    assert(target < (uint64_t)context->program->getCodeSize());

    int64_t value = getStackValue0(context, ip);
    if (value != 0)
        return context->code + target;
    return ip + 2;
}

VmWord* ExecuteLoadConstant(ExecutionContext* context, VmWord* ip)
{
    setStackValue0(context, ip, context->program->getIntegerConstant((int)((ip[1] >> Operand1Shift) & OperandSizeMask)));
    return ip + 2;
}

VmWord* ExecuteLoadString(ExecutionContext* context, VmWord* ip)
{
    setStackValue0(context, ip, context->stringManager->newString(context->program->getString((int)((ip[1] >> Operand1Shift) & OperandSizeMask))));
    return ip + 2;
}

VmWord* ExecuteAddIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs + rhs);
    return ip + 2;
}

VmWord* ExecuteAddReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    double value = lhs + rhs;
    int64_t ivalue = *(int64_t*)&value;
    setStackValue0(context, ip, ivalue);
    return ip + 2;
}

VmWord* ExecuteAddStrings(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    const char* left = nullptr;
    const char* right = nullptr;
    int leftLen = 0;
    int rightLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    context->stringManager->getString(rhs, right, rightLen);
    setStackValue0(context, ip, context->stringManager->addStrings(left, leftLen, right, rightLen));
    return ip + 2;
}

VmWord* ExecuteSubIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs - rhs);
    return ip + 2;
}

VmWord* ExecuteSubReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    double value = lhs - rhs;
    int64_t ivalue = *(int64_t*)&value;
    setStackValue0(context, ip, ivalue);
    return ip + 2;
}

VmWord* ExecuteMulIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs * rhs);
    return ip + 2;
}

VmWord* ExecuteMulReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    double value = lhs * rhs;
    int64_t ivalue = *(int64_t*)&value;
    setStackValue0(context, ip, ivalue);
    return ip + 2;
}

VmWord* ExecuteDivIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs / rhs);
    return ip + 2;
}

VmWord* ExecuteDivReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    double value = lhs / rhs;
    int64_t ivalue = *(int64_t*)&value;
    setStackValue0(context, ip, ivalue);
    return ip + 2;
}

VmWord* ExecuteModIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs % rhs);
    return ip + 2;
}

VmWord* ExecuteModReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    double value = fmod(lhs, rhs);
    int64_t ivalue = *(int64_t*)&value;
    setStackValue0(context, ip, ivalue);
    return ip + 2;
}

VmWord* ExecuteEqualIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs == rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteEqualReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    setStackValue0(context, ip, lhs == rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteEqualStrings(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    const char* left = nullptr;
    const char* right = nullptr;
    int leftLen = 0;
    int rightLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    context->stringManager->getString(rhs, right, rightLen);
    bool result = StringPiece(left, leftLen).exactCompareWithCase(StringPiece(right, rightLen));
    setStackValue0(context, ip, result ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteNotEqualIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs != rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteNotEqualReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    setStackValue0(context, ip, lhs != rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteNotEqualStrings(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    const char* left = nullptr;
    const char* right = nullptr;
    int leftLen = 0;
    int rightLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    context->stringManager->getString(rhs, right, rightLen);
    int result = StringPiece(left, leftLen).exactCompareWithCaseInt(StringPiece(right, rightLen));
    setStackValue0(context, ip, result != 0 ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteLessIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs < rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteLessReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    setStackValue0(context, ip, lhs < rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteLessStrings(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    const char* left = nullptr;
    const char* right = nullptr;
    int leftLen = 0;
    int rightLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    context->stringManager->getString(rhs, right, rightLen);
    int result = StringPiece(left, leftLen).exactCompareWithCaseInt(StringPiece(right, rightLen));
    setStackValue0(context, ip, result < 0 ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteGreaterIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs > rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteGreaterReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    setStackValue0(context, ip, lhs > rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteGreaterStrings(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    const char* left = nullptr;
    const char* right = nullptr;
    int leftLen = 0;
    int rightLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    context->stringManager->getString(rhs, right, rightLen);
    int result = StringPiece(left, leftLen).exactCompareWithCaseInt(StringPiece(right, rightLen));
    setStackValue0(context, ip, result > 0 ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteLessEqualsIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs <= rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteLessEqualsReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    setStackValue0(context, ip, lhs <= rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteLessEqualsStrings(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    const char* left = nullptr;
    const char* right = nullptr;
    int leftLen = 0;
    int rightLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    context->stringManager->getString(rhs, right, rightLen);
    int result = StringPiece(left, leftLen).exactCompareWithCaseInt(StringPiece(right, rightLen));
    setStackValue0(context, ip, result <= 0 ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteGreaterEqualsIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs >= rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteGreaterEqualsReals(ExecutionContext* context, VmWord* ip)
{
    int64_t ilhs = getStackValue1(context, ip);
    int64_t irhs = getStackValue2(context, ip);
    double lhs = *(double*)&ilhs;
    double rhs = *(double*)&irhs;
    setStackValue0(context, ip, lhs >= rhs ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteGreaterEqualsStrings(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    const char* left = nullptr;
    const char* right = nullptr;
    int leftLen = 0;
    int rightLen = 0;
    context->stringManager->getString(lhs, left, leftLen);
    context->stringManager->getString(rhs, right, rightLen);
    int result = StringPiece(left, leftLen).exactCompareWithCaseInt(StringPiece(right, rightLen));
    setStackValue0(context, ip, result >= 0 ? 1 : 0);
    return ip + 2;
}

VmWord* ExecuteOrIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs | rhs);
    return ip + 2;
}

VmWord* ExecuteAndIntegers(ExecutionContext* context, VmWord* ip)
{
    int64_t lhs = getStackValue1(context, ip);
    int64_t rhs = getStackValue2(context, ip);
    setStackValue0(context, ip, lhs & rhs);
    return ip + 2;
}

VmWord* ExecuteNegateInteger(ExecutionContext* context, VmWord* ip)
{
    int64_t rhs = getStackValue1(context, ip);
    setStackValue0(context, ip, -rhs);
    return ip + 2;
}

VmWord* ExecuteNegateReal(ExecutionContext* context, VmWord* ip)
{
    int64_t irhs = getStackValue1(context, ip);
    double rhs = *(double*)&irhs;
    rhs = -rhs;
    int64_t ivalue = *(int64_t*)&rhs;
    setStackValue0(context, ip, ivalue);
    return ip + 2;
}

VmWord* ExecuteNotInteger(ExecutionContext* context, VmWord* ip)
{
    int64_t rhs = getStackValue1(context, ip);
    setStackValue0(context, ip, ~rhs);
    return ip + 2;
}

VmWord* ExecuteIntegerToReal(ExecutionContext* context, VmWord* ip)
{
    int64_t rhs = getStackValue1(context, ip);
    double value = double(rhs);
    int64_t ivalue = *(int64_t*)&value;
    setStackValue0(context, ip, ivalue);
    return ip + 2;
}

VmWord* ExecuteRealToInteger(ExecutionContext* context, VmWord* ip)
{
    int64_t irhs = getStackValue1(context, ip);
    double rhs = *(double*)&irhs;
    setStackValue0(context, ip, int64_t(rhs));
    return ip + 2;
}

VmWord* ExecuteMove(ExecutionContext* context, VmWord* ip)
{
    setStackValue0(context, ip, getStackValue1(context, ip));
    return ip + 2;
}

VmWord* ExecutePrintBoolean(ExecutionContext* context, VmWord* ip)
{
    int64_t value = getStackValue0(context, ip);
    context->window->print(value == 1 ? "True" : "False");
    return ip + 2;
}

VmWord* ExecutePrintInteger(ExecutionContext* context, VmWord* ip)
{
    int64_t value = getStackValue0(context, ip);
    context->window->printf("%lld", value);
    return ip + 2;
}

VmWord* ExecutePrintReal(ExecutionContext* context, VmWord* ip)
{
    int64_t ivalue = getStackValue0(context, ip);
    double value = *(double*)&ivalue;
    context->window->printf("%f", value);
    return ip + 2;
}

VmWord* ExecutePrintString(ExecutionContext* context, VmWord* ip)
{
    int64_t value = getStackValue0(context, ip);
    const char* text = nullptr;
    int textLen = 0;
    context->stringManager->getString(value, text, textLen);
    context->window->printn(text, textLen);
    return ip + 2;
}

VmWord* ExecutePrintNewline(ExecutionContext* context, VmWord* ip)
{
    context->window->printn("\n", 1);
    return ++ip;
}

VmWord* ExecuteInputInteger(ExecutionContext* context, VmWord* ip)
{
    const std::string& text = context->window->input();
    setStackValue0(context, ip, atoll(text.c_str()));
    return ip + 2;
}

VmWord* ExecuteInputString(ExecutionContext* context, VmWord* ip)
{
    const std::string& text = context->window->input();
    setStackValue0(context, ip, context->stringManager->newString(StringPiece(text.data(), (int)text.length())));
    return ip + 2;
}

VmWord* ExecuteFnLen(ExecutionContext* context, VmWord* ip)
{
    const char* text = nullptr;
    int length = 0;
    context->stringManager->getString(getStackValue1(context, ip), text, length);
    setStackValue0(context, ip, (int64_t)length);
    return ip + 2;
}

VmWord* ExecuteFnLeft(ExecutionContext* context, VmWord* ip)
{
    const char* text = nullptr;
    int length = 0;
    context->stringManager->getString(getStackValue1(context, ip), text, length);
    int64_t newLength = getStackValue2(context, ip);
    
    if (newLength > length)
        newLength = length;
    if (newLength < 0)
        newLength = 0;

    int64_t newString = context->stringManager->newString(StringPiece(text, (int)newLength));
    setStackValue0(context, ip, newString);

    return ip + 2;
}
