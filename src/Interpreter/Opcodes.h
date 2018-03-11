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
#include "VirtualMachine.h"

#define OP_NOARG(id)  (int(id) << 8) | 0x00
#define OP_ONEINT(id) (int(id) << 8) | 0x01
#define OP_JMP(id)    (int(id) << 8) | 0x02
#define OP_JMPARG(id) (int(id) << 8) | 0x03
#define OP_1ARG(id)   (int(id) << 8) | 0x04
#define OP_2ARGS(id)  (int(id) << 8) | 0x05
#define OP_3ARGS(id)  (int(id) << 8) | 0x06
#define OP_4ARGS(id)  (int(id) << 8) | 0x07

enum class Instruction
{
    Nop, End,
    Reserve,
    Jmp, JmpZero, JmpNotZero,
    LoadConstant, LoadString,
    AddIntegers0, AddIntegers1, AddIntegers2, AddIntegers3,
    AddStrings0, AddStrings1, AddStrings2, AddStrings3,
    EqIntegers0, EqIntegers1, EqIntegers2, EqIntegers3,
    EqStrings0, EqStrings1, EqStrings2, EqStrings3,
    GrIntegers0, GrIntegers1, GrIntegers2, GrIntegers3,
    OrIntegers0, OrIntegers1, OrIntegers2, OrIntegers3,
    Move0, Move1,
    PrintInteger0, PrintInteger1,
    PrintIntegerNewline0, PrintIntegerNewline1,
    PrintString0, PrintString1,
    PrintStringNewline0, PrintStringNewline1,
    InputInteger,
    InputString,
    FnLen, FnLeft
};

enum Opcode : VmWord
{
    Op_nop = OP_NOARG(Instruction::Nop),
    Op_end = OP_NOARG(Instruction::End),

    Op_reserve = OP_ONEINT(Instruction::Reserve),
    
    Op_jmp = OP_JMP(Instruction::Jmp),
    Op_jmp_zero = OP_JMPARG(Instruction::JmpZero),
    Op_jmp_not_zero = OP_JMPARG(Instruction::JmpNotZero),
    
    Op_load_constant = OP_2ARGS(Instruction::LoadConstant),
    Op_load_string = OP_2ARGS(Instruction::LoadString),

    Op_add_integers0 = OP_3ARGS(Instruction::AddIntegers0),
    Op_add_integers1 = OP_3ARGS(Instruction::AddIntegers1),
    Op_add_integers2 = OP_3ARGS(Instruction::AddIntegers2),
    Op_add_integers3 = OP_3ARGS(Instruction::AddIntegers3),
    Op_add_strings0 = OP_3ARGS(Instruction::AddStrings0),
    Op_add_strings1 = OP_3ARGS(Instruction::AddStrings1),
    Op_add_strings2 = OP_3ARGS(Instruction::AddStrings2),
    Op_add_strings3 = OP_3ARGS(Instruction::AddStrings3),
    Op_eq_integers0 = OP_3ARGS(Instruction::EqIntegers0),
    Op_eq_integers1 = OP_3ARGS(Instruction::EqIntegers1),
    Op_eq_integers2 = OP_3ARGS(Instruction::EqIntegers2),
    Op_eq_integers3 = OP_3ARGS(Instruction::EqIntegers3),
    Op_eq_strings0 = OP_3ARGS(Instruction::EqStrings0),
    Op_eq_strings1 = OP_3ARGS(Instruction::EqStrings1),
    Op_eq_strings2 = OP_3ARGS(Instruction::EqStrings2),
    Op_eq_strings3 = OP_3ARGS(Instruction::EqStrings3),
    Op_gr_integers0 = OP_3ARGS(Instruction::GrIntegers0),
    Op_gr_integers1 = OP_3ARGS(Instruction::GrIntegers1),
    Op_gr_integers2 = OP_3ARGS(Instruction::GrIntegers2),
    Op_gr_integers3 = OP_3ARGS(Instruction::GrIntegers3),
    Op_or_integers0 = OP_3ARGS(Instruction::OrIntegers0),
    Op_or_integers1 = OP_3ARGS(Instruction::OrIntegers1),
    Op_or_integers2 = OP_3ARGS(Instruction::OrIntegers2),
    Op_or_integers3 = OP_3ARGS(Instruction::OrIntegers3),

    Op_move0 = OP_2ARGS(Instruction::Move0),
    Op_move1 = OP_2ARGS(Instruction::Move1),

    Op_print_integer0 = OP_1ARG(Instruction::PrintInteger0),
    Op_print_integer1 = OP_1ARG(Instruction::PrintInteger1),
    Op_print_integer0_newline = OP_1ARG(Instruction::PrintIntegerNewline0),
    Op_print_integer1_newline = OP_1ARG(Instruction::PrintIntegerNewline1),
    Op_print_string0 = OP_1ARG(Instruction::PrintString0),
    Op_print_string1 = OP_1ARG(Instruction::PrintString1),
    Op_print_string0_newline = OP_1ARG(Instruction::PrintStringNewline0),
    Op_print_string1_newline = OP_1ARG(Instruction::PrintStringNewline1),

    Op_input_integer = OP_1ARG(Instruction::InputInteger),
    Op_input_string = OP_1ARG(Instruction::InputString),

    Op_fn_len = OP_2ARGS(Instruction::FnLen),
    Op_fn_left = OP_3ARGS(Instruction::FnLeft)
};
