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
#include "../string/string_split.h"
#include "test_base.h"
#include "ceammc_format.h"
#include "ceammc_pd.h"

#include "catch.hpp"

using namespace ceammc;

typedef TestExternal<StringSplit> StringSplitTest;

#define DSTR(l) Data(new DataTypeString(l))

static CanvasPtr cnv = PureData::instance().createTopCanvas("test_canvas");

TEST_CASE("string.split", "[external]")
{
    setup_string0x2esplit();

    SECTION("process")
    {
        StringSplitTest t("str.split");
        REQUIRE(t.numInlets() == 1);
        REQUIRE(t.numOutlets() == 1);

        WHEN_SEND_BANG_TO(0, t);
        REQUIRE_NO_MSG(t);

        WHEN_SEND_FLOAT_TO(0, t, 0);
        REQUIRE_NO_MSG(t);

        WHEN_SEND_LIST_TO(0, t, L());
        REQUIRE_NO_MSG(t);

        WHEN_SEND_LIST_TO(0, t, LF(1));
        REQUIRE_NO_MSG(t);

        WHEN_SEND_LIST_TO(0, t, LA("a", "b"));
        REQUIRE_NO_MSG(t);

        REQUIRE_PROPERTY(t, @sep, "");

        WHEN_SEND_TDATA_TO(0, t, DataTypeString("abcde"));
        REQUIRE(t.hasNewMessages(0));
        REQUIRE(t.lastMessage(0).isList());
        REQUIRE(to_string(t.lastMessage(0).listValue()) == "a b c d e");
    }

    SECTION("data")
    {
        StringSplitTest t("str.split", LA(":"));
        REQUIRE_PROPERTY(t, @sep, ":");

        WHEN_SEND_TDATA_TO(0, t, DataTypeString("ab:cde:"));
        REQUIRE(t.hasNewMessages(0));
        REQUIRE(t.lastMessage(0).isList());
        REQUIRE(to_string(t.lastMessage(0).listValue()) == "ab cde");

        WHEN_SEND_TDATA_TO(0, t, DataTypeString("ab:cde:"));
        REQUIRE(t.hasNewMessages(0));
        REQUIRE(t.lastMessage(0).isList());
        REQUIRE(to_string(t.lastMessage(0).listValue()) == "ab cde");
    }

    SECTION("symbol")
    {
        StringSplitTest t("str.split", LA(":"));
        WHEN_SEND_SYMBOL_TO(0, t, "A:B:C");

        REQUIRE(t.hasNewMessages(0));
        REQUIRE(t.lastMessage(0).isList());
        REQUIRE(to_string(t.lastMessage(0).listValue()) == "A B C");
    }

    SECTION("create")
    {
        SECTION("empty")
        {
            StringSplitTest t("str.split", L());
            REQUIRE_PROPERTY(t, @sep, "");
        }

        SECTION("list")
        {
            StringSplitTest t("str.split", LA("A", "B"));
            REQUIRE_PROPERTY(t, @sep, "A");
        }

        SECTION("space")
        {
            StringSplitTest t("str.split", LA("\"", "\""));
            REQUIRE_PROPERTY(t, @sep, " ");
        }
    }

    SECTION("@sep")
    {
        StringSplitTest t("str.split", L());
        REQUIRE_PROPERTY(t, @sep, "");

        t.setProperty("@sep", LF(123));
        REQUIRE_PROPERTY(t, @sep, "123");

        t.setProperty("@sep", L());
        REQUIRE_PROPERTY(t, @sep, "");

        t.setProperty("@sep", LA("A", "B"));
        REQUIRE_PROPERTY(t, @sep, "A");
    }
}
