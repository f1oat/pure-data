/*****************************************************************************
 * Copyright 2020 Serge Poltavsky. All rights reserved.
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
#include "ceammc_property_enum.h"

namespace ceammc {

SymbolEnumProperty::SymbolEnumProperty(const std::string& name, t_symbol* def, PropValueAccess access)
    : EnumProperty<t_symbol*>(name, def, access)
{
}

SymbolEnumProperty::SymbolEnumProperty(const std::string& name, std::initializer_list<t_symbol*> values, PropValueAccess access)
    : EnumProperty(name, values, access)
{
}

SymbolEnumProperty::SymbolEnumProperty(const std::string& name, std::initializer_list<const char*> values, PropValueAccess access)
    : EnumProperty(name, (values.size() == 0) ? gensym("???") : gensym(*values.begin()), access)
{
    for (size_t i = 1; i < values.size(); i++)
        appendEnum(gensym(*(values.begin() + i)));
}

}
