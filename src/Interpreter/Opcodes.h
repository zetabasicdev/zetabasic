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

enum Opcode : VmWord
{
    Op_nop,
    Op_end,
    Op_reserve,
    Op_init_mem,
    Op_free_mem,
    Op_read_mem,
    Op_write_mem,
    Op_del_str,
    Op_new_array,
    Op_del_array,
    Op_jmp,
    Op_jmpz,
    Op_jmpnz,
    Op_load_c,
    Op_load_st,
    Op_add_i,
    Op_add_r,
    Op_add_st,
    Op_sub_i,
    Op_sub_r,
    Op_mul_i,
    Op_mul_r,
    Op_div_i,
    Op_div_r,
    Op_mod_i,
    Op_mod_r,
    Op_eq_i,
    Op_eq_r,
    Op_eq_st,
    Op_neq_i,
    Op_neq_r,
    Op_neq_st,
    Op_lt_i,
    Op_lt_r,
    Op_lt_st,
    Op_gt_i,
    Op_gt_r,
    Op_gt_st,
    Op_lte_i,
    Op_lte_r,
    Op_lte_st,
    Op_gte_i,
    Op_gte_r,
    Op_gte_st,
    Op_or_i,
    Op_and_i,
    Op_neg_i,
    Op_neg_r,
    Op_not_i,
    Op_i2r,
    Op_r2i,
    Op_mov,
    Op_print_b,
    Op_print_i,
    Op_print_r,
    Op_print_st,
    Op_print_nl,
    Op_input_i,
    Op_input_st,
    Op_fn_len,
    Op_fn_left
};
