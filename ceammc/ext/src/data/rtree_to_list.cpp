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
#include "rtree_to_list.h"
#include "ceammc_factory.h"
#include "ceammc_format.h"
#include "ceammc_rtree.h"

RhythmTreeToList::RhythmTreeToList(const PdArgs& args)
    : BaseObject(args)
    , dur_(nullptr)
    , rtree_(nullptr)
{
    dur_ = new FloatProperty("@dur", 1);
    dur_->setArgIndex(0);
    dur_->setSuccessFn([this](Property*) { changed_ = true; });
    addProperty(dur_);

    rtree_ = new AtomProperty("@rtree", MListAtom());
    rtree_->setAtomCheckFn([](const Atom& a) -> bool {
        return a.isDataType(DataTypeMList::staticType());
    });
    rtree_->setSuccessFn([this](Property*) { changed_ = true; });
    rtree_->setArgIndex(1);
    addProperty(rtree_);

    createOutlet();
}

void RhythmTreeToList::onBang()
{
    if (changed_) {
        cached_ = rtree::rhythm_tree(dur_->value(), rtree_->value().asDataT<DataTypeMList>());
        LIB_LOG << "update cached value: " << cached_;
        changed_ = false;
    }

    listTo(0, cached_);
}

void RhythmTreeToList::onFloat(t_float f)
{
    if (dur_->setValue(f)) {
        changed_ = true;
        onBang();
    }
}

void RhythmTreeToList::onAny(t_symbol* s, const AtomListView& args)
{
    std::string str(s->s_name);
    str += ' ';
    str += to_string(args, " ");
    MListAtom ml;
    if (!ml->setFromDataString(str)) {
        OBJ_ERR << "can't parse message: " << s->s_name << ' ' << to_string(args);
        return;
    }

    rtree_->setValue(ml);
    changed_ = true;
}

void RhythmTreeToList::onDataT(const MListAtom& ml)
{
    rtree_->value() = ml;
    changed_ = true;
}

void setup_rtree_to_list()
{
    ObjectFactory<RhythmTreeToList> obj("rtree.to_list");
    obj.addAlias("rtree->list");
    obj.processData<DataTypeMList>();

    obj.setXletsInfo(
        { "bang:  calc and output\n"
          "float: set duration, calc and output\n"
          "any:   try to set rhythm tree, no output" },
        { "list: list of rhythm fractions" });

    obj.setDescription("convert OpenMusic-like rhythm tree to list of fractions");
    obj.setCategory("data");
    obj.setKeywords({"rhythm", "rtree", "openmusic", "pattern"});
}
