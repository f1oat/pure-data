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
#include "list_last.h"
#include "test_list_base.h"

PD_COMPLETE_TEST_SETUP(ListLast, list, last)

TEST_CASE("list.last", "[externals]")
{
    pd_test_init();

    SECTION("create")
    {
        TestListLast t("list.insert");
        REQUIRE(t.numInlets() == 1);
        REQUIRE(t.numOutlets() == 1);
    }

    SECTION("do")
    {
        TestExtListLast t("list.last");

        t << L();
        REQUIRE_THAT(t, !hasOutput(&t));
        ;
        t << LF(1000);
        REQUIRE_THAT(t, outputFloat(&t, 1000));
        t << LF(100, 200, 300);
        REQUIRE_THAT(t, outputFloat(&t, 300));

        t.send(MLA());
        REQUIRE_THAT(t, !hasOutput(&t));
        t.send(MLA(1));
        REQUIRE_THAT(t, outputFloat(&t, 1));
        t.send(MLA("a", "b", "c"));
        REQUIRE_THAT(t, outputSymbol(&t, "c"));

        t.send(MLA("a", "b", "c", MLA()));
        REQUIRE(t.outputAtomAt(0) == MLA());
        t.send(MLA("a", "b", "c", MLA(1, 2)));
        REQUIRE(t.outputAtomAt(0) == MLA(1, 2));
    }
}
