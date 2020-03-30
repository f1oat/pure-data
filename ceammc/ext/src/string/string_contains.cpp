/*****************************************************************************
 * Copyright 2017 Serge Poltavsky. All rights reserved.
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
#include "string_contains.h"
#include "ceammc_factory.h"
#include "ceammc_format.h"
#include "ceammc_string.h"
#include "datatype_string.h"

StringContains::StringContains(const PdArgs& a)
    : BaseObject(a)
    , subj_(to_string(positionalArguments()))
{
    createInlet();
    createOutlet();
}

void StringContains::onSymbol(t_symbol* s)
{
    boolTo(0, string::contains(s->s_name, subj_.c_str()));
}

void StringContains::onDataT(const DataTypeString* str)
{
    boolTo(0, string::contains(str->str(), subj_));
}

void StringContains::onInlet(size_t, const AtomList& l)
{
    subj_ = to_string(l);
}

void setup_string_contains()
{
    ObjectFactory<StringContains> obj("string.contains");
    obj.processData<DataTypeString>();
    obj.addAlias("str.contains");
    obj.setCategory("string");
    obj.setKeywords({ "contains", "search" });
    obj.setDescription("checks if string contains specified substring");
}
