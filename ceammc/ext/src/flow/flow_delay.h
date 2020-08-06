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
#ifndef FLOW_DELAY_H
#define FLOW_DELAY_H

#include "ceammc_clock.h"
#include "ceammc_factory.h"
#include "ceammc_object.h"
using namespace ceammc;

class FlowDelay : public BaseObject {
    Message last_msg_;
    FloatProperty* delay_;
    ClockLambdaFunction clock_;

public:
    FlowDelay(const PdArgs& args);

    void onBang() override;
    void onFloat(t_float f) override;
    void onSymbol(t_symbol* s) override;
    void onList(const AtomList& l) override;
    void onAny(t_symbol* s, const AtomListView& l) override;
    void onInlet(size_t n, const AtomList& l) override;

    bool processAnyProps(t_symbol* sel, const AtomListView& lst) override;
};

void setup_flow_delay();

#endif // FLOW_DELAY_H
