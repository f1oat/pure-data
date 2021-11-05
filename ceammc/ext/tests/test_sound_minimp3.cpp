/*****************************************************************************
 * Copyright 2021 Serge Poltavsky. All rights reserved.
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

#include "catch.hpp"
#include "ceammc_loader_minimp3.h"
#include "ceammc_sound.h"

using namespace ceammc;
using namespace ceammc::sound;

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "."
#endif

TEST_CASE("minimp3", "[ceammc_sound]")
{
    SECTION("supportedFormats")
    {
        FormatList lst = MiniMp3::supportedFormats();
        REQUIRE(lst.size() == 1);
        REQUIRE(lst[0].first == "MP3");
    }

    SECTION("load constant rate")
    {
        MiniMp3 loader(TEST_DATA_DIR "/test_data0.mp3");
        REQUIRE(loader.channels() == 1);
        REQUIRE(loader.filename() == TEST_DATA_DIR "/test_data0.mp3");
        REQUIRE(loader.isOpened());
        REQUIRE(loader.sampleRate() == 44100);
        REQUIRE(loader.sampleCount() == 441);

        t_word dest[1024];
        auto rc = loader.read(dest, 512, 0, 0, 1024);
        REQUIRE(rc == 441);

        // invalid channel
        REQUIRE(loader.read(dest, 512, 1, 0, 1000) == -1);

        REQUIRE(loader.read(dest, 100, 0, 10, 1000) == 100);
        REQUIRE(loader.read(dest, 1000, 0, 10, 1000) == 431);
    }

    SECTION("load vbr")
    {
        MiniMp3 loader(TEST_DATA_DIR "/test_data0_vbr.mp3");
        REQUIRE(loader.channels() == 1);
        REQUIRE(loader.filename() == TEST_DATA_DIR "/test_data0_vbr.mp3");
        REQUIRE(loader.isOpened());
        REQUIRE(loader.sampleRate() == 44100);
        REQUIRE(loader.sampleCount() == 441);

        t_word dest[1024];
        auto rc = loader.read(dest, 512, 0, 0, 1024);
        REQUIRE(rc == 441);

        // invalid channel
        REQUIRE(loader.read(dest, 512, 1, 0, 1000) == -1);

        REQUIRE(loader.read(dest, 100, 0, 10, 1000) == 100);
        REQUIRE(loader.read(dest, 1000, 0, 10, 1000) == 431);
    }

    SECTION("misc")
    {
        SECTION("cbr")
        {
            SECTION("1")
            {
                MiniMp3 loader(TEST_DATA_DIR "/mp3/test_1ch_12000_128.mp3");
                REQUIRE(loader.channels() == 1);
                REQUIRE(loader.isOpened());
                REQUIRE(loader.sampleRate() == 12000);
                REQUIRE(loader.sampleCount() == 12000);

                t_word dest[3000];
                auto rc = loader.read(dest, 3000, 0, 1000, 3000);
                REQUIRE(rc == 3000);

                // from test_1ch_12000_128_off1000_len32.dat
                REQUIRE(dest[0].w_float == Approx(-0.58001708984).margin(0.0001));
                REQUIRE(dest[1].w_float == Approx(-0.64694213867).margin(0.0001));
                REQUIRE(dest[2].w_float == Approx(-0.66976928711).margin(0.0001));
                REQUIRE(dest[3].w_float == Approx(-0.64694213867).margin(0.0001));
                REQUIRE(dest[4].w_float == Approx(-0.58004760742).margin(0.0001));
                REQUIRE(dest[5].w_float == Approx(-0.4736328125).margin(0.0001));
                REQUIRE(dest[6].w_float == Approx(-0.33493041992).margin(0.0001));
                REQUIRE(dest[7].w_float == Approx(-0.17340087891).margin(0.0001));
                REQUIRE(dest[8].w_float == Approx(-6.103515625e-05).margin(0.0001));
            }

            SECTION("1")
            {
                MiniMp3 loader(TEST_DATA_DIR "/mp3/test_1ch_44100_192.mp3");
                REQUIRE(loader.channels() == 1);
                REQUIRE(loader.isOpened());
                REQUIRE(loader.sampleRate() == 44100);
                REQUIRE(loader.sampleCount() == 44100);

                t_word dest[512];
                auto rc = loader.read(dest, 512, 0, 1000, 3000);
                REQUIRE(rc == 512);

                // from test_1ch_44100_192_off1000_len32.dat
                REQUIRE(dest[0].w_float == Approx(0.29229736328).margin(0.0001));
                REQUIRE(dest[1].w_float == Approx(0.32705688477).margin(0.0001));
                REQUIRE(dest[2].w_float == Approx(0.36071777344).margin(0.0001));
                REQUIRE(dest[3].w_float == Approx(0.39324951172).margin(0.0001));
                REQUIRE(dest[4].w_float == Approx(0.42446899414).margin(0.0001));
                REQUIRE(dest[5].w_float == Approx(0.45434570312).margin(0.0001));
                REQUIRE(dest[6].w_float == Approx(0.48272705078).margin(0.0001));
                REQUIRE(dest[7].w_float == Approx(0.50952148438).margin(0.0001));
                REQUIRE(dest[8].w_float == Approx(0.53469848633).margin(0.0001));
            }

            SECTION("2ch: first")
            {
                MiniMp3 loader(TEST_DATA_DIR "/mp3/test_2ch_44100_192.mp3");
                REQUIRE(loader.channels() == 2);
                REQUIRE(loader.isOpened());
                REQUIRE(loader.sampleRate() == 44100);
                REQUIRE(loader.sampleCount() == 88200);

                t_word dest[1000];
                auto rc = loader.read(dest, 1000, 0, 1000, 10000);
                REQUIRE(rc == 1000);

                // from test_2ch_44100_192_off1000_len32.dat
                REQUIRE(dest[0].w_float == Approx(0.29229736328).margin(0.0001));
                REQUIRE(dest[1].w_float == Approx(0.32705688477).margin(0.0001));
                REQUIRE(dest[2].w_float == Approx(0.36071777344).margin(0.0001));
                REQUIRE(dest[3].w_float == Approx(0.39324951172).margin(0.0001));
                REQUIRE(dest[4].w_float == Approx(0.42446899414).margin(0.0001));
                REQUIRE(dest[5].w_float == Approx(0.45434570312).margin(0.0001));
                REQUIRE(dest[6].w_float == Approx(0.48272705078).margin(0.0001));
                REQUIRE(dest[7].w_float == Approx(0.50952148438).margin(0.0001));
                REQUIRE(dest[8].w_float == Approx(0.53469848633).margin(0.0001));
            }

            SECTION("2ch: second")
            {
                MiniMp3 loader(TEST_DATA_DIR "/mp3/test_2ch_44100_192.mp3");
                REQUIRE(loader.channels() == 2);
                REQUIRE(loader.isOpened());
                REQUIRE(loader.sampleRate() == 44100);
                REQUIRE(loader.sampleCount() == 88200);

                t_word dest[1000];
                auto rc = loader.read(dest, 1000, 1, 1000, 10000);
                REQUIRE(rc == 1000);

                // from test_2ch_44100_192_off1000_len32.dat
                REQUIRE(dest[0].w_float == Approx(0.29229736328).margin(0.0001));
                REQUIRE(dest[1].w_float == Approx(0.32705688477).margin(0.0001));
                REQUIRE(dest[2].w_float == Approx(0.36071777344).margin(0.0001));
                REQUIRE(dest[3].w_float == Approx(0.39324951172).margin(0.0001));
                REQUIRE(dest[4].w_float == Approx(0.42446899414).margin(0.0001));
                REQUIRE(dest[5].w_float == Approx(0.45434570312).margin(0.0001));
                REQUIRE(dest[6].w_float == Approx(0.48272705078).margin(0.0001));
                REQUIRE(dest[7].w_float == Approx(0.50952148438).margin(0.0001));
                REQUIRE(dest[8].w_float == Approx(0.53469848633).margin(0.0001));
            }
        }
    }
}