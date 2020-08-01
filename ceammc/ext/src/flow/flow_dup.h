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
#ifndef FLOW_DUP_H
#define FLOW_DUP_H

#include "ceammc_clock.h"
#include "ceammc_object.h"
using namespace ceammc;

class FlowDup : public BaseObject {
    FloatProperty* delay_;
    ClockLambdaFunction clock_;
    Message msg_;

public:
    FlowDup(const PdArgs& a);
    void onInlet(size_t n, const AtomList& l) override;
    void onBang() override;
    void onFloat(t_float f) override;
    void onSymbol(t_symbol* s) override;
    void onList(const AtomList& l) override;
    void onAny(t_symbol* s, const AtomList& l) override;

private:
    void delay();
    void reset();
    static void inletProxy(t_inlet* x, t_symbol* s, int argc, t_atom* argv);
    static void anyFn(void* x, t_symbol* s, int argc, t_atom* argv);
};

void setup_flow_dup();

#endif // FLOW_DUP_H
