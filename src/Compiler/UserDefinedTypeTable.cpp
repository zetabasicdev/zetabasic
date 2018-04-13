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

#include "UserDefinedTypeTable.h"

UserDefinedTypeTable::UserDefinedTypeTable()
    :
    mUdtPool(),
    mUdtFieldPool(),
    mUdts(),
    mNextUdtId(Type_Udt)
{
    // intentionally left blank
}

UserDefinedTypeTable::~UserDefinedTypeTable()
{
    // intentionally left blank
}

void UserDefinedTypeTable::reset()
{
    mUdtPool.reset();
    mUdtFieldPool.reset();
    mUdts.clear();
    mNextUdtId = Type_Udt;
}

const UserDefinedType* UserDefinedTypeTable::findUdt(const StringPiece& name)
{
    for (auto udt : mUdts)
        if (udt->name == name)
            return udt;
    return nullptr;
}

const UserDefinedType* UserDefinedTypeTable::findUdt(int id)
{
    for (auto udt : mUdts)
        if (udt->id == id)
            return udt;
    return nullptr;
}

UserDefinedType* UserDefinedTypeTable::newUdt()
{
    assert(mNextUdtId < kMaxTypes);
    UserDefinedType* udt = mUdtPool.alloc(1);
    udt->id = mNextUdtId++;
    udt->fieldCount = 0;
    udt->size = 0;
    udt->fields = nullptr;
    mUdts.push_back(udt);
    return udt;
}

UserDefinedTypeField* UserDefinedTypeTable::newUdtField()
{
    UserDefinedTypeField* udtField = mUdtFieldPool.alloc(1);
    udtField->next = nullptr;
    udtField->offset = 0;
    udtField->type = Type_Unknown;
    return udtField;
}
