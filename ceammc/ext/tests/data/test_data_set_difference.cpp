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
#include "../data/set_difference.h"
#include "test_base.h"
#include "catch.hpp"
#include "ceammc_pd.h"

#include <stdio.h>

typedef TestExternal<SetDifference> SetDifferenceTest;

#define REQUIRE_SET_OUTPUT(t, set)                                \
    {                                                             \
        REQUIRE_NEW_DATA_AT_OUTLET(0, t);                         \
        const DataTypeSet* s = t.typedLastDataAt<DataTypeSet>(0); \
        REQUIRE(s != 0);                                          \
        REQUIRE(*s == set);                                       \
    }

#define DSET(l) DataPtr(new DataTypeSet(l))
#define DINT(v) DataPtr(new IntData(v))
#define DSTR(v) DataPtr(new StrData(v))

static CanvasPtr cnv = PureData::instance().createTopCanvas("test_canvas");

TEST_CASE("set.diff", "[externals]")
{
    setup_set0x2edifference();

    SECTION("create")
    {
        SECTION("empty")
        {
            SetDifferenceTest t("set.diff");
            REQUIRE(t.numInlets() == 2);
            REQUIRE(t.numOutlets() == 1);

            // {1,2} \ {} = {1,2}
            WHEN_SEND_TDATA_TO(0, t, DataTypeSet(LF(1, 2)));
            REQUIRE_SET_OUTPUT(t, DataTypeSet(LF(1, 2)));
        }

        SECTION("args")
        {
            // {2,4} \ {1,2,3} = {4}
            SetDifferenceTest t("set.diff", LF(1, 2, 3));
            WHEN_SEND_TDATA_TO(0, t, DataTypeSet(LF(2, 4)));
            REQUIRE_SET_OUTPUT(t, DataTypeSet(LF(4)));

            // {1,2,3} \ {1,2,3} = {}
            WHEN_SEND_TDATA_TO(0, t, DataTypeSet(LF(1, 2, 3)));
            REQUIRE_SET_OUTPUT(t, DataTypeSet());

            // {} \ {1,2,3} = {}
            WHEN_SEND_TDATA_TO(0, t, DataTypeSet());
            REQUIRE_SET_OUTPUT(t, DataTypeSet());
        }
    }

    SECTION("do")
    {
        SetDifferenceTest t("set.diff");

        // {3,4,5} \ {} = {3,4,5}
        WHEN_SEND_TDATA_TO(0, t, DataTypeSet(LF(3, 4, 5)));
        REQUIRE_SET_OUTPUT(t, DataTypeSet(LF(3, 4, 5)));

        // {} \ {} = {}
        WHEN_SEND_TDATA_TO(0, t, DataTypeSet());
        REQUIRE_SET_OUTPUT(t, DataTypeSet());

        // {3,5} \ {1,4} = {}
        WHEN_SEND_LIST_TO(1, t, LF(1, 4));
        WHEN_SEND_TDATA_TO(0, t, DataTypeSet(LF(3, 5)));
        REQUIRE_SET_OUTPUT(t, DataTypeSet(LF(3, 5)));
    }

    SECTION("do data")
    {
        SetDifferenceTest t("set.diff");

        // {2,4} \ {1,3} = {2,4}
        WHEN_SEND_LIST_TO(1, t, LA(DINT(1), DINT(3)));
        WHEN_SEND_TDATA_TO(0, t, DataTypeSet(LA(DINT(2), DINT(4))));
        REQUIRE_SET_OUTPUT(t, DataTypeSet(LA(DINT(2), DINT(4))));

        // {2,4} \ {1,3} = {2,4}
        WHEN_SEND_LIST_TO(1, t, LA(DSET(LA(DINT(1), DINT(3))).asAtom()));
        WHEN_SEND_TDATA_TO(0, t, DataTypeSet(LA(DINT(2), DINT(4))));
        REQUIRE_SET_OUTPUT(t, DataTypeSet(LA(DINT(2), DINT(4))));

        // {2,3,4,5} \ {1,3} = {2,4,5}
        WHEN_SEND_TDATA_TO(0, t, DataTypeSet(LA(DINT(2), DINT(4), DINT(3), DINT(5))));
        REQUIRE_SET_OUTPUT(t, DataTypeSet(LA(DINT(2), DINT(5), DINT(4))));
    }

    SECTION("do list")
    {
        SetDifferenceTest t("set.diff");

        // {1,3} \ {1,2} = {3}
        WHEN_SEND_LIST_TO(1, t, LF(1, 2));
        WHEN_SEND_LIST_TO(0, t, LF(1, 3));
        REQUIRE_SET_OUTPUT(t, DataTypeSet(LF(3)));

        // {} \ {} = {}
        WHEN_SEND_LIST_TO(1, t, L());
        WHEN_SEND_LIST_TO(0, t, L());
        REQUIRE_SET_OUTPUT(t, DataTypeSet());
    }
}