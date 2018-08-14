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
#ifndef LIST_SET_H
#define LIST_SET_H

#include "ceammc_object.h"

using namespace ceammc;

class DataTypeMList;

class ListSet : public BaseObject {
    t_float idx_;
    DataAtom value_;

public:
    ListSet(const PdArgs& args);

    void onList(const AtomList&) override;
    void onInlet(size_t n, const AtomList& lst) override;
    void onDataT(const DataTypeMList& lst);

public:
    t_float index() const;
    const DataAtom& value() const;
};

void setup_list_set();

#endif // LIST_SET_H
