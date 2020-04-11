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
#include "test_flow_base.h"
#include "test_external.h"


typedef TestExternal<FlowPassIf> FlowPassIfTest;

static CanvasPtr cnv = PureData::instance().createTopCanvas("test_canvas");

TEST_CASE("flow.pass_if", "[externals]")
{
    SECTION("init")
    {
        FlowPassIfTest t("flow.pass_if");
        REQUIRE(t.numInlets() == 2);
        REQUIRE(t.numOutlets() == 2);
        REQUIRE(t.canvas() != 0);
        REQUIRE(t.canvas() == t.rootCanvas());
    }

    SECTION("float")
    {
        FlowPassIfTest t("flow.pass_if");
        WHEN_SEND_BANG_TO(0, t);
        REQUIRE_BANG_AT_OUTLET(1, t);
        REQUIRE_NO_MESSAGES_AT_OUTLET(0, t);
    }

    SECTION("float")
    {
        FlowPassIfTest t("flow.pass_if");
        WHEN_SEND_FLOAT_TO(0, t, 2);
        REQUIRE_FLOAT_AT_OUTLET(1, t, 2);
        REQUIRE_NO_MESSAGES_AT_OUTLET(0, t);
    }

    SECTION("symbol")
    {
        FlowPassIfTest t("flow.pass_if");

        WHEN_SEND_SYMBOL_TO(0, t, "c");
        REQUIRE_SYMBOL_AT_OUTLET(1, t, "c");
        REQUIRE_NO_MESSAGES_AT_OUTLET(0, t);
    }

    SECTION("list")
    {
        FlowPassIfTest t("flow.pass_if");

        WHEN_SEND_LIST_TO(0, t, LA("a", "b"));
        REQUIRE_LIST_AT_OUTLET(1, t, LA("a", "b"));
        REQUIRE_NO_MESSAGES_AT_OUTLET(0, t);
    }

    SECTION("any")
    {
        FlowPassIfTest t("flow.pass_if");

        WHEN_SEND_ANY_TO(t, "a", LF(1, 2));
        REQUIRE_ANY_AT_OUTLET(1, t, LA("a", 1, 2));
        REQUIRE_NO_MESSAGES_AT_OUTLET(0, t);
    }

    SECTION("puredata")
    {
        setup_flow0x2epass_if();
        LogExternalOutput::setup();

        pd::External flow_pass_if("flow.pass_if");
        REQUIRE_FALSE(flow_pass_if.isNull());

        pd::External x1(">", LF(10));
        REQUIRE_FALSE(x1.isNull());
        flow_pass_if.connectTo(1, x1.object(), 0);
        x1.connectTo(0, flow_pass_if.object(), 1);

        LogExternalOutput x2;
        flow_pass_if.connectTo(0, x2.object(), 0);

        flow_pass_if.sendFloat(14);
        REQUIRE(x2.msg().isFloat());
        REQUIRE(x2.msg().atomValue() == A(14));

        x2.reset();
        flow_pass_if.sendFloat(10);
        REQUIRE(x2.msg().isNone());

        flow_pass_if.sendFloat(11);
        REQUIRE(x2.msg().isFloat());
        REQUIRE(x2.msg().atomValue() == A(11));
    }
}
