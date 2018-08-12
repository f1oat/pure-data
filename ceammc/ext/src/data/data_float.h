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
#ifndef DATA_FLOAT_H
#define DATA_FLOAT_H

#include "ceammc_object.h"
#include "data_protocol.h"

using namespace ceammc;

typedef NumericIFace<BaseObject, t_float> DataFloatBase;

class DataFloat : public DataFloatBase {
    t_float value_;

public:
    DataFloat(const PdArgs& a);

    t_float& value() final { return value_; }
    const t_float& value() const final { return value_; }
};

void setup_data_float();

#endif // DATA_FLOAT_H
