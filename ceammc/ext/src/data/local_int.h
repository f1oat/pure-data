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
#ifndef LOCAL_INT_H
#define LOCAL_INT_H

#include "data_protocol.h"
#include "local_base.h"

using namespace ceammc;

typedef long LocalIntType;
typedef NumericIFace<LocalBase<LocalIntType>, LocalIntType> LocalIntBase;

class LocalInt : public LocalIntBase {
public:
    LocalInt(const PdArgs& a);
    LocalIntType& value() final { return ref(); }
    const LocalIntType& value() const final { return ref(); }
};

void setup_local_int();

#endif // LOCAL_INT_H
