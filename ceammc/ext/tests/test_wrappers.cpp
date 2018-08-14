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
#include "test_wrappers.h"

Atom test_atom_wrap(const char* v)
{
    return Atom(gensym(v));
}

Atom test_atom_wrap(t_symbol* v)
{
    return Atom(v);
}
Atom test_atom_wrap(float v)
{
    return Atom(v);
}
Atom test_atom_wrap(const Atom& v)
{
    return v;
}
Atom test_atom_wrap(const DataPtr& d)
{
    return d.asAtom();
}

Atom D(DataType t, DataId id)
{
    return Atom(DataDesc(t, id));
}
