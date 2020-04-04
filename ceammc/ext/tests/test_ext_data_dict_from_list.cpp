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
#include "../data/dict_from_list.h"
#include "datatype_dict.h"
#include "test_base.h"
#include "test_external.h"

PD_COMPLETE_TEST_SETUP(DictFromList, dict, from_list)

TEST_CASE("dict.from_list", "[externals]")
{
    pd_test_init();

    SECTION("create")
    {
        SECTION("default")
        {
            TestDictFromList t("dict.from_list");
            REQUIRE(t.numInlets() == 1);
            REQUIRE(t.numOutlets() == 1);
            REQUIRE_PROPERTY_FLOAT(t, @step, 2);
        }

        SECTION("args")
        {
            TestDictFromList t("dict.from_list", LA(4));
            REQUIRE(t.numInlets() == 1);
            REQUIRE(t.numOutlets() == 1);
            REQUIRE_PROPERTY_FLOAT(t, @step, 4);
        }

        SECTION("@prop")
        {
            TestDictFromList t("dict.from_list", LA("@step", 3));
            REQUIRE(t.numInlets() == 1);
            REQUIRE(t.numOutlets() == 1);
            REQUIRE_PROPERTY_FLOAT(t, @step, 3);
        }

        SECTION("invalid")
        {
            TestDictFromList t("dict.from_list", LA(1));
            REQUIRE(t.numInlets() == 1);
            REQUIRE(t.numOutlets() == 1);
            REQUIRE_PROPERTY_FLOAT(t, @step, 2);
        }

        SECTION("invalid")
        {
            TestDictFromList t("dict.from_list", LA(-11));
            REQUIRE(t.numInlets() == 1);
            REQUIRE(t.numOutlets() == 1);
            REQUIRE_PROPERTY_FLOAT(t, @step, 2);
        }
    }

    SECTION("do")
    {
        TestExtDictFromList t("dict.from_list");
        REQUIRE(t.object());
        REQUIRE_PROPERTY_FLOAT(t, @step, 2);

        t.sendBang();
        REQUIRE(!t.hasOutput());
        t.sendFloat(22);
        REQUIRE(!t.hasOutput());
        t << "SYM";
        REQUIRE(!t.hasOutput());
        t.call("test");
        REQUIRE(!t.hasOutput());

        t << LA("a", "b", "c", "d");
        REQUIRE(t.hasOutput());
        REQUIRE(t.isOutputDataAt(0));
        REQUIRE(t.outputAtomAt(0) == DataPtr(new DataTypeDict("[a:b][c:d]")));

        t << LA("a", "b", "c");
        REQUIRE(t.hasOutput());
        REQUIRE(t.isOutputDataAt(0));
        REQUIRE(t.outputAtomAt(0) == DataPtr(new DataTypeDict("[a:b]")));

        t->setProperty("@step", LA(3));
        t << LA("a", "b", "c");
        REQUIRE(t.hasOutput());
        REQUIRE(t.isOutputDataAt(0));
        REQUIRE(t.outputAtomAt(0) == DataPtr(new DataTypeDict("[a:b c]")));

        t << LA("a", "b", "c", "d");
        REQUIRE(t.hasOutput());
        REQUIRE(t.isOutputDataAt(0));
        REQUIRE(t.outputAtomAt(0) == DataPtr(new DataTypeDict("[a:b c]")));
    }

    SECTION("alias")
    {
        TestExtDictFromList t("list->dict");
        REQUIRE(t.object());
    }
}
