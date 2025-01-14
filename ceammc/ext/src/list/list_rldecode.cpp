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
#include "list_rldecode.h"
#include "ceammc_factory.h"
#include "ceammc_fn_list.h"

#include <algorithm>
#include <numeric>

ListRLDecode::ListRLDecode(const PdArgs& args)
    : BaseObject(args)
{
    createInlet();
    createOutlet();

    createCbListProperty(
        "@l",
        [this]() -> AtomList { return len_; },
        [this](const AtomListView& lv) -> bool { len_ = lv; return true; })
        ->setArgIndex(0);
}

void ListRLDecode::onList(const AtomListView& lv)
{
    const size_t N = std::min(lv.size(), len_.size());
    std::vector<std::pair<Atom, size_t>> rle;
    rle.reserve(N);

    for (size_t i = 0; i < N; i++)
        rle.push_back({ lv[i], len_[i].asInt() });

    listTo(0, list::rleDecode(rle));
}

void ListRLDecode::onInlet(size_t n, const AtomListView& lv)
{
    len_ = lv;
}

void setup_list_rldecode()
{
    ObjectFactory<ListRLDecode> obj("list.rldecode");
    obj.addAlias("list.unwrap");
    obj.setXletsInfo({ "list: entries", "list: repeats" }, { "list: result list" });

    obj.setDescription("Run-length list decoder (RLE)");
    obj.setCategory("list");
    obj.setKeywords({"list", "rle", "decode"});
}
