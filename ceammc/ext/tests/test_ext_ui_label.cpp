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
#include "../ui/ui_label.h"
#include "ui_external_test.h"

UI_COMPLETE_TEST_SETUP(Label)

TEST_CASE("ui.label", "[ui.label]")
{
    ui_test_init();

    SECTION("construct")
    {
        TestLabel t("ui.label");
        REQUIRE(t->numInlets() == 0);
        REQUIRE(t->numOutlets() == 0);

        REQUIRE_UI_LIST_PROPERTY(t, "text", L1("Label"));

        REQUIRE_UI_LIST_PROPERTY(t, "align", L1("left"));
        REQUIRE_UI_LIST_PROPERTY(t, "fontweight", L1("normal"));
        REQUIRE_UI_FLOAT_PROPERTY(t, "fontsize", 32);

        REQUIRE_UI_FLOAT_PROPERTY(t, "margin_left", 5);
        REQUIRE_UI_FLOAT_PROPERTY(t, "margin_top", 5);
        REQUIRE_UI_FLOAT_PROPERTY(t, "margin_bottom", 5);
        REQUIRE_UI_FLOAT_PROPERTY(t, "margin_right", 5);

        REQUIRE_UI_LIST_PROPERTY(t, "background_color", L4(1, 1, 1, 1));
        REQUIRE_UI_LIST_PROPERTY(t, "text_color", L4(0.f, 0.f, 0.f, 1));

        SECTION("num args")
        {
            TestLabel t("ui.label", L3(1, 2, 3));
            REQUIRE_UI_LIST_PROPERTY(t, "text", L3(1, 2, 3));
            REQUIRE(t->text() == "1 2 3");
        }

        SECTION("mixed args")
        {
            TestLabel t("ui.label", L3("A", "B", 1000));
            REQUIRE_UI_LIST_PROPERTY(t, "text", L3("A", "B", 1000));
            REQUIRE(t->text() == "A B 1000");
        }

        SECTION("mixed args and props")
        {
            TestLabel t("ui.label", L5("A", "BC", 1000, "@pinned", 1));
            REQUIRE_UI_LIST_PROPERTY(t, "text", L3("A", "BC", 1000));
            REQUIRE(t->text() == "A BC 1000");
        }
    }

    SECTION("props")
    {
        TestLabel t("ui.label");
        REQUIRE_UI_LIST_PROPERTY(t, "align", L1("left"));
        t->setProperty(gensym("align"), L1("center"));
        REQUIRE_UI_LIST_PROPERTY(t, "align", L1("center"));
        t->setProperty(gensym("align"), L1("right"));
        REQUIRE_UI_LIST_PROPERTY(t, "align", L1("right"));
    }

    SECTION("clear")
    {
        TestLabel t("ui.label", L5(1, 2, 3, 4, 5));
        REQUIRE(t->text() == "1 2 3 4 5");

        t.call("clear");
        REQUIRE(t->text() == "");
    }

    SECTION("input")
    {
        TestExtLabel t("ui.label", L2("A", "test"));
        t.send(10);
        REQUIRE(t->text() == "10");
        t.bang();
        REQUIRE(t->text() == "10");
        t.send("ABC");
        REQUIRE(t->text() == "ABC");
        t.send(L3(1, 2, 3));
        REQUIRE(t->text() == "1 2 3");
        t.call("msg", L2("A", 100));
        REQUIRE(t->text() == "msg A 100");

        t.call("append", L2(3, 4));
        REQUIRE(t->text() == "msg A 100 3 4");

        t.call("prepend", L2(1, 2));
        REQUIRE(t->text() == "1 2 msg A 100 3 4");

        // but @text property is not changed
        REQUIRE_UI_LIST_PROPERTY(t, "text", L2("A", "test"));
    }
}
