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
#include "list_insert.h"
#include "ceammc_factory.h"
#include "datatype_mlist.h"

ListInsert::ListInsert(const PdArgs& args)
    : BaseObject(args)
    , lst_(positionalArguments())
    , index_(nullptr)
{
    index_ = new SizeTProperty("@index", 0);
    addProperty(index_);

    createInlet();
    createInlet();
    createOutlet();
}

void ListInsert::onList(const AtomList& lst)
{
    AtomList res;
    res.reserve(lst.size() + lst_.size());

    if (index_->value() > lst.size()) {
        OBJ_ERR << "index value is too big: " << index_->value();
        return;
    }

    const size_t N = std::min<size_t>(lst.size(), index_->value());

    // insert before
    for (size_t i = 0; i < N; i++)
        res.append(lst[i]);

    // main insert
    for (size_t i = 0; i < lst_.size(); i++)
        res.append(lst_[i].asAtom());

    // insert after
    for (size_t i = N; i < lst.size(); i++)
        res.append(lst[i]);

    listTo(0, res);
}

void ListInsert::onInlet(size_t n, const AtomList& lst)
{
    switch (n) {
    case 1:
        lst_.set(lst);
        break;
    case 2:
        index_->set(lst);
        break;
    default:
        OBJ_ERR << "invalid inlet: " << n;
        break;
    }
}

void ListInsert::onDataT(const DataTPtr<DataTypeMList>& dptr)
{
    if (index_->value() > dptr->size()) {
        OBJ_ERR << "index value is too big: " << index_->value();
        return;
    }

    const size_t N = std::min<size_t>(dptr->size(), index_->value());

    DataTypeMList res(*dptr);
    res.insert(N, lst_);
    dataTo(0, DataTPtr<DataTypeMList>(res));
}

void setup_list_insert()
{
    ObjectFactory<ListInsert> obj("list.insert");
    obj.processData<DataTypeMList>();
}
