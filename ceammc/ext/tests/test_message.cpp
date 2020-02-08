/*****************************************************************************
 * Copyright 2016 Serge Poltavsky. All rights reserved.
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

#include "ceammc_message.h"
#include "test_common.h"

using namespace ceammc;

TEST_CASE("Message", "[ceammc::Message]")
{
    Message v1;
    REQUIRE(v1.isNone());
    REQUIRE(v1.type() == Message::NONE);
    REQUIRE(v1 != Message());

    Message v2(1.f);
    REQUIRE(v2.isFloat());

    Message v3(gensym("a"));
    REQUIRE(v3.isSymbol());

    t_atom atoms[2];
    SETFLOAT(&atoms[0], 1);
    SETSYMBOL(&atoms[1], gensym("b"));
    Message v4(2, &atoms[0]);
    REQUIRE(v4.isList());

    Message v5(gensym("c"), 2, &atoms[0]);
    REQUIRE(v5.isAny());

    Message v6(Atom(1.f));
    REQUIRE(v6.isFloat());
    Message v7(Atom(gensym("b")));
    REQUIRE(v7.isSymbol());

    Message v8(DataPtr(new IntData(123)).asAtom());
    REQUIRE(v8.isData());

    Message v9(AtomList(DataPtr(new IntData(123)).asAtom()));
    REQUIRE(v9.isData());

    Message v10(AtomList(DataPtr(new IntData(123)).asAtom()));
    REQUIRE(v10.isData());

    Message v11(AtomList(DataPtr(new IntData(124)).asAtom()));
    REQUIRE(v11.isData());

    Message v12(&s_bang);
    REQUIRE(v12.isBang());

    SECTION("template")
    {
        Message m("msg", 1, 2, 3);
        REQUIRE(m.isAny());
        REQUIRE(m.atomValue() == A("msg"));
        REQUIRE(m.listValue() == LF(1, 2, 3));
    }

    SECTION("compare")
    {
        REQUIRE(v1 == v1);
        REQUIRE(v2 == v2);
        REQUIRE(v3 == v3);
        REQUIRE(v4 == v4);
        REQUIRE(v5 == v5);

        REQUIRE(v1 != v2);
        REQUIRE(v1 != v3);
        REQUIRE(v1 != v4);
        REQUIRE_FALSE(v1 == v5);

        REQUIRE(v2 != v3);
        REQUIRE(v2 != v4);
        REQUIRE(v2 != v5);

        REQUIRE(v3 != v4);
        REQUIRE(v3 != v5);

        REQUIRE(v4 != v5);

        REQUIRE(v9 == v10);
        REQUIRE(v10 != v11);
    }

    SECTION("compare2")
    {
        REQUIRE(v2 == Message(1.f));
        REQUIRE(v2 != Message(1.0001f));

        REQUIRE(v3 == Message(gensym("a")));
        REQUIRE(v3 != Message(gensym("b")));

        AtomList l1;
        REQUIRE(v4 != Message(l1));
        l1.append(1);
        l1.append(gensym("b"));
        REQUIRE(v4 == Message(l1));

        REQUIRE(v5 != Message(gensym("a"), l1));
        REQUIRE(v5 != Message(gensym("c"), L()));
        REQUIRE(v5 == Message(gensym("c"), l1));

        AtomList l2;
        l2.append(gensym("a"));
        l2.append(gensym("b"));
        l2.append(gensym("c"));

        Message c1(l2);
        Message c2(gensym("a"), l2);
        REQUIRE(c1 != c2);
    }

    SECTION("set")
    {
        v1.setFloat(1.f);
        REQUIRE(v1.isFloat());
        v1.setSymbol(gensym("a"));
        REQUIRE(v1.isSymbol());
        v1.setList(L());
        REQUIRE(v1.isList());
        v1.setAny(gensym("b"), L());
        REQUIRE(v1.isAny());
        v1.setList(2, &atoms[0]);
        REQUIRE(v1.isList());
        v1.setAny(gensym("c"), 2, &atoms[0]);
        REQUIRE(v1.isAny());

        v1.setAtom(Atom(1.f));
        REQUIRE(v1.isFloat());
        v1.setAtom(Atom(gensym("a")));
        REQUIRE(v1.isSymbol());

        v1.setList(AtomList(DataPtr(new IntData(123)).asAtom()));
        REQUIRE(v1.isData());
    }

    SECTION("test any value")
    {
        AtomList ref(gensym("a"));
        ref.append(1);
        ref.append(2);

        Message msg(gensym("a"), AtomList(1, 2));
        REQUIRE(msg.anyValue() == ref);
    }

    SECTION("data")
    {
        SECTION("valid")
        {
            DataPtr d0(new IntData(123));
            Message m(d0.asAtom());

            REQUIRE(m.isData());
            REQUIRE_FALSE(m.dataValue().isNull());
            REQUIRE(m.dataValue()->isEqual(d0.data()));
            REQUIRE(m.dataValue() == d0);
        }

        SECTION("invalid")
        {
            Message m(123);
            REQUIRE_FALSE(m.isData());
            REQUIRE(m.dataValue().isNull());
        }

        SECTION("ownership")
        {
            Message m(DataPtr(new IntData(123)).asAtom());
            REQUIRE(m.isData());
            REQUIRE(m.dataValue().isValid());
            REQUIRE(m.dataValue() == DataPtr(new IntData(123)));
        }
    }
}
