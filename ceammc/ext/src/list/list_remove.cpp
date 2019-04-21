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
#include "list_remove.h"
#include "ceammc_convert.h"
#include "ceammc_factory.h"
#include "ceammc_fn_list.h"
#include "datatype_mlist.h"

ListRemove::ListRemove(const PdArgs& args)
    : BaseObject(args)
{
    createInlet();
    createOutlet();

    onInlet(1, positionalArguments());
}

void ListRemove::onInlet(size_t, const AtomList& lst)
{
    auto l = list::uniqueSorted(lst);
    idx_.clear();
    idx_.reserve(l.size());
    for (auto& el : l)
        idx_.push_back(el.asInt());
}

void ListRemove::onList(const AtomList& lst)
{
    size_t N = lst.size();
    AtomList res;
    res.reserve(N);

    precalcIndexes(N);

    for (size_t i = 0; i < N; i++) {
        auto it = std::find(calc_idx_.begin(), calc_idx_.end(), i);
        if (it != calc_idx_.end())
            continue;

        res.append(lst[i]);
    }

    listTo(0, res);
}

void ListRemove::onDataT(const DataTPtr<DataTypeMList>& dptr)
{
    size_t N = dptr->size();
    DataTypeMList res;
    res.reserve(N);

    precalcIndexes(N);

    for (size_t i = 0; i < N; i++) {
        auto it = std::find(calc_idx_.begin(), calc_idx_.end(), i);
        if (it != calc_idx_.end())
            continue;

        res.append((*dptr)[i]);
    }

    dataTo(0, DataTPtr<DataTypeMList>(res));
}

void ListRemove::precalcIndexes(size_t N)
{
    calc_idx_.clear();
    calc_idx_.reserve(idx_.size());
    for (auto i : idx_) {
        auto pos = relativeIndex<int>(i, N);
        if (pos < 0)
            continue;

        calc_idx_.push_back(pos);
    }
}

void setup_list_remove()
{
    ObjectFactory<ListRemove> obj("list.remove");
    obj.processData<DataTypeMList>();
    obj.mapFloatToList();
    obj.mapSymbolToList();
}
