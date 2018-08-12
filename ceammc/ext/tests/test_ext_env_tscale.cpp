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
#include "../env/env_tscale.h"
#include "test_base.h"
#include "catch.hpp"
#include "ceammc_pd.h"

#include <stdio.h>

typedef TestExternal<EnvTimeScale> EnvTimeScaleTest;

static CanvasPtr cnv = PureData::instance().createTopCanvas("test_canvas");

#define REQUIRE_ENV_OUTPUT(t, env)                                   \
    {                                                                \
        REQUIRE_NEW_DATA_AT_OUTLET(0, t);                            \
        const DataTypeEnv* env0 = t.typedLastDataAt<DataTypeEnv>(0); \
        REQUIRE(env0 != 0);                                          \
        REQUIRE(*env0 == env);                                       \
        t.cleanAllMessages();                                        \
    }

TEST_CASE("env.tscale", "[externals]")
{
    SECTION("init")
    {
        setup_env_tscale();

        SECTION("empty")
        {
            EnvTimeScaleTest t("env.tscale");
            REQUIRE(t.numInlets() == 1);
            REQUIRE(t.numOutlets() == 1);
            REQUIRE_PROPERTY(t, @scale, 1.f);
        }

        SECTION("float")
        {
            EnvTimeScaleTest t("env.tscale", LF(2));
            REQUIRE_PROPERTY(t, @scale, 2.f);
        }

        SECTION("@prop")
        {
            EnvTimeScaleTest t("env.tscale", LA("@scale", 3));
            REQUIRE_PROPERTY(t, @scale, 3.f);
        }

        SECTION("invalid")
        {
            EnvTimeScaleTest t("env.tscale", LA("@scale", -3));
            REQUIRE_PROPERTY(t, @scale, 1.f);
        }
    }

    SECTION("on data")
    {
        EnvTimeScaleTest t("env", LA("@scale", 2));
        WHEN_SEND_TDATA_TO(0, t, DataTypeEnv());
        REQUIRE_ENV_OUTPUT(t, DataTypeEnv());

        DataTypeEnv env;
        env.setAR(1000, 2000);

        {
            WHEN_SEND_TDATA_TO(0, t, env);

            DataTypeEnv out;
            out.setAR(2000, 4000);
            REQUIRE_ENV_OUTPUT(t, out);
        }
    }
}
