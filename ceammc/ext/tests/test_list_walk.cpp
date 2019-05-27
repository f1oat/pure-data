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

#include "../list/list_walk.h"
#include "catch.hpp"
#include "ceammc.hpp"
#include "test_base.h"
#include "test_external.h"

PD_COMPLETE_TEST_SETUP(ListWalk, list, walk)

TEST_CASE("list.walk", "[externals]")
{
    pd_test_init();

    SECTION("test single")
    {
        TestListWalk t("list.walk", L());

        REQUIRE(t.numInlets() == 1);
        REQUIRE(t.numOutlets() == 2);
        REQUIRE(t.hasProperty("@direction"));
        REQUIRE(t.hasProperty("@index"));
        REQUIRE(t.hasProperty("@value"));
        REQUIRE(t.hasProperty("@mode"));
        REQUIRE(t.hasProperty("@length"));
        REQUIRE(t.hasProperty("@single"));
        REQUIRE(t.hasProperty("@loop"));
        REQUIRE(t.hasProperty("@wrap"));
        REQUIRE(t.hasProperty("@clip"));
        REQUIRE(t.hasProperty("@fold"));
        REQUIRE(t.hasProperty("@size"));
        REQUIRE_PROPERTY_FLOAT(t, @single, 1);
        REQUIRE(t.property("@size")->readonly());
        REQUIRE(!t.property("@index")->readonly());

        REQUIRE(t.messageCount() == 0);

        t.sendList(LF(1.0, 2.0, 3.0));

        CALL(t, current);
        REQUIRE_LIST_MSG(t, LF(1));
        REQUIRE_INDEX(t, 0);

        CALL(t, next);
        REQUIRE_LIST_MSG(t, LF(2));
        REQUIRE_INDEX(t, 1);
        CALL(t, current);
        REQUIRE_LIST_MSG(t, LF(2));

        CALL(t, next);
        REQUIRE_LIST_MSG(t, LF(3));
        REQUIRE_INDEX(t, 2);
        CALL(t, current);
        REQUIRE_LIST_MSG(t, LF(3));

        CALL(t, next);
        REQUIRE_NO_MSG(t);
        REQUIRE_INDEX(t, 2);

        CALL(t, current);
        REQUIRE_NO_MSG(t);

        CALL(t, reset);
        REQUIRE_INDEX(t, 0);
        CALL(t, current);
        REQUIRE_LIST_MSG(t, LF(1));
        CALL(t, next);
        REQUIRE_LIST_MSG(t, LF(2));
        CALL(t, next);
        REQUIRE_LIST_MSG(t, LF(3));

        CALL(t, next);
        REQUIRE_NO_MSG(t);
        REQUIRE_INDEX(t, 2);
        CALL(t, next);
        REQUIRE_NO_MSG(t);
        REQUIRE_INDEX(t, 2);
        CALL(t, next);
        REQUIRE_NO_MSG(t);
        REQUIRE_INDEX(t, 2);

        CALL(t, prev);
        REQUIRE_LIST_MSG(t, LF(2));
        CALL(t, prev);
        REQUIRE_LIST_MSG(t, LF(1));

        CALL(t, prev);
        REQUIRE_NO_MSG(t);
        REQUIRE_INDEX(t, 0);
        CALL(t, prev);
        REQUIRE_NO_MSG(t);
        REQUIRE_INDEX(t, 0);

        CALL(t, next);
        REQUIRE_LIST_MSG(t, LF(2));
        REQUIRE_INDEX(t, 1);

        SECTION("test single step")
        {
            TestListWalk t("list.walk");

            t.sendList(LF(1.0, 2.0, 3.0, 4.0, 5.0));

            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(1));

            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(3));

            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(5));

            CALL_N(t, next, 2);
            REQUIRE_NO_MSG(t);

            CALL_N(t, prev, 2);
            REQUIRE_LIST_MSG(t, LF(3));

            CALL_N(t, prev, 2);
            REQUIRE_LIST_MSG(t, LF(1));

            CALL_N(t, prev, 2);
            REQUIRE_NO_MSG(t);
            REQUIRE_INDEX(t, 0);
        }

        SECTION("test single length 2")
        {
            SECTION("step 1")
            {
                TestListWalk t("list.walk", LA("@length", 2));

                t.sendList(LF(1.0, 2.0, 3.0, 4.0, 5.0));

                CALL(t, current);
                REQUIRE_LIST_MSG(t, AtomList(1, 2));

                CALL(t, next);
                REQUIRE_LIST_MSG(t, AtomList(2, 3));

                CALL(t, next);
                REQUIRE_LIST_MSG(t, AtomList(3, 4));

                CALL(t, next);
                REQUIRE_LIST_MSG(t, AtomList(4, 5));

                CALL(t, next);
                REQUIRE_LIST_MSG(t, LF(5));

                CALL(t, next);
                REQUIRE_NO_MSG(t);

                CALL(t, prev);
                REQUIRE_LIST_MSG(t, AtomList(4, 5));

                CALL(t, prev);
                REQUIRE_LIST_MSG(t, AtomList(3, 4));

                CALL(t, prev);
                REQUIRE_LIST_MSG(t, AtomList(2, 3));

                CALL(t, prev);
                REQUIRE_LIST_MSG(t, AtomList(1, 2));

                CALL(t, prev);
                REQUIRE_NO_MSG(t);
                REQUIRE_INDEX(t, 0);
            }

            SECTION("step 2")
            {
                TestListWalk t("list.walk", LA("@length", 2));

                t.sendList(LF(1.0, 2.0, 3.0, 4.0, 5.0));

                CALL(t, current);
                REQUIRE_LIST_MSG(t, AtomList(1, 2));

                CALL_N(t, next, 2);
                REQUIRE_LIST_MSG(t, AtomList(3, 4));

                CALL_N(t, next, 2);
                REQUIRE_LIST_MSG(t, LF(5));

                CALL_N(t, next, 2);
                REQUIRE_NO_MSG(t);
                REQUIRE_INDEX(t, 4);

                CALL_N(t, prev, 2);
                REQUIRE_LIST_MSG(t, AtomList(3, 4));

                CALL_N(t, prev, 2);
                REQUIRE_LIST_MSG(t, AtomList(1, 2));

                CALL(t, prev);
                REQUIRE_NO_MSG(t);
                REQUIRE_INDEX(t, 0);
            }
        }
    }

    SECTION("test wrap")
    {
        SECTION("basic")
        {
            TestListWalk t("list.walk", LA("@wrap"));

            CALL(t, current);
            REQUIRE_NO_MSG(t);
            REQUIRE_INDEX(t, 0);
            CALL(t, next);
            REQUIRE_NO_MSG(t);

            t.sendList(LF(1.0, 2.0, 3.0));

            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);

            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(2));
            REQUIRE_INDEX(t, 1);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(2));
            REQUIRE_INDEX(t, 1);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);

            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(2));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(3));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(2));

            CALL(t, reset);
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(3));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(2));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
        }

        SECTION("step 2")
        {
            TestListWalk t("list.walk", LA("@wrap"));

            CALL(t, current);
            REQUIRE_NO_MSG(t);
            CALL(t, next);
            REQUIRE_NO_MSG(t);

            t.sendList(LF(1.0, 2.0, 3.0));

            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);

            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(2));
            REQUIRE_INDEX(t, 1);
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);

            CALL_N(t, prev, 3);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);

            CALL_N(t, prev, 4);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);
        }

        SECTION("length 2")
        {
            TestListWalk t("list.walk", LA("@wrap", "@length", 2));

            CALL(t, current);
            REQUIRE_NO_MSG(t);
            CALL(t, next);
            REQUIRE_NO_MSG(t);

            t.sendList(LF(1.0, 2.0, 3.0, 4.0, 5.0));

            CALL(t, current);
            REQUIRE_LIST_MSG(t, AtomList(1, 2));
            REQUIRE_INDEX(t, 0);

            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, AtomList(3, 4));
            REQUIRE_INDEX(t, 2);
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, AtomList(5, 1));
            REQUIRE_INDEX(t, 4);
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, AtomList(2, 3));
            REQUIRE_INDEX(t, 1);

            CALL_N(t, prev, 3);
            REQUIRE_LIST_MSG(t, AtomList(4, 5));
            REQUIRE_INDEX(t, 3);
            CALL_N(t, prev, 3);
            REQUIRE_LIST_MSG(t, AtomList(1, 2));
            REQUIRE_INDEX(t, 0);
        }
    }

    SECTION("test clip")
    {
        SECTION("basic")
        {
            TestListWalk t("list.walk", LA("@clip"));

            CALL(t, current);
            REQUIRE_NO_MSG(t);
            REQUIRE_INDEX(t, 0);
            CALL(t, next);
            REQUIRE_NO_MSG(t);

            t.sendList(LF(1.0, 2.0, 3.0));

            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);

            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(2));
            REQUIRE_INDEX(t, 1);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);

            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(2));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);

            CALL(t, reset);
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
        }

        SECTION("step 2")
        {
            TestListWalk t("list.walk", LA("@clip"));

            CALL(t, current);
            REQUIRE_NO_MSG(t);
            CALL_N(t, next, 2);
            REQUIRE_NO_MSG(t);

            t.sendList(LF(1.0, 2.0, 3.0));

            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(1));

            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(3));
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(3));
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);

            CALL_N(t, prev, 2);
            REQUIRE_LIST_MSG(t, LF(1));

            CALL_N(t, prev, 4);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);
        }

        SECTION("length 2")
        {
            TestListWalk t("list.walk", LA("@clip", "@length", 2));
            REQUIRE_PROPERTY_FLOAT(t, @clip, 1);

            CALL(t, current);
            REQUIRE_NO_MSG(t);
            CALL(t, next);
            REQUIRE_NO_MSG(t);

            t.sendList(LF(1.0, 2.0, 3.0, 4.0, 5.0));

            CALL(t, current);
            REQUIRE_LIST_MSG(t, AtomList(1, 2));

            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, AtomList(3, 4));
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, AtomList(5, 5));
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, AtomList(5, 5));
            REQUIRE_INDEX(t, 4);

            CALL_N(t, prev, 3);
            REQUIRE_LIST_MSG(t, AtomList(2, 3));
            CALL_N(t, prev, 3);
            REQUIRE_LIST_MSG(t, AtomList(1, 2));
            REQUIRE_INDEX(t, 0);
        }
    }
}
