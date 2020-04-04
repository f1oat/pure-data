/*****************************************************************************
 * Copyright 2018 Serge Poltavsky. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#include "test_datatypes.h"
#include "ceammc_datastorage.h"
#include "ceammc_factory.h"

#include <sstream>

static ceammc::AbstractData* newIntData(const ceammc::AtomList& args)
{
    if (args.isInteger())
        return new IntData(args.asT<int>());
    else {
        using namespace ceammc;
        LIB_ERR << "integer is expected, got: " << args;
        return nullptr;
    }
}

static ceammc::AbstractData* newStrData(const ceammc::AtomList& args)
{
    if (args.isSymbol())
        return new StrData(args.asT<t_symbol*>()->s_name);
    else {
        using namespace ceammc;
        LIB_ERR << "string is expected, got: " << args;
        return nullptr;
    }
}

int IntData::dataType = ceammc::DataStorage::instance().registerNewType("IntData", newIntData);
int IntData::constructor_called = 0;
int IntData::destructor_called = 0;

int StrData::dataType = ceammc::DataStorage::instance().registerNewType("StrData", newStrData);
int StrData::constructor_called = 0;
int StrData::destructor_called = 0;

IntData::IntData(int v) noexcept
    : v_(v)
{
    constructor_called++;
}

IntData::IntData(const IntData& i)
    : v_(i.v_)
{
    constructor_called++;
}

IntData::~IntData() noexcept
{
    destructor_called++;
}

int IntData::value() const noexcept { return v_; }

void IntData::setValue(int v) noexcept { v_ = v; }

bool IntData::isEqual(const ceammc::AbstractData* d) const noexcept
{
    const IntData* dt = d->as<IntData>();
    if (!dt)
        return false;

    return v_ == dt->v_;
}

bool IntData::isLess(const ceammc::AbstractData* d) const noexcept
{
    const IntData* dt = d->as<IntData>();
    if (!dt)
        return false;

    return v_ < dt->v_;
}

std::string IntData::toString() const
{
    std::ostringstream buf;
    buf << v_;
    return buf.str();
}

std::string IntData::valueToJsonString() const
{
    return toString();
}

int IntData::type() const noexcept { return dataType; }

IntData* IntData::clone() const { return new IntData(v_); }

class TestInt : public ceammc::BaseObject {
public:
    TestInt(const ceammc::PdArgs& a)
        : ceammc::BaseObject(a)
    {
        createOutlet();
    }

    void onFloat(t_float f)
    {
        atomTo(0, new IntData(f));
    }
};

void IntData::init()
{
    ceammc::ObjectFactory<TestInt> obj("test.int");
}

bool IntData::operator==(const IntData& d) const noexcept
{
    return v_ == d.v_;
}

StrData::StrData(const std::string& v)
    : v_(v)
{
    constructor_called++;
}

StrData::~StrData() noexcept
{
    destructor_called++;
}

const std::string& StrData::get() const noexcept { return v_; }

void StrData::setValue(const std::string& v) { v_ = v; }

bool StrData::isEqual(const ceammc::AbstractData* d) const noexcept
{
    const StrData* dt = d->as<StrData>();
    if (!dt)
        return false;

    return v_ == dt->v_;
}

std::string StrData::toString() const
{
    return v_;
}

int StrData::type() const noexcept { return dataType; }

StrData* StrData::clone() const { return new StrData(v_); }

bool StrData::operator==(const StrData& d) const noexcept
{
    return v_ == d.v_;
}
