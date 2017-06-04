/*****************************************************************************
 * Copyright 2017 Serge Poltavsky. All rights reserved.
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
#include "ceammc_datapointer.h"
#include "ceammc_log.h"

#include <cstdio>
#include <typeinfo>

using namespace ceammc;

BaseData::~BaseData() {}

std::string BaseData::toString() const
{
    char buf[120];
    sprintf(buf, "[Data %s id: %i]", typeid(*this).name(), type());
    return buf;
}

bool BaseData::isEqual(const BaseData* d) const
{
    if (type() != d->type())
        return false;

    return this == d;
}

DataManager::DataManager()
{
}

DataManager& DataManager::instance()
{
    static DataManager fact;
    return fact;
}

void DataManager::addGetFn(DataType type, GetDataFn fn)
{
    LIB_DBG << "registered data type: " << type;
    fn_[type] = fn;
}

void DataManager::addCloneFn(DataType type, CloneDataFn fn)
{
    fn_clone_[type] = fn;
}

Data<BaseData>* DataManager::clone(const DataDesc& d)
{
    CloneFnMap::iterator it = fn_clone_.find(d.type);
    if (it == fn_.end()) {
        LIB_ERR << "type not found: " << d.type;
        return 0;
    } else
        return it->second(d.id);
}

Data<BaseData>* DataManager::get(const DataDesc& d)
{
    GetFnMap::iterator it = fn_.find(d.type);
    if (it == fn_.end()) {
        LIB_ERR << "type not found: " << d.type;
        return 0;
    } else
        return it->second(d.id);
}

BaseData* DataManager::rawData(const DataDesc& d)
{
    Data<BaseData>* data = get(d);
    return data == 0 ? 0 : data->data();
}

BaseData* DataManager::rawData(DataType type, DataId id)
{
    return rawData(DataDesc(type, id));
}
