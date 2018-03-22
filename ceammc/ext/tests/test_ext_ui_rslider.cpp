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
#include "../ui/ui_rslider.h"
#include "ui_external_test.h"

UI_COMPLETE_TEST_SETUP(RSlider)

TEST_CASE("ui.rslider", "[ui.rslider]")
{
    ui_test_init();

    SECTION("construct")
    {
        TestRSlider t("ui.rslider");
        REQUIRE(t->numInlets() == 1);
        REQUIRE(t->numOutlets() == 1);
        REQUIRE_UI_LIST_PROPERTY(t, "size", L2(120, 16));
        REQUIRE_UI_FLOAT_PROPERTY(t, "min", 0);
        REQUIRE_UI_FLOAT_PROPERTY(t, "max", 1);
        REQUIRE_UI_FLOAT_PROPERTY(t, "sync", 0);
        REQUIRE_UI_LIST_PROPERTY(t, "value", L2(0.4, 0.6));
        REQUIRE_UI_FLOAT_PROPERTY(t, "range", 0.2);
        REQUIRE_UI_FLOAT_PROPERTY(t, "low", 0.4);
        REQUIRE_UI_FLOAT_PROPERTY(t, "high", 0.6);

        SECTION("props 1")
        {
            TestExtRSlider t("ui.rslider", L4("@min", -2, "@max", 0.f));
            REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(-1.2, -0.8));
            REQUIRE_UI_FLOAT_PROPERTY(t, "range", 0.4);
        }

        SECTION("props 2")
        {
            TestExtRSlider t("ui.rslider", L4("@min", 1, "@max", 0.f));
            REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.6, 0.4));
        }

        SECTION("props 3")
        {
            TestExtRSlider t("ui.rslider", L4("@min", 1, "@max", 1));
            REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(1, 1));
        }
    }

    SECTION("set")
    {
        TestExtRSlider t("ui.rslider");

        t << $2("@value", 0.1, 0.9);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.1, 0.9));

        t << $2("@value", 0.6, 0.5);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.5, 0.6));
        REQUIRE_UI_FLOAT_PROPERTY(t, "range", 0.1);

        t << $2("@value", -10, 100);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.f, 1));

        t << $2("@value", 10, -100);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.f, 1));

        t << $1("@low", -1);
        REQUIRE_UI_FLOAT_PROPERTY(t, "low", 0.f);
        t << $1("@low", 0.4);
        REQUIRE_UI_FLOAT_PROPERTY(t, "low", 0.4f);
        t << $1("@high", 0.5);
        REQUIRE_UI_FLOAT_PROPERTY(t, "high", 0.5f);
        t << $1("@high", 0.3);
        REQUIRE_UI_FLOAT_PROPERTY(t, "high", 0.4f);
        REQUIRE_UI_FLOAT_PROPERTY(t, "low", 0.3f);

        t <<= L3("set", 0.1, 0.2);
        REQUIRE_NO_OUTPUT(t);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.1, 0.2));

        t <<= L3("set", 0.5, 0.2);
        REQUIRE_NO_OUTPUT(t);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.2, 0.5));
    }

    SECTION("mouse")
    {
        TestExtRSlider t("ui.rslider");

        t.mouseDown(0, 8);
        REQUIRE_NO_OUTPUT(t);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0, 0));
        t.mouseDown(120, 8);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(1, 1));
        t.mouseDown(60, 8);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.5, 0.5));

        t.mouseDrag(120, 8);
        REQUIRE_NO_OUTPUT(t);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.5, 1));

        t.mouseUp(120, 8);
        REQUIRE_OUTPUT_LIST(t, 0, ListApprox(0.5, 1));

        t.mouseDown(120, 8);
        t.mouseDrag(90, 8);
        t.mouseDrag(60, 8);
        t.mouseUp(60, 8);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.5, 1));
        REQUIRE_OUTPUT_LIST(t, 0, ListApprox(0.5, 1));
    }

    SECTION("send")
    {
        TestExtRSlider t("ui.rslider", L2("@send", "r1"));
        t.addListener("r1");

        t << BANG;
        REQUIRE_LIST_WAS_SEND(t, "r1", ListApprox(0.4, 0.6));
        REQUIRE_OUTPUT_LIST(t, 0, ListApprox(0.4, 0.6));
    }

    SECTION("bang")
    {
        TestExtRSlider t("ui.rslider");
        t << BANG;
        REQUIRE_OUTPUT_LIST(t, 0, ListApprox(0.4, 0.6));
        t << L2(0.5, 0.9);
        REQUIRE_OUTPUT_LIST(t, 0, ListApprox(0.5, 0.9));
        t << L2(0.5, 0.1);
        REQUIRE_OUTPUT_LIST(t, 0, ListApprox(0.1, 0.5));

        t << $1("min", 1) << $1("max", 0);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.1, 0.5));
        t << BANG;
        REQUIRE_OUTPUT_LIST(t, 0, ListApprox(0.1, 0.5));

        t << L2(0.5, 0.9);
        REQUIRE_UI_FLOAT_PROPERTY(t, "high", 0.9);
        REQUIRE_UI_FLOAT_PROPERTY(t, "low", 0.5);

        t << L2(0.9, 0.5);
        REQUIRE_UI_FLOAT_PROPERTY(t, "high", 0.9);
        REQUIRE_UI_FLOAT_PROPERTY(t, "low", 0.5);
    }

    SECTION("presets")
    {
        TestExtRSlider t("ui.rslider");
        REQUIRE(t->presetId() == gensym("ui.rslider.0"));

        t <<= L2("store", 0.f);
        t << L2(0.1, 0.2) <<= L2("store", 1);
        t << L2(0.8, 0.9) <<= L2("store", 2);

        t->loadPreset(0);
        REQUIRE_OUTPUT_LIST(t, 0, L2(0.4, 0.6));
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.4, 0.6));

        t->loadPreset(1);
        REQUIRE_OUTPUT_LIST(t, 0, L2(0.1, 0.2));
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.1, 0.2));

        t->loadPreset(2);
        REQUIRE_OUTPUT_LIST(t, 0, L2(0.8, 0.9));
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.8, 0.9));

        t->loadPreset(4);
        REQUIRE_NO_OUTPUT(t);
        REQUIRE_UI_LIST_PROPERTY(t, "value", ListApprox(0.8, 0.9));
    }
}
