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
#include "flow_pass_if.h"
#include "ceammc_factory.h"

FlowPassIf::FlowPassIf(const PdArgs& a)
    : BaseObject(a)
    , pass_(0)
{
    createInlet();
    createOutlet();
    createOutlet();
}

void FlowPassIf::onBang()
{
    pass_ = 0;
    bangTo(1);

    if (pass_)
        bangTo(0);
}

void FlowPassIf::onFloat(t_float v)
{
    pass_ = 0;
    floatTo(1, v);

    if (pass_)
        floatTo(0, v);
}

void FlowPassIf::onSymbol(t_symbol* s)
{
    pass_ = 0;
    symbolTo(1, s);

    if (pass_)
        symbolTo(0, s);
}

void FlowPassIf::onList(const AtomListView& lv)
{
    pass_ = 0;
    listTo(1, lv);

    if (pass_)
        listTo(0, lv);
}

void FlowPassIf::onAny(t_symbol* s, const AtomListView& lv)
{
    pass_ = 0;
    anyTo(1, s, lv);

    if (pass_)
        anyTo(0, s, lv);
}

void FlowPassIf::onInlet(size_t n, const AtomListView& lv)
{
    if (n != 1 || lv.empty())
        return;

    pass_ = (lv.toT<size_t>(0) == 1) ? 1 : 0;
}

void setup_flow_pass_if()
{
    ObjectFactory<FlowPassIf> obj("flow.pass_if");
    obj.addAlias("pass_if");
    obj.noPropsDispatch();
    obj.setXletsInfo({ "any: input", "float: 0|1 from predicate" }, { "any: output", "any: output to predicate" });

    obj.setDescription("pass values that accepted by predicate object");
    obj.setCategory("flow");
    obj.setKeywords({"pass", "functional"});
}
