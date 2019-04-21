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
#include "dict_keys.h"
#include "ceammc_factory.h"

DictKeys::DictKeys(const PdArgs& args)
    : BaseObject(args)
{
    createOutlet();
}

void DictKeys::onDataT(const DataTPtr<DataTypeDict>& dptr)
{
    AtomList res;

    const auto& d = dptr->innerData();
    for (auto& kv : d)
        res.append(kv.first);

    listTo(0, res);
}

void setup_dict_keys()
{
    ObjectFactory<DictKeys> obj("dict.keys");
    obj.processData<DataTypeDict>();
}
