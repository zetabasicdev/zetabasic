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

#include <cstdint>

typedef uint64_t VmWord;

int getInstructionSize(VmWord word);

// instructions can take up to 4 stack-based operands, which all fit into a 64-bit
// word--this means maximum operand size is 16 bits (or 14 bits due to stack
// descriptor)
const int64_t OperandSizeMask = 0xffff;
const int64_t MaxOperandSize  = 0xffff;
const int64_t MaxOperandValue = 0x3fff;

const int64_t StackLocals = 0;
const int64_t StackTemporaries = 1;
const int64_t StackParmeters = 2;
const int64_t StackGlobals = 3;

const int64_t Operand0Shift = 0;
const int64_t Operand1Shift = 16;
const int64_t Operand2Shift = 32;
const int64_t Operand3Shift = 48;

// jump addresses always follow operand 0.
const int64_t JumpSizeMask = 0xffffffffffff;
const int64_t JumpShift = 16;
