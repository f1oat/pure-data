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
#include "test_base.h"
#include "test_external.h"

PD_COMPLETE_TEST_SETUP(ListWalk, list, walk)

#define REQUIRE_LIST_MSG(obj, lst)                     \
    {                                                  \
        REQUIRE(obj.hasNewMessages());                 \
        REQUIRE(obj.lastMessage().isList());           \
        REQUIRE(obj.lastMessage().listValue() == lst); \
    }

#define REQUIRE_PROP(obj, name, val)                   \
    {                                                  \
        Property* p = obj.property(gensym("@" #name)); \
        REQUIRE(p != 0);                               \
        REQUIRE(p->get() == val);                      \
    }

#define REQUIRE_INDEX(obj, idx)                         \
    {                                                   \
        REQUIRE(obj.p_index() == AtomList(float(idx))); \
    }

#define REQUIRE_NO_MSG(obj) REQUIRE_FALSE(obj.hasNewMessages())

TEST_CASE("list.walk 2", "[externals]")
{
    pd_test_init();

    SECTION("test fold")
    {
        SECTION("basic")
        {
            AtomList args(gensym("@fold"));
            TestListWalk t("list.walk", args);

            CALL(t, current);
            REQUIRE_NO_MSG(t);
            REQUIRE_INDEX(t, 0);
            CALL(t, next);
            REQUIRE_NO_MSG(t);
            REQUIRE_INDEX(t, 0);

            t.sendList(LF(1.0, 2.0, 3.0));
            REQUIRE_PROP(t, index, AtomList(0.f));

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
            REQUIRE_LIST_MSG(t, LF(2));
            REQUIRE_INDEX(t, 1);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);
            CALL(t, next);
            REQUIRE_LIST_MSG(t, LF(2));
            REQUIRE_INDEX(t, 1);

            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(2));
            REQUIRE_INDEX(t, 1);
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(3));
            REQUIRE_INDEX(t, 2);
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(2));
            REQUIRE_INDEX(t, 1);

            CALL(t, reset);
            REQUIRE_PROP(t, index, AtomList(0.f));
            CALL(t, prev);
            REQUIRE_PROP(t, index, LF(1));
            REQUIRE_LIST_MSG(t, LF(2));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(3));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(2));
            CALL(t, prev);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);
        }

        SECTION("step 2")
        {
            AtomList args(gensym("@fold"));
            TestListWalk t("list.walk", args);

            CALL(t, current);
            REQUIRE_NO_MSG(t);
            CALL_N(t, next, 2);
            REQUIRE_NO_MSG(t);

            t.sendList(LF(1.0, 2.0, 3.0));

            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(1));

            // 1 2 3 2 1 2 3 2 1
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(3));
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, LF(1));
            CALL_N(t, next, 3);
            REQUIRE_LIST_MSG(t, LF(2));
            REQUIRE_INDEX(t, 1);

            CALL_N(t, prev, 3);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);

            CALL_N(t, prev, 4);
            REQUIRE_LIST_MSG(t, LF(1));
            REQUIRE_INDEX(t, 0);
        }

        SECTION("length 2")
        {
            AtomList args(gensym("@fold"));
            args.append(AtomList(gensym("@length"), 2));
            TestListWalk t("list.walk", args);

            CALL(t, current);
            REQUIRE_NO_MSG(t);
            CALL(t, next);
            REQUIRE_NO_MSG(t);

            t.sendList(LF(1.0, 2.0, 3.0, 4.0, 5.0));

            CALL(t, current);
            REQUIRE_LIST_MSG(t, AtomList(1, 2));

            // 1 2 3 4 5 4 3 2 1 2 3 4 5 4 3 2 1
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, AtomList(3, 4));
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, AtomList(5, 4));
            CALL_N(t, next, 2);
            REQUIRE_LIST_MSG(t, AtomList(3, 2));

            CALL_N(t, prev, 3);
            REQUIRE_LIST_MSG(t, AtomList(4, 5));
            CALL_N(t, prev, 3);
            REQUIRE_LIST_MSG(t, AtomList(1, 2));
            CALL_N(t, prev, 3);
            REQUIRE_LIST_MSG(t, AtomList(4, 3));
        }
    }

    SECTION("test length < 1")
    {
        TestListWalk t("list.walk", L());
        REQUIRE(t.setProperty("@length", AtomList(0.f)));

        CALL(t, current);
        REQUIRE_NO_MSG(t);

        t.sendList(LF(1.0, 2.0));
        CALL(t, current);
        REQUIRE_NO_MSG(t);

        REQUIRE(t.setProperty("@length", AtomList(-10)));

        CALL(t, current);
        REQUIRE_NO_MSG(t);
    }

    SECTION("test @size")
    {
        TestListWalk t("list.walk", L());
        REQUIRE_PROP(t, size, AtomList(0.f));

        t.sendList(LF(1.0, 2.0));
        REQUIRE_PROP(t, size, LF(2));
    }

    SECTION("test bang")
    {
        TestListWalk t("list.walk", AtomList(1, 2));
        REQUIRE(t.property("@mode")->get() == AtomList(gensym("single")));
        REQUIRE(t.setProperty("@loop", L()));
        REQUIRE(t.property("@mode")->get() == AtomList(gensym("wrap")));

        t.sendBang();
        REQUIRE_LIST_MSG(t, LF(1));
        t.sendBang();
        REQUIRE_LIST_MSG(t, LF(2));
        t.sendBang();
        REQUIRE_LIST_MSG(t, LF(1));
    }

    SECTION("test @direction")
    {
        TestListWalk t("list.walk", LF(1.0, 2.0, 3.0));
        REQUIRE_PROP(t, direction, AtomList(1.f));
        REQUIRE(t.setProperty("@direction", AtomList(0.f)));
        REQUIRE(t.setProperty("@loop", L()));

        t.sendBang();
        REQUIRE_LIST_MSG(t, LF(1));
        t.sendBang();
        REQUIRE_LIST_MSG(t, LF(3));
        t.sendBang();
        REQUIRE_LIST_MSG(t, LF(2));
        CALL(t, prev);
        REQUIRE_LIST_MSG(t, LF(2));
        CALL(t, next);
        REQUIRE_LIST_MSG(t, LF(1));

        REQUIRE(t.setProperty("@direction", AtomList(1.f)));
        t.sendBang();
        REQUIRE_LIST_MSG(t, LF(1));
        t.sendBang();
        REQUIRE_LIST_MSG(t, LF(2));
        CALL(t, prev);
        REQUIRE_LIST_MSG(t, LF(2));
        CALL(t, next);
        REQUIRE_LIST_MSG(t, LF(3));
    }

    SECTION("test float")
    {
        SECTION("forward")
        {
            TestListWalk t("list.walk", LF(1.0, 2.0, 3.0, 4.0, 5.0));
            REQUIRE_PROP(t, direction, AtomList(1.f));
            REQUIRE(t.setProperty("@loop", L()));

            t.sendFloat(1);
            REQUIRE_LIST_MSG(t, LF(1));
            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(2));
            t.sendFloat(0);
            REQUIRE_LIST_MSG(t, LF(2));

            t.sendFloat(2);
            REQUIRE_LIST_MSG(t, LF(2));
            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(4));

            t.sendFloat(-4);
            REQUIRE_LIST_MSG(t, LF(4));
            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(5));

            t.sendFloat(-1);
            REQUIRE_LIST_MSG(t, LF(5));
            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(4));
        }

        SECTION("backward")
        {
            TestListWalk t("list.walk", LF(1.0, 2.0, 3.0, 4.0, 5.0));
            REQUIRE_PROP(t, direction, AtomList(1.f));
            REQUIRE(t.setProperty("@direction", AtomList(0.f)));
            REQUIRE(t.setProperty("@loop", L()));

            t.sendFloat(1);
            REQUIRE_LIST_MSG(t, LF(1));
            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(5));

            t.sendFloat(2);
            REQUIRE_LIST_MSG(t, LF(5));
            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(3));

            t.sendFloat(-4);
            REQUIRE_LIST_MSG(t, LF(3));
            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(2));

            t.sendFloat(-1);
            REQUIRE_LIST_MSG(t, LF(2));
            CALL(t, current);
            REQUIRE_LIST_MSG(t, LF(3));
        }
    }

    SECTION("test set index")
    {
        TestListWalk t("list.walk", L());
        t.p_set_index(LF(2));
        REQUIRE_PROP(t, index, AtomList(0.0f));
        t.p_set_index(AtomList(-2));
        REQUIRE(t.p_index() == AtomList(0.0f));

        t.sendList(AtomList(2, 3));
        t.p_set_index(AtomList(0.0f));
        REQUIRE_PROP(t, index, AtomList(0.0f));
        t.p_set_index(AtomList(1.0f));
        REQUIRE_PROP(t, index, AtomList(1.0f));
        t.p_set_index(AtomList(2.0f));
        REQUIRE_PROP(t, index, AtomList(1.0f));
        t.p_set_index(AtomList(3.0f));
        REQUIRE_PROP(t, index, AtomList(1.0f));
        t.p_set_index(AtomList(4.0f));
        REQUIRE_PROP(t, index, AtomList(1.0f));

        t.p_set_index(AtomList(-1.0f));
        REQUIRE_PROP(t, index, AtomList(1.0f));
        t.p_set_index(AtomList(-2.0f));
        REQUIRE_PROP(t, index, AtomList(0.0f));
        t.p_set_index(AtomList(-3.0f));
        REQUIRE_PROP(t, index, AtomList(0.0f));
        t.p_set_index(AtomList(-4.0f));
        REQUIRE_PROP(t, index, AtomList(0.0f));
    }
}
