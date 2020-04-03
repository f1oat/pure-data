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
#ifndef DATA_MLIST_H
#define DATA_MLIST_H

#include "mlist_iface.h"

using DataMListBase = DataMListIFace<BaseObject>;

class DataMList : public DataMListBase {
    DataTypeMList mlist_;

public:
    DataMList(const PdArgs& args);
    DataTypeMList& mlist() final { return mlist_; }
    const DataTypeMList& mlist() const final { return mlist_; }
};

void setup_data_mlist();

#endif // DATA_MLIST_H
