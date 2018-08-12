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
#ifndef LIST_INSERT_H
#define LIST_INSERT_H

#include "ceammc_dataatomlist.h"
#include "ceammc_object.h"

using namespace ceammc;

class DataTypeMList;

class ListInsert : public BaseObject {
    DataAtomList lst_;
    SizeTProperty* index_;

public:
    ListInsert(const PdArgs& args);

    void onList(const AtomList& lst) override;
    void onInlet(size_t n, const AtomList& lst) override;
    void onDataT(const DataTypeMList& lst);
};

void setup_list_insert();

#endif // LIST_INSERT_H
