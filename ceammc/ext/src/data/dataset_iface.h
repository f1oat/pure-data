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
#ifndef DATASET_IFACE_H
#define DATASET_IFACE_H

#include "ceammc_object.h"
#include "data_protocol.h"
#include "datatype_set.h"

using namespace ceammc;

template <class T>
class DataSetIface : public CollectionIFace<T> {
public:
    DataSetIface(const PdArgs& args)
        : CollectionIFace<T>(args)
    {
    }

    void dump() const override
    {
        BaseObject::dump();
        OBJ_DBG << data().toString();
    }

    void onBang() override
    {
        this->dataTo(0, data().clone());
    }

    void onFloat(float f) override
    {
        data().add(Atom(f));
    }

    void onSymbol(t_symbol* s) override
    {
        data().add(Atom(s));
    }

    void onList(const AtomList& l) override
    {
        data().add(l);
    }

    void onDataT(const DataTPtr<DataTypeSet>& s)
    {
        data() = *s.data();
        onBang();
    }

    void proto_add(const AtomList& l) override
    {
        data().add(l);
    }

    void proto_clear() override
    {
        data().clear();
    }

    size_t proto_size() const override
    {
        return data().size();
    }

    bool proto_remove(const AtomList& lst) override
    {
        if (!data().contains(lst))
            return false;

        data().remove(lst);
        return true;
    }

    void proto_set(const AtomList& lst) override
    {
        data() = DataTypeSet(lst);
    }

public:
    virtual DataTypeSet& data() = 0;
    virtual const DataTypeSet& data() const = 0;
};

#endif // DATASET_IFACE_H
