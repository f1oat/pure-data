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
#ifndef LIST_APPLY_TO_H
#define LIST_APPLY_TO_H

#include "ceammc_object.h"
using namespace ceammc;

class DataTypeMList;

class ListApplyTo : public BaseObject {
    typedef std::vector<int> IndexList;
    IndexList idxs_;
    IndexList norm_idxs_;
    AtomList mapped_;
    int cur_idx_;
    bool on_loop_;

public:
    ListApplyTo(const PdArgs& args);

    bool processAnyProps(t_symbol* sel, const AtomList& lst) override;

    void onList(const AtomList&) override;
    void onInlet(size_t n, const AtomList&) override;
    void onDataT(const DataTPtr<DataTypeMList>& dptr);

private:
    void setIndexes(const AtomList& lst);
    void loopProcess(const AtomList& v);
    void normalizeIndexes(size_t N);
};

void setup_list_apply_to();

#endif // LIST_APPLY_TO_H
