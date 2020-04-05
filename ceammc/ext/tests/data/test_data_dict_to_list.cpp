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
#include "../data/dict_to_list.h"
#include "ceammc_format.h"
#include "datatype_dict.h"
#include "test_base.h"
#include "test_external.h"

PD_COMPLETE_TEST_SETUP(DictToList, dict, to_list)

TEST_CASE("dict.to_list", "[externals]")
{
    pd_test_init();

    SECTION("create")
    {
        TestDictToList t("dict.to_list");
        REQUIRE(t.numInlets() == 1);
        REQUIRE(t.numOutlets() == 1);
    }

    SECTION("do")
    {
        TestExtDictToList t("dict.to_list");
        REQUIRE(t.object());

        t.send(DataPtr(new DataTypeDict()));
        REQUIRE(t.hasOutput());
        REQUIRE(t.isOutputListAt(0));
        REQUIRE(t.outputListAt(0) == L());

        t.send(DataPtr(new DataTypeDict("[a:b]")));
        REQUIRE(t.hasOutput());
        REQUIRE(t.isOutputListAt(0));
        REQUIRE(t.outputListAt(0) == LA("a", "b"));

        t.send(DataPtr(new DataTypeDict("[a: b c d]")));
        REQUIRE(t.hasOutput());
        REQUIRE(t.isOutputListAt(0));
        REQUIRE(t.outputListAt(0) == LA("a", "b", "c", "d"));

        t.send(DataPtr(new DataTypeDict("[a: b c d][e: 123]")));
        REQUIRE(t.hasOutput());
        REQUIRE(t.isOutputListAt(0));
        REQUIRE(t.outputListAt(0) == LA("a", "b", "c", "d", "e", 123));
    }

    SECTION("alias")
    {
        TestExtDictToList t("dict->list");
        REQUIRE(t.object());
    }
}
