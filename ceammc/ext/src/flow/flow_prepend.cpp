/*****************************************************************************
 * Copyright 2023 Serge Poltavski. All rights reserved.
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
#include "flow_prepend.h"
#include "ceammc_factory.h"

FlowPrepend::FlowPrepend(const PdArgs& args)
    : BaseObject(args)
    , inlet2_(this)
{
    bindProxyInlet(inlet2_, 1);
    createOutlet();

    message_ = new MessageProperty("@msg");
    message_->setArgIndex(0);
    addProperty(message_);
}

void FlowPrepend::onBang()
{
    output();
    bangTo(0);
}

void FlowPrepend::onFloat(t_float v)
{
    output();
    floatTo(0, v);
}

void FlowPrepend::onSymbol(t_symbol* s)
{
    output();
    symbolTo(0, s);
}

void FlowPrepend::onList(const AtomListView& lv)
{
    output();
    listTo(0, lv);
}

void FlowPrepend::onAny(t_symbol* s, const AtomListView& lv)
{
    output();
    anyTo(0, s, lv);
}

void FlowPrepend::onProxyAny(int id, t_symbol* s, const AtomListView& lv)
{
    message_->value().setAny(s, lv);
}

void FlowPrepend::output()
{
    if (message_->value().isNone())
        bangTo(0);
    else
        messageTo(0, message_->value());
}

void setup_flow_prepend()
{
    ObjectFactory<FlowPrepend> obj("flow.prepend");
    obj.noPropsDispatch();
    obj.setXletsInfo({ "any: input flow", "any: set prepend message" }, { "any: output" });

    obj.useProxyAny();

    obj.setDescription("prepend message to flow stream");
    obj.setCategory("flow");
    obj.setKeywords({ "flow", "prepend" });
}
