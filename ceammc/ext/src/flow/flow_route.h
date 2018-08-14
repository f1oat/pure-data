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
#ifndef FLOW_ROUTE_H
#define FLOW_ROUTE_H

#include "ceammc_object.h"
using namespace ceammc;

class FlowRoute : public BaseObject {
    const size_t n_;

public:
    FlowRoute(const PdArgs& args);
    void parseProperties() override;
    bool processAnyProps(t_symbol* sel, const AtomList& lst) override;

    void onList(const AtomList& lst) override;
    void onAny(t_symbol* s, const AtomList& lst) override;
    void onData(const DataPtr& ptr) override;
};

void setup_flow_route();

#endif // FLOW_ROUTE_H
