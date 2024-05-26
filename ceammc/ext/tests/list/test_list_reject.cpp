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
#include "list_reject.h"
#include "test_list_base.h"

PD_COMPLETE_TEST_SETUP(ListReject, list, reject)

TEST_CASE("list.reject", "[externals]")
{
    pd_test_init();

    SECTION("create")
    {
        SECTION("default")
        {
            TObj t("list.reject");
            REQUIRE(t.numInlets() == 2);
            REQUIRE(t.numOutlets() == 1);
            REQUIRE_PROPERTY_LIST(t, @args, L());
        }

        SECTION("args")
        {
            TObj t("list.reject", LA(5, 4, 3, 2, 1));
            REQUIRE_PROPERTY_LIST(t, @args, LF(1, 2, 3, 4, 5));
        }

        SECTION("@args")
        {
            TObj t("list.reject", LA("@args", 5, 5, 4, 3, 2, 1));
            REQUIRE_PROPERTY_LIST(t, @args, LF(1, 2, 3, 4, 5));
        }

        SECTION("multiple")
        {
            TObj t("list.pass", LF(20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1));
            REQUIRE_PROPERTY_LIST(t, @args, LF(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20));
        }
    }

    SECTION("do")
    {
        TExt t("list.reject");

        t << 1;
        REQUIRE(t.hasOutput());
        REQUIRE(t.outputFloatAt(0) == 1);
        t << "ABC";
        REQUIRE(t.hasOutput());
        REQUIRE(t.outputSymbolAt(0) == SYM("ABC"));
        t << LF(1, 2, 3);
        REQUIRE(t.hasOutput());
        REQUIRE(t.outputListAt(0) == LF(1, 2, 3));

        t.sendListTo(LA("A", 1), 1);
        REQUIRE_PROPERTY_LIST(t, @args, LA(1, "A"));

        t << 1;
        REQUIRE(!t.hasOutputAt(0));

        t << 2;
        REQUIRE(t.hasOutput());
        REQUIRE(t.outputFloatAt(0) == 2);

        t << "A";
        REQUIRE(!t.hasOutputAt(0));

        t << "B";
        REQUIRE(t.hasOutput());
        REQUIRE(t.outputSymbolAt(0) == SYM("B"));

        t << LA("A", "B", "C", 1, 2, 3, 2, 1, "A");
        REQUIRE(t.outputListAt(0) == LA("B", "C", 2, 3, 2));
    }

    SECTION("mlist")
    {
        TExt t("list.reject", LF(1, 2));

        t.send(MLA());
        REQUIRE(t.hasOutput());
        REQUIRE(t.outputAtomAt(0) == MLA());

        t.send(MLA(3, 2, 1, 4, -2));
        REQUIRE(t.hasOutput());
        REQUIRE(t.outputAtomAt(0) == MLA(3, 4, -2));
    }
}