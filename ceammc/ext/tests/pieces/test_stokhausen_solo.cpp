/*****************************************************************************
 * Copyright 2023 Serge Poltavski. All rights reserved.
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
#include "stok_solo_data.h"
#include "test_common.h"
#include "test_pieces_base.h"

using namespace solo;

TEST_CASE("pieces.stok_solo~", "[externals]")
{
    test::pdPrintToStdError();

    SECTION("init")
    {
        Scheme s;
        REQUIRE(s.lengthSec() == 0);
        REQUIRE_FALSE(s.setScheme(-1));
        REQUIRE_FALSE(s.setScheme(0));
        REQUIRE_FALSE(s.setScheme(3));
        REQUIRE_FALSE(s.setScheme(4));
        REQUIRE_FALSE(s.setScheme(5));
        REQUIRE_FALSE(s.setScheme(6));
        REQUIRE_FALSE(s.setScheme(7));

        REQUIRE(s.setScheme(2));

        REQUIRE(s.cycles_[0].cycleLength() == 108);
        REQUIRE(s.cycles_[1].cycleLength() == 168);
        REQUIRE(s.cycles_[2].cycleLength() == 66);
        REQUIRE(s.cycles_[3].cycleLength() == 85);
        REQUIRE(s.cycles_[4].cycleLength() == Approx(182.4));
        REQUIRE(s.cycles_[5].cycleLength() == Approx(136.8));

        REQUIRE(s.cycles_[0].periodLength() == 12);
        REQUIRE(s.cycles_[1].periodLength() == 24);
        REQUIRE(s.cycles_[2].periodLength() == 6);
        REQUIRE(s.cycles_[3].periodLength() == 8.5);
        REQUIRE(s.cycles_[4].periodLength() == Approx(30.4));
        REQUIRE(s.cycles_[5].periodLength() == Approx(17.1));

        REQUIRE(s.cycles_[0].periodCount() == 9);
        REQUIRE(s.cycles_[1].periodCount() == 7);
        REQUIRE(s.cycles_[2].periodCount() == 11);
        REQUIRE(s.cycles_[3].periodCount() == 10);
        REQUIRE(s.cycles_[4].periodCount() == 6);
        REQUIRE(s.cycles_[5].periodCount() == 8);

        REQUIRE(s.periodCount() == 51);
        REQUIRE(s.lengthSec() == Approx(746.2));
    }

    SECTION("EventList")
    {
        SoloEventList l;
        REQUIRE(l.empty());
        REQUIRE(l.timeToNextEvent() == -1);
        REQUIRE_FALSE(l.isValidNext());
        REQUIRE_FALSE(l.isValidCurrent());
        REQUIRE(l.nextPtr() == nullptr);
        REQUIRE(l.currentPtr() == nullptr);

        l.add(SoloEvent(CYCLE_A, TRACK_MIC1, SOLO_EVENT_ON, 10));
        l.reset();

        REQUIRE_FALSE(l.empty());
        REQUIRE(l.timeToNextEvent() == -1);
        REQUIRE_FALSE(l.isValidNext());
        REQUIRE(l.isValidCurrent());
        REQUIRE(l.nextPtr() == nullptr);
        REQUIRE(l.currentPtr());

        l.add(SoloEvent(CYCLE_A, TRACK_MIC2, SOLO_EVENT_OFF, 100));

        REQUIRE_FALSE(l.empty());
        REQUIRE(l.timeToNextEvent() == 90);
        REQUIRE(l.isValidNext());
        REQUIRE(l.isValidCurrent());
        REQUIRE(l.nextPtr());
        REQUIRE(l.currentPtr());

        l.moveNext({});
        REQUIRE(l.timeToNextEvent() == -1);
        REQUIRE_FALSE(l.isValidNext());
        REQUIRE(l.isValidCurrent());
        REQUIRE(l.nextPtr() == nullptr);
        REQUIRE(l.currentPtr());
        REQUIRE(l.currentPtr()->absTimeMsec() == 100);

        l.add(SoloEvent(CYCLE_A, TRACK_FB1, SOLO_EVENT_OFF, 0));
        l.add(SoloEvent(CYCLE_A, TRACK_FB2, SOLO_EVENT_ON, 0));
        l.add(SoloEvent(CYCLE_A, TRACK_OUT1, SOLO_EVENT_ON, 0));
        l.reset();
        l.moveSame({});
        REQUIRE(l.timeToNextEvent() == 10);
        REQUIRE(l.isValidNext());
        REQUIRE(l.isValidCurrent());
        REQUIRE(l.nextPtr());
        REQUIRE(l.currentPtr());
        REQUIRE(l.currentPtr()->track() == TRACK_OUT1);
    }

    SECTION("toString")
    {
        Scheme s(2);
        REQUIRE(s.scheme() == 2);
        REQUIRE(s.lengthSec() == Approx(746.2));
        REQUIRE(s.periodCount() == 51);
        REQUIRE(s.toString(0) == "_32_23_2_41312_12_1_11_2_E__21_11_21_3____8__3___6_");
        REQUIRE(s.periodTimeSec(0) == 0);
        REQUIRE(s.periodTimeSec(1) == 12);
        REQUIRE(s.periodTimeSec(2) == 24);
        REQUIRE(s.periodTimeSec(8) == 8 * 12);
        REQUIRE(s.periodTimeSec(9) == 9 * 12);
        REQUIRE(s.periodTimeSec(10) == 9 * 12 + 1 * 24);
        REQUIRE(s.periodTimeSec(11) == 9 * 12 + 2 * 24);
        REQUIRE(s.periodTimeSec(12) == 9 * 12 + 3 * 24);
        REQUIRE(s.periodTimeSec(13) == 9 * 12 + 4 * 24);
        REQUIRE(s.periodTimeSec(14) == 9 * 12 + 5 * 24);
        REQUIRE(s.periodTimeSec(15) == 9 * 12 + 6 * 24);
        REQUIRE(s.periodTimeSec(16) == 9 * 12 + 7 * 24);
        REQUIRE(s.periodTimeSec(17) == 9 * 12 + 7 * 24 + 1 * 6);
        REQUIRE(s.periodTimeSec(50) == Approx(729.1));
        REQUIRE(s.periodTimeSec(51) == Approx(746.2));
        REQUIRE(s.periodTimeSec(52) == 0);
    }

    SECTION("cycleBeginPeriodIndex")
    {
        Scheme s;
        s.setScheme(2);
        REQUIRE(s.cycleBeginPeriodIndex(CYCLE_A) == 0);
        REQUIRE(s.cycleBeginPeriodIndex(CYCLE_B) == 9);
        REQUIRE(s.cycleBeginPeriodIndex(CYCLE_C) == 16);
        REQUIRE(s.cycleBeginPeriodIndex(CYCLE_D) == 27);
        REQUIRE(s.cycleBeginPeriodIndex(CYCLE_E) == 37);
        REQUIRE(s.cycleBeginPeriodIndex(CYCLE_F) == 43);

        REQUIRE(s.cycleEndPeriodIndex(CYCLE_A) == 9);
        REQUIRE(s.cycleEndPeriodIndex(CYCLE_B) == 16);
        REQUIRE(s.cycleEndPeriodIndex(CYCLE_C) == 27);
        REQUIRE(s.cycleEndPeriodIndex(CYCLE_D) == 37);
        REQUIRE(s.cycleEndPeriodIndex(CYCLE_E) == 43);
        REQUIRE(s.cycleEndPeriodIndex(CYCLE_F) == 51);
    }

    SECTION("scheme2")
    {
        Scheme s;
        s.setScheme(2);
        auto p = s.periodAt(TRACK_OUT1, CYCLE_C, 0);
        REQUIRE(p->event == solo::EVENT_ON);
        REQUIRE(p->cycle() == CYCLE_C);
        REQUIRE(p->fullLengthTime() == 6);
        REQUIRE(p->attackTime() == 1.5);
        REQUIRE(p->releaseTime() == 3);

        p = s.periodAt(TRACK_OUT2, CYCLE_C, 0);
        REQUIRE(p->event == solo::EVENT_ON);
        REQUIRE(p->cycle() == CYCLE_C);
        REQUIRE(p->fullLengthTime() == 6);
        REQUIRE(p->attackTime() == 3);
        REQUIRE(p->releaseTime() == 4.5);
    }

    SECTION("perf")
    {
        SoloEventList e;

        e.addOn(Period(CYCLE_A, EVENT_ON, 1000).setPerf(1), TRACK_MIC1, 1000, 5);
        REQUIRE(e.size() == 3);
        REQUIRE(e.eventAt(0).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(1).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(2).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(0).period() == 5);
        REQUIRE(e.eventAt(1).period() == 5);
        REQUIRE(e.eventAt(2).period() == 5);
        e.clear();

        e.addOn(Period(CYCLE_A, EVENT_ON, 1000).setPerf(2), TRACK_MIC1, 1000, 6);
        REQUIRE(e.size() == 5);
        REQUIRE(e.eventAt(0).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(1).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(2).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(3).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(4).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(0).period() == 6);
        REQUIRE(e.eventAt(1).period() == 6);
        REQUIRE(e.eventAt(2).period() == 6);
        REQUIRE(e.eventAt(3).period() == 6);
        e.clear();
    }

    SECTION("EventList")
    {
        SoloEventList e;
        REQUIRE(e.empty());

        e.add(SoloEvent(CYCLE_A, TRACK_MIC1, SOLO_EVENT_ON, 100));
        REQUIRE(e.size() == 1);
        e.add(SoloEvent(CYCLE_A, TRACK_MIC2, SOLO_EVENT_ON, 100));
        REQUIRE(e.size() == 2);

        REQUIRE(e.data()[0].track() == TRACK_MIC1);
        REQUIRE(e.data()[1].track() == TRACK_MIC2);

        e.add(SoloEvent(CYCLE_A, TRACK_MIC1, SOLO_EVENT_OFF, 50));
        REQUIRE(e.size() == 3);
        REQUIRE(e.data()[0].type() == SOLO_EVENT_OFF);
        REQUIRE(e.data()[1].type() == SOLO_EVENT_ON);
        REQUIRE(e.data()[2].type() == SOLO_EVENT_ON);

        e.clear();
        REQUIRE(e.empty());

        e.addPeriod(Period(CYCLE_A, EVENT_OFF, 5), TRACK_MIC1, 2, 100);
        REQUIRE(e.size() == 1);
        REQUIRE(e.data().front().period() == 2);
        REQUIRE(e.data().front().cycle() == CYCLE_A);
        REQUIRE(e.data().front().track() == TRACK_MIC1);
        REQUIRE(e.data().front().type() == SOLO_EVENT_OFF);
        REQUIRE(e.data().front().absTimeMsec() == 100);

        e.clear();
        e.addPeriod(Period(CYCLE_A, EVENT_ON, 5), TRACK_MIC1, 74, 100.5);
        REQUIRE(e.size() == 1);
        REQUIRE(e.data().front().period() == 74);
        REQUIRE(e.data().front().cycle() == CYCLE_A);
        REQUIRE(e.data().front().track() == TRACK_MIC1);
        REQUIRE(e.data().front().type() == SOLO_EVENT_ON);
        REQUIRE(e.data().front().absTimeMsec() == 100.5);

        e.clear();
        e.addPeriod(Period(CYCLE_A, EVENT_ON, 5).setRelOffset(0.5, 0.25), TRACK_MIC1, 74, 1000);
        REQUIRE(e.size() == 3);
        REQUIRE(e.data()[0].period() == 74);
        REQUIRE(e.data()[0].cycle() == CYCLE_A);
        REQUIRE(e.data()[0].track() == TRACK_MIC1);
        REQUIRE(e.data()[0].type() == SOLO_EVENT_OFF);
        REQUIRE(e.data()[0].absTimeMsec() == 1000);
        REQUIRE(e.data()[1].period() == 74);
        REQUIRE(e.data()[1].cycle() == CYCLE_A);
        REQUIRE(e.data()[1].track() == TRACK_MIC1);
        REQUIRE(e.data()[1].type() == SOLO_EVENT_ON);
        REQUIRE(e.data()[1].absTimeMsec() == 3500);
        REQUIRE(e.data()[2].period() == 74);
        REQUIRE(e.data()[2].cycle() == CYCLE_A);
        REQUIRE(e.data()[2].track() == TRACK_MIC1);
        REQUIRE(e.data()[2].type() == SOLO_EVENT_OFF);
        REQUIRE(e.data()[2].absTimeMsec() == 4750);

        e.clear();
        e.addPeriod(Period(CYCLE_A, EVENT_CRESC, 16), TRACK_MIC1, 74, 1000);
        REQUIRE(e.size() == 64);
        REQUIRE(e.data()[0].period() == 74);
        REQUIRE(e.data()[0].absTimeMsec() == 1000);
        REQUIRE(e.data()[0].value() == 0);
        REQUIRE(e.data()[1].absTimeMsec() == 1250);
        REQUIRE(e.data()[1].value() == 1 / 64.0);
        REQUIRE(e.data()[2].absTimeMsec() == 1500);
        REQUIRE(e.data()[2].value() == 2 / 64.0);
        REQUIRE(e.data()[32].absTimeMsec() == 1000 + 250 * 32);
        REQUIRE(e.data()[32].value() == 0.5);

        e.clear();
        e.addPeriod(Period(CYCLE_A, EVENT_CRESC, 16).setRelOffset(0.25, 0.5), TRACK_MIC1, 74, 1000);
        REQUIRE(e.size() == 65);
        REQUIRE(e.data()[0].period() == 74);
        REQUIRE(e.data()[0].absTimeMsec() == 1000);
        REQUIRE(e.data()[0].value() == 0);
        REQUIRE(e.data()[0].type() == SOLO_EVENT_OFF);
        REQUIRE(e.data()[1].absTimeMsec() == 5000);
        REQUIRE(e.data()[1].value() == 0);
        REQUIRE(e.data()[2].absTimeMsec() == 5125);
        REQUIRE(e.data()[2].value() == 1 / 64.0);
        REQUIRE(e.data()[3].absTimeMsec() == 5250);
        REQUIRE(e.data()[3].value() == 2 / 64.0);
        REQUIRE(e.data()[33].absTimeMsec() == 5000 + 125 * 32);
        REQUIRE(e.data()[33].value() == 0.5);

        e.addScheme(Scheme(2));
        REQUIRE(e.size() == 863);
    }

    SECTION("addBurst")
    {
        SoloEventList e;
        e.addBurst(Period(CYCLE_A, EVENT_BURST, 1).setPerf(1), TRACK_FB1, 1000, 3);
        REQUIRE(e.size() == 3);
        REQUIRE(e.eventAt(0).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(1).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(2).type() == SOLO_EVENT_OFF);

        REQUIRE(e.eventAt(0).absTimeMsec() == 1000);
        REQUIRE(e.eventAt(1).absTimeMsec() == 1425);
        REQUIRE(e.eventAt(2).absTimeMsec() == 1575);

        e.clear();
        e.addBurst(Period(CYCLE_A, EVENT_BURST, 3).setPerf(2), TRACK_FB1, 0, 3);
        REQUIRE(e.size() == 5);
        REQUIRE(e.eventAt(0).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(1).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(2).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(3).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(4).type() == SOLO_EVENT_OFF);

        REQUIRE(e.eventAt(0).absTimeMsec() == 0);
        REQUIRE(e.eventAt(1).absTimeMsec() == 900);
        REQUIRE(e.eventAt(2).absTimeMsec() == 1050);
        REQUIRE(e.eventAt(3).absTimeMsec() == 1800);
        REQUIRE(e.eventAt(4).absTimeMsec() == 1950);
    }

    SECTION("periodEvents")
    {
        SoloEventList e;
        e.addScheme(Scheme(2));

        REQUIRE(e.periodEvents(0).size() == 10);
        REQUIRE(e.eventAt(0).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(1).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(2).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(3).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(4).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(5).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(6).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(7).type() == SOLO_EVENT_ON);
        REQUIRE(e.eventAt(8).type() == SOLO_EVENT_OFF);
        REQUIRE(e.eventAt(9).type() == SOLO_EVENT_ON);
    }

    SECTION("cyclePhase")
    {
        Scheme s(2);
        REQUIRE(s.lengthSec() == Approx(746.2));
        REQUIRE(s.cyclePhase(0) == 0);
        REQUIRE(s.cyclePhase(9 * 6) == 0.5);
        REQUIRE(s.cyclePhase(9 * 9) == 0.75);
        REQUIRE(s.cyclePhase(108) == 0);
        REQUIRE(s.cyclePhase(108 + 7 * 6) == 0.25);
        REQUIRE(s.cyclePhase(108 + 7 * 18) == 0.75);
        REQUIRE(s.cyclePhase(108 + 168) == 0);
    }

    SECTION("cycleAt")
    {
        Scheme s(2);
        REQUIRE(s.lengthSec() == Approx(746.2));
        REQUIRE(s.cycleAt(0) == CYCLE_A);
        REQUIRE(s.cycleAt(9 * 6) == CYCLE_A);
        REQUIRE(s.cycleAt(9 * 9) == CYCLE_A);
        REQUIRE(s.cycleAt(108) == CYCLE_B);
        REQUIRE(s.cycleAt(108 + 168) == CYCLE_C);
        REQUIRE(s.cycleAt(-123) == -1);
        REQUIRE(s.cycleAt(234234234) == -1);
    }

    SECTION("periodAt")
    {
        Scheme s(2);
        REQUIRE(s.lengthSec() == Approx(746.2));
        REQUIRE(s.periodAt(0) == 0);
        REQUIRE(s.periodAt(11) == 0);
        REQUIRE(s.periodAt(12) == 1);
        REQUIRE(s.periodAt(24) == 2);
        REQUIRE(s.periodAt(108) == 0);
        REQUIRE(s.periodAt(132) == 1);
    }

    SECTION("periodPhase")
    {
        Scheme s(2);
        REQUIRE(s.lengthSec() == Approx(746.2));
        REQUIRE(s.periodPhase(0) == 0);
        REQUIRE(s.periodPhase(6) == 0.5);
        REQUIRE(s.periodPhase(12) == 0);
        REQUIRE(s.periodPhase(13.5) == 0.125);
        REQUIRE(s.periodPhase(15) == 0.25);
        REQUIRE(s.periodPhase(21) == 0.75);
    }

    SECTION("atTime")
    {
        Scheme s(2);
        REQUIRE(s.lengthSec() == Approx(746.2));
        REQUIRE(s.atTime(0) == PositionInfo { 0, CYCLE_A, 0, 0, 0 });
        REQUIRE(s.atTime(108).cycle == CYCLE_B);
        REQUIRE(s.atTime(108 + 168).cycle == CYCLE_C);
    }
}
