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
#include "ceammc_platform.h"
#include "path_search.h"
#include "test_path_base.h"

#include <chrono>
#include <thread>

PD_COMPLETE_TEST_SETUP(PathSearch, path, search)

TEST_CASE("path.search", "[externals]")
{
    pd_test_init();
    test::pdPrintToStdError();

    SECTION("init")
    {
        SECTION("default")
        {
            TExt t("path.search");
            REQUIRE(t.numInlets() == 1);
            REQUIRE(t.numOutlets() == 2);

            REQUIRE_PROPERTY(t, @async, 1);
            REQUIRE_PROPERTY(t, @depth, 0.);
            REQUIRE_PROPERTY(t, @paths, L());
            REQUIRE_PROPERTY(t, @home, 1);
            REQUIRE_PROPERTY(t, @std, 1);
        }

        SECTION("args")
        {
            TExt t("path.search", LA(TEST_DATA_DIR));
            REQUIRE_PROPERTY(t, @paths, LA(TEST_DATA_DIR));
        }

        SECTION("args quoted")
        {
            TExt t("path.search", AtomList::parseString("\"a path with spaces\""));
            REQUIRE_PROPERTY(t, @paths, LA("a path with spaces"));
        }

        SECTION("args many quoted")
        {
            TExt t("path.search", AtomList::parseString("\"a path with spaces\" \"second path\""));
            REQUIRE_PROPERTY(t, @paths, LA("a path with spaces", "second path"));
        }
    }

    SECTION("simple @sync search in user dir")
    {
        TExt t("path.search", "@sync");
        REQUIRE_PROPERTY(t, @async, 0.);

        t << "test_data0.mp3";

        t.schedTicks(200);
        REQUIRE(t.hasOutputAt(1));
        REQUIRE(t.isOutputBangAt(1));

        t->setProperty("@paths", LA(TEST_DATA_DIR));
        t->dump();

        t << "test_data0.mp3";

        REQUIRE(t.hasOutputAt(0));
        REQUIRE(t.outputSymbolAt(0)->s_name == std::string(TEST_DATA_DIR "/test_data0.mp3"));
    }

    SECTION("simple @sync search in home dir")
    {
        TExt t("path.search", "@sync");
        REQUIRE_PROPERTY(t, @async, 0.);

        std::string path0 = platform::home_directory() + "/.bashrc";
        std::string path1 = platform::home_directory() + "/.vimrc";

        if (platform::path_exists(path0.c_str())) {
            t << ".bashrc";
            t.schedTicks(200);
            REQUIRE(t.hasOutputAt(0));
            REQUIRE(t.outputSymbolAt(0) == SYM(path0.c_str()));

            // no home search
            t->setProperty("@home", LF(0));
            t << ".bashrc";
            REQUIRE(t.hasOutputAt(1));
            REQUIRE(t.isOutputBangAt(1));
        }

        if (platform::path_exists(path1.c_str())) {
            t->setProperty("@home", LF(1));

            t << ".vimrc";
            t.schedTicks(200);
            REQUIRE(t.hasOutputAt(0));
            REQUIRE(t.outputSymbolAt(0) == SYM(path1.c_str()));
        }
    }

    SECTION("recursive @sync search in user dir")
    {
        TExt t("path.search", PROJECT_SOURCE_DIR "/ceammc/ext", "@sync", "@depth", 0);
        REQUIRE_PROPERTY(t, @async, 0.);

        const auto fname = platform::basename(__FILE__);
        const std::string full_name = TEST_SRC_DIR "/" + fname;

#ifndef __APPLE__
        // not enough recursion depth
        t << fname.c_str();
        REQUIRE(t.hasOutputAt(1));
        REQUIRE(t.isOutputBangAt(1));

        // still not enough
        t->setProperty("@depth", LF(1));
        t << fname.c_str();
        REQUIRE(t.hasOutputAt(1));
        REQUIRE(t.isOutputBangAt(1));
#endif

        // should be ok
        t->setProperty("@depth", LF(2));
        t << fname.c_str();

        REQUIRE(t.hasOutputAt(0));
        REQUIRE(t.outputSymbolAt(0) == SYM(full_name.c_str()));

        // also ok
        t->setProperty("@depth", LF(-1));
        t << fname.c_str();

        REQUIRE(t.hasOutputAt(0));
        REQUIRE(t.outputSymbolAt(0) == SYM(full_name.c_str()));
    }
}
