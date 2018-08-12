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
#include "../data/datatype_mlist.h"
#include "../list/list_reverse.h"
#include "test_base.h"
#include "test_external.h"

PD_COMPLETE_TEST_SETUP(ListReverse, list, reverse);

TEST_CASE("list.reverse", "[externals]")
{
    pd_test_init();

    SECTION("create")
    {
        TestListReverse t("list.reverse");
        REQUIRE(t.numInlets() == 1);
        REQUIRE(t.numOutlets() == 1);
    }

    SECTION("do")
    {
        TestExtListReverse t("list.reverse");

        t << L();
        REQUIRE(t.outputListAt(0) == L());

        t << LF(1);
        REQUIRE(t.outputFloatAt(0) == 1);

        t << LF(1, 2);
        REQUIRE(t.outputListAt(0) == LF(2, 1));

        t << LA(1, 2, "A", "B");
        REQUIRE(t.outputListAt(0) == LA("B", "A", 2, 1));

        t.send(DataTypeMList());
        REQUIRE(t.outputDataAt(0) == DataPtr(new DataTypeMList));

        t.send(DataTypeMList("(1 2 3)"));
        REQUIRE(t.outputDataAt(0) == DataPtr(new DataTypeMList("(3 2 1)")));

        t.send(DataTypeMList("(() () ())"));
        REQUIRE(t.outputDataAt(0) == DataPtr(new DataTypeMList("(() () ())")));

        t.send(DataTypeMList("((A B C) (1 2 3) (\"with spaces\"))"));
        REQUIRE(t.outputDataAt(0) == DataPtr(new DataTypeMList("((\"with spaces\") (1 2 3) (A B C))")));
    }
}
