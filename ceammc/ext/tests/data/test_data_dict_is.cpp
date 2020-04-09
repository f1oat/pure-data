/*****************************************************************************
 * Copyright 2020 Serge Poltavsky. All rights reserved.
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
#include "dict_is.h"
#include "test_data_base.h"

PD_COMPLETE_TEST_SETUP(IsDict, is, dict)

TEST_CASE("is_dict", "[externals]")
{
    pd_test_init();

    SECTION("create")
    {
        TExt t("is_dict");
        REQUIRE(t.numInlets() == 1);
        REQUIRE(t.numOutlets() == 1);
    }

    SECTION("do")
    {
        TExt t("is_dict");

        t.bang();
        REQUIRE(!t.hasOutput());
    }
}
