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

#include "ConstantTable.h"
#include "ExpressionNode.h"
#include "Opcodes.h"
#include "StringTable.h"
#include "Symbol.h"
#include "Translator.h"

Translator::Translator(TItemBuffer<VmWord>& codeBuffer,
                       StringTable& stringTable,
                       ConstantTable& constantTable,
                       SymbolTable& symbolTable,
                       Node& root)
    :
    mCodeBuffer(codeBuffer),
    mStringTable(stringTable),
    mConstantTable(constantTable),
    mSymbolTable(symbolTable),
    mRoot(root),
    mReserveIndex(-1),
    mNamedLabels(),
    mLabelTargets(),
    mNextTemporary(0),
    mMaxTemporaries(0)
{
    // intentionally left blank
}

Translator::~Translator()
{
    // intentionally left blank
}

void Translator::run()
{
    mRoot.translate(*this);
    fixupLabels();
#ifdef DUMP_INTERNALS
    dumpCode();
#endif
}

void Translator::startCodeBody()
{
    assert(mReserveIndex == -1);
    auto code = mCodeBuffer.alloc(2, mReserveIndex);
    code[0] = Op_reserve;
    code[1] = mSymbolTable.getSize();
}

void Translator::endCodeBody()
{
    assert(mReserveIndex != -1);
    mCodeBuffer[mReserveIndex + 1] += mMaxTemporaries;
}

ResultIndex Translator::loadConstant(int64_t value)
{
    // for an integer constant, it can either be loaded directly into
    // a result index, or it must be loaded from the constant table
    // if the size is too large (or it is negative -- todo fix this?)
    if ((value & OperandSizeMask) == value)
        return ResultIndex(ResultIndexType::Literal, (int)value);

    int constantIndex = mConstantTable.addInteger(value);
    assert(constantIndex <= MaxOperandValue);

    int temporaryIndex = getTemporary();
    auto ops = mCodeBuffer.alloc(2);
    ops[0] = Op_load_constant;

    // temporary index is stored in first operand
    // constant index is stored in second operand
    ops[1] = ((uint64_t)constantIndex << Operand2Shift) | (uint64_t)temporaryIndex;

    return ResultIndex(ResultIndexType::Local, temporaryIndex);
}

ResultIndex Translator::loadStringConstant(const StringPiece& value)
{
    int constantIndex = mStringTable.addString(value);
    assert(constantIndex <= MaxOperandValue);

    if ((constantIndex & OperandSizeMask) == constantIndex)
        return ResultIndex(ResultIndexType::Literal, constantIndex);

    int temporaryIndex = getTemporary();
    auto ops = mCodeBuffer.alloc(2);
    ops[0] = Op_load_string;

    // temporary index is stored in first operand
    // constant index is stored in second operand
    ops[1] = ((uint64_t)constantIndex << Operand2Shift) | (uint64_t)temporaryIndex;

    return ResultIndex(ResultIndexType::Local, temporaryIndex);
}

ResultIndex Translator::loadIdentifier(Symbol* symbol)
{
    return ResultIndex(ResultIndexType::Local, symbol->getLocation());
}

ResultIndex Translator::binaryOperator(uint64_t baseOpcode, const ResultIndex& lhs, const ResultIndex& rhs)
{
    uint64_t op = baseOpcode;
    uint64_t opBase = op >> 8;
    if (lhs.getType() == ResultIndexType::Local && rhs.getType() == ResultIndexType::Literal)
        op = (op & 0xff) | ((opBase + 1) << 8);
    else if (lhs.getType() == ResultIndexType::Literal && rhs.getType() == ResultIndexType::Local)
        op = (op & 0xff) | ((opBase + 2) << 8);
    else if (lhs.getType() == ResultIndexType::Local && rhs.getType() == ResultIndexType::Local)
        op = (op & 0xff) | ((opBase + 3) << 8);

    int temporaryIndex = getTemporary();

    auto ops = mCodeBuffer.alloc(2);
    ops[0] = op;

    // target index is stored in first operand
    // lhs index is stored in second operand
    // rhs index is stored in third operand
    ops[1] = ((uint64_t)rhs.getValue() << Operand3Shift) | ((uint64_t)lhs.getValue() << Operand2Shift) | (uint64_t)temporaryIndex;

    return ResultIndex(ResultIndexType::Local, temporaryIndex);
}

void Translator::jump(const StringPiece& name)
{
    auto label = getLabelByName(name);
    jump(label);
}

void Translator::jump(Label label)
{
    assert(label >= 0 && label < (int)mLabelTargets.size());

    auto ops = mCodeBuffer.alloc(2);
    ops[0] = Op_jmp;
    ops[1] = (uint64_t)label << JumpShift;
}

void Translator::jump(uint64_t opcode, Label label, const ResultIndex& result)
{
    assert(opcode >= Op_jmp && opcode <= Op_jmp_not_zero);
    assert(label >= 0 && label < (int)mLabelTargets.size());

    auto ops = mCodeBuffer.alloc(2);
    ops[0] = opcode;
    ops[1] = ((uint64_t)label << JumpShift) | (uint64_t)result.getValue();
}

void Translator::assign(Symbol* target, const ResultIndex& index)
{
    if (target->getType() == Typename::String && index.getType() == ResultIndexType::Literal) {
        // special case for string literal (i.e., S$ = "Test")
        auto ops = mCodeBuffer.alloc(2);
        ops[0] = Op_load_string;
        ops[1] = ((uint64_t)index.getValue() << Operand2Shift) | (uint64_t)target->getLocation();
        return;
    }

    auto ops = mCodeBuffer.alloc(2);
    ops[0] = (index.getType() == ResultIndexType::Literal) ? Op_move0 : Op_move1;

    // target index is stored in first operand
    // source index is stored in second operand
    ops[1] = ((uint64_t)index.getValue() << Operand2Shift) | (uint64_t)target->getLocation();
}

ResultIndex Translator::ensureLocal(Typename type, const ResultIndex& index)
{
    // only do something if it is a literal
    if (index.getType() == ResultIndexType::Local)
        return index;

    int temporaryIndex = getTemporary();
    if (type == Typename::String) {
        // special case for string literal (i.e., S$ = "TEST")
        auto ops = mCodeBuffer.alloc(2);
        ops[0] = Op_load_string;
        ops[1] = ((uint64_t)index.getValue() << Operand2Shift) | (uint64_t)temporaryIndex;
        return ResultIndex(ResultIndexType::Local, temporaryIndex);
    }

    auto ops = mCodeBuffer.alloc(2);
    ops[0] = Op_move0;

    // target index is stored in first operand
    // source index is stored in second operand
    ops[1] = ((uint64_t)index.getValue() << Operand2Shift) | (uint64_t)temporaryIndex;
    return ResultIndex(ResultIndexType::Local, temporaryIndex);
}

void Translator::print(Typename type, const ResultIndex& index, bool trailingSemicolon)
{
    auto ops = mCodeBuffer.alloc(2);

    // target index/value is stored in first operand
    ops[1] = (uint64_t)index.getValue();
    switch (type) {
    case Typename::Integer:
        if (index.getType() == ResultIndexType::Literal)
            ops[0] = trailingSemicolon ? Op_print_integer0 : Op_print_integer0_newline;
        else if (index.getType() == ResultIndexType::Local)
            ops[0] = trailingSemicolon ? Op_print_integer1 : Op_print_integer1_newline;
        break;
    case Typename::String:
        if (index.getType() == ResultIndexType::Literal)
            ops[0] = trailingSemicolon ? Op_print_string0 : Op_print_string0_newline;
        else if (index.getType() == ResultIndexType::Local)
            ops[0] = trailingSemicolon ? Op_print_string1 : Op_print_string1_newline;
        break;
    default:
        break;
    }
}

void Translator::input(Symbol* target)
{
    auto ops = mCodeBuffer.alloc(2);

    switch (target->getType()) {
    case Typename::Integer:
        ops[0] = Op_input_integer;
        break;
    case Typename::String:
        ops[0] = Op_input_string;
        break;
    default:
        assert(false);
        break;
    }

    ops[1] = (uint64_t)target->getLocation();
}

void Translator::end()
{
    *mCodeBuffer.alloc(1) = Op_end;
}

ResultIndex Translator::builtInFunction(uint64_t opcode, TNodeList<ExpressionNode>& arguments)
{
    static std::vector<ResultIndex> indices;

    // ensure all argument indices are within temporaries (as opposed to literals)
    indices.clear();
    for (auto& arg : arguments)
        indices.push_back(ensureLocal(arg.getType(), arg.getResultIndex()));

    auto ops = mCodeBuffer.alloc(2);
    ops[0] = opcode;
    int temporaryIndex = getTemporary();
    switch (indices.size()) {
    case 1:
        ops[1] = ((uint64_t)indices[0].getValue() << Operand2Shift) | (uint64_t)temporaryIndex;
        break;
    case 2:
        ops[1] = ((uint64_t)indices[0].getValue() << Operand2Shift) |
            ((uint64_t)indices[1].getValue() << Operand3Shift) |
            (uint64_t)temporaryIndex;
        break;
    default:
        assert(false);
        break;
    }

    return ResultIndex(ResultIndexType::Local, temporaryIndex);
}

void Translator::placeLabel(const StringPiece& name)
{
    placeLabel(getLabelByName(name));
}

void Translator::placeLabel(Label label)
{
    assert(label >= 0 && label < (int)mLabelTargets.size());
    assert(mLabelTargets[label] == -1);
    mLabelTargets[label] = mCodeBuffer.getSize();
}

Label Translator::generateLabel()
{
    auto label = (Label)mLabelTargets.size();
    mLabelTargets.push_back(-1);
    return label;
}

void Translator::clearTemporaries()
{
    mNextTemporary = 0;
}

void Translator::fixupLabels()
{
    // loop through instructions, looking for jump statements and fix the targets accordingly.
    for (int ix = 0; ix < mCodeBuffer.getSize(); ) {
        uint8_t type = mCodeBuffer[ix] & 0xff;
        if (type == 0x02 || type == 0x03) {
            VmWord word = mCodeBuffer[ix + 1];
            Label label = (Label)((word >> JumpShift) & JumpSizeMask);
            assert(label >= 0 && label < (Label)mLabelTargets.size());
            word &= ~(JumpSizeMask << JumpShift);
            word |= ((uint64_t)mLabelTargets[label] << JumpShift);
            mCodeBuffer[ix + 1] = word;
        }
        ix += getInstructionSize(mCodeBuffer[ix]);
    }
}

int Translator::getTemporary()
{
    int temporary = mNextTemporary++;
    if (mNextTemporary > mMaxTemporaries)
        mMaxTemporaries = mNextTemporary;
    return mSymbolTable.getSize() + temporary;
}

Label Translator::getLabelByName(const StringPiece& name)
{
    std::string labelName = std::string(name.getText(), name.getLength());
    auto iter = mNamedLabels.find(labelName);
    if (iter == mNamedLabels.end()) {
        // instance a new label
        auto label = (int)mLabelTargets.size();
        mLabelTargets.push_back(-1);
        mNamedLabels[labelName] = label;
        return label;
    }
    return iter->second;
}


void Translator::dumpCode()
{
    static const char* instructions[] = {
        "nop", "end",
        "reserve",
        "jump", "jump_zero", "jump_not_zero",
        "load_constant", "load_string",
        "add_integers", "add_integers", "add_integers", "add_integers",
        "add_string", "add_string", "add_string", "add_string",
        "eq_integers", "eq_integers", "eq_integers", "eq_integers",
        "eq_string", "eq_string", "eq_string", "eq_string",
        "gr_integers", "gr_integers", "gr_integers", "gr_integers",
        "or_integers", "or_integers", "or_integers", "or_integers",
        "move", "move",
        "print_integer", "print_integer",
        "print_integer_newline", "print_integer_newline",
        "print_string", "print_string",
        "print_string_newline", "print_string_newline",
        "input_integer", "input_string",
        "fn_len", "fn_left"
    };

    for (int ix = 0; ix < mCodeBuffer.getSize(); ) {
        printf("%06X: [%03lld/%02llX] %s ", ix, mCodeBuffer[ix] >> 8, mCodeBuffer[ix] & 0xff, instructions[mCodeBuffer[ix] >> 8]);
        switch (mCodeBuffer[ix] & 0xff) {
        case 0x00:
            printf("\n");
            break;
        case 0x01:
            printf("%lld\n", mCodeBuffer[ix + 1]);
            break;
        case 0x02:
            printf("[%06llX]\n", mCodeBuffer[ix + 1] >> JumpShift);
            break;
        case 0x03:
            printf("#%lld, [%06llX]\n",
                   mCodeBuffer[ix + 1] & OperandSizeMask,
                   mCodeBuffer[ix + 1] >> JumpShift);
            break;
        case 0x04:
            printf("%lld\n", mCodeBuffer[ix + 1] & OperandSizeMask);
            break;
        case 0x05:
            printf("%lld, %lld\n",
                    mCodeBuffer[ix + 1] & OperandSizeMask,
                   (mCodeBuffer[ix + 1] >> Operand2Shift) & OperandSizeMask);
            break;
        case 0x06:
            printf("%lld, %lld, %lld\n",
                    mCodeBuffer[ix + 1] & OperandSizeMask,
                   (mCodeBuffer[ix + 1] >> Operand2Shift) & OperandSizeMask,
                   (mCodeBuffer[ix + 1] >> Operand3Shift) & OperandSizeMask);
            break;
        case 0x07:
            break;
        default:
            assert(false);
            break;
        }
        ix += getInstructionSize(mCodeBuffer[ix]);
    }
}
