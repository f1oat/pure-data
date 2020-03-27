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
#include "ceammc_fn_list.h"
#include "test_common.h"

using namespace ceammc;

TEST_CASE("list functions", "[ceammc::list]")
{
    SECTION("histogram")
    {
        REQUIRE(list::histogram(L(), 0) == L());
        REQUIRE(list::histogram(L(), 10) == L());
        REQUIRE(list::histogram(LF(1), 0) == L());
        REQUIRE(list::histogram(LF(1, 2), 0) == L());

        REQUIRE(list::histogram(LF(1), 1) == LF(1));
        REQUIRE(list::histogram(LF(1, 2), 1) == LF(1));

        REQUIRE(list::histogram(LF(1, 2, 3, 4), 4) == LF(0.25, 0.25, 0.25, 0.25));
        REQUIRE(list::histogram(LF(2, 2, 2, 2), 2) == LF(1, 0.f));
        REQUIRE(list::histogram(LF(1, 2, 2, 2), 2) == LF(0.25, 0.75));
        REQUIRE(list::histogram(LF(1, 1, 2, 2), 2) == LF(0.5, 0.5));
        REQUIRE(list::histogram(LF(1, 1, 1, 2), 2) == LF(0.75, 0.25));
        REQUIRE(list::histogram(LF(1, 1, 1, 1), 2) == LF(1, 0.0));

        REQUIRE(list::histogram(LF(1, 1.1f, 2, 2), 2) == LF(0.5, 0.5));
        REQUIRE(list::histogram(LF(1, 1.1f, 1.9f, 2), 3) == LF(0.5, 0.25, 0.25));
    }

    SECTION("normalizeByRange")
    {

#define REQUIRE_NORMALIZE(l1, dest)               \
    {                                             \
        AtomList res;                             \
        REQUIRE(list::normalizeByRange(l1, res)); \
        REQUIRE(res == dest);                     \
    }

        AtomList res;

        REQUIRE_FALSE(list::normalizeByRange(L(), res));
        REQUIRE_FALSE(list::normalizeByRange(LF(0), res));
        REQUIRE_FALSE(list::normalizeByRange(LF(10), res));
        REQUIRE_FALSE(list::normalizeByRange(LF(-10), res));
        REQUIRE_FALSE(list::normalizeByRange(LF(1, 1, 1, 1), res));
        REQUIRE(res.empty());

        REQUIRE_NORMALIZE(LF(0, 1), LF(0, 1));
        REQUIRE_NORMALIZE(LF(0, 10), LF(0, 1));
        REQUIRE_NORMALIZE(LF(1, 2, 3, 4, 5), LF(0, 0.25, 0.5, 0.75, 1));
        REQUIRE_NORMALIZE(LF(-1, -2, -3, -4, -5), LF(1, 0.75, 0.5, 0.25, 0));
        REQUIRE_NORMALIZE(LF(-20, 0, 20), LF(0, 0.5, 1.0));
    }

    SECTION("enumerate")
    {
        REQUIRE(list::enumerate(L()) == L());
        REQUIRE(list::enumerate(LF(100)) == LF(0.0, 100));
        REQUIRE(list::enumerate(LF(100), 20) == LF(20, 100));
        REQUIRE(list::enumerate(LF(100), -20) == LF(-20, 100));

        REQUIRE(list::enumerate(LF(100), 20, list::PREPEND) == LF(20, 100));
        REQUIRE(list::enumerate(LF(100), -20, list::PREPEND) == LF(-20, 100));

        REQUIRE(list::enumerate(LF(100), 20, list::APPEND) == LF(100, 20));
        REQUIRE(list::enumerate(LF(100), -20, list::APPEND) == LF(100, -20));
    }

    SECTION("unique")
    {
        REQUIRE(list::uniqueSorted(L()) == L());
        REQUIRE(list::uniqueSorted(LF(100)) == LF(100));
        REQUIRE(list::uniqueSorted(LF(1, 2)) == LF(1, 2));
        REQUIRE(list::uniqueSorted(LF(1, 2, 3, 1, 3, 2)) == LF(1, 2, 3));
        REQUIRE(list::uniqueSorted(LF(1, 1, 1, 1)) == LF(1));

        REQUIRE(list::uniqueStable(L()) == L());
        REQUIRE(list::uniqueStable(LF(100)) == LF(100));
        REQUIRE(list::uniqueStable(LF(2, 1)) == LF(2, 1));
        REQUIRE(list::uniqueStable(LF(1, 3, 3, 1, 3, 2)) == LF(1, 3, 2));
        REQUIRE(list::uniqueStable(LF(1, 1, 1, 1)) == LF(1));
    }

    SECTION("shift")
    {
        REQUIRE(list::shift(L(), 0) == L());
        REQUIRE(list::shift(L(), 0.5) == L());
        REQUIRE(list::shift(L(), 1) == L());
        REQUIRE(list::shift(L(), -1) == L());

        REQUIRE(list::shift(LF(20), 0) == LF(20));
        REQUIRE(list::shift(LF(20), 0.5) == LF(20));
        REQUIRE(list::shift(LF(20), 1) == LF(20));
        REQUIRE(list::shift(LF(20), 1.5) == LF(20));
        REQUIRE(list::shift(LF(20), 2) == LF(20));
        REQUIRE(list::shift(LF(20), -0.5) == LF(20));

        // periodic by N
        REQUIRE(list::shift(LF(0.f, 10), 0) == LX(0, 10));
        REQUIRE(list::shift(LF(0.f, 10), 2) == LX(0, 10));
        REQUIRE(list::shift(LF(0.f, 10), 4) == LX(0, 10));
        REQUIRE(list::shift(LF(0.f, 10), -2) == LX(0, 10));
        REQUIRE(list::shift(LF(0.f, 10), -4) == LX(0, 10));

        //
        REQUIRE(list::shift(LF(0.f, 10), 1) == LX(10, 0));
        REQUIRE(list::shift(LF(0.f, 10), 3) == LX(10, 0));
        REQUIRE(list::shift(LF(0.f, 10), -1) == LX(10, 0));
        REQUIRE(list::shift(LF(0.f, 10), -3) == LX(10, 0));

        // fractional shift
        REQUIRE(list::shift(LF(0.f, 10), 0) == LX(0, 10));
        REQUIRE(list::shift(LF(0.f, 10), 0.3) == LX(3, 10));
        REQUIRE(list::shift(LF(0.f, 10), 0.5) == LX(5, 10));
        REQUIRE(list::shift(LF(0.f, 10), 0.7) == LX(7, 10));
        REQUIRE(list::shift(LF(0.f, 10), 1) == LX(10, 0));
        REQUIRE(list::shift(LF(0.f, 10), 1.3) == LX(10, 3));
        REQUIRE(list::shift(LF(0.f, 10), 1.5) == LX(10, 5));
        REQUIRE(list::shift(LF(0.f, 10), 1.7) == LX(10, 7));
        REQUIRE(list::shift(LF(0.f, 10), 2) == LX(0, 10));

        // fractional negative shift
        REQUIRE(list::shift(LF(0.f, 10), 0) == LX(0, 10));
        REQUIRE(list::shift(LF(0.f, 10), -0.3) == LX(10, 7));
        REQUIRE(list::shift(LF(0.f, 10), -0.5) == LX(10, 5));
        REQUIRE(list::shift(LF(0.f, 10), -0.7) == LX(10, 3));
        REQUIRE(list::shift(LF(0.f, 10), -1) == LX(10, 0));
        REQUIRE(list::shift(LF(0.f, 10), -1.3) == LX(7, 10));
        REQUIRE(list::shift(LF(0.f, 10), -1.5) == LX(5, 10));
        REQUIRE(list::shift(LF(0.f, 10), -1.7) == LX(3, 10));
        REQUIRE(list::shift(LF(0.f, 10), -2) == LX(0, 10));
    }

    SECTION("shift 3")
    {
        // integer
        REQUIRE(list::shift(LF(1, 2, 3), 0) == LX(1, 2, 3));
        REQUIRE(list::shift(LF(1, 2, 3), 1) == LX(2, 3, 1));
        REQUIRE(list::shift(LF(1, 2, 3), 2) == LX(3, 1, 2));
        REQUIRE(list::shift(LF(1, 2, 3), 3) == LX(1, 2, 3));
        REQUIRE(list::shift(LF(1, 2, 3), 4) == LX(2, 3, 1));
        REQUIRE(list::shift(LF(1, 2, 3), 5) == LX(3, 1, 2));
        REQUIRE(list::shift(LF(1, 2, 3), 6) == LX(1, 2, 3));

        // negative
        REQUIRE(list::shift(LF(1, 2, 3), -1) == LX(3, 1, 2));
        REQUIRE(list::shift(LF(1, 2, 3), -2) == LX(2, 3, 1));
        REQUIRE(list::shift(LF(1, 2, 3), -3) == LX(1, 2, 3));
        REQUIRE(list::shift(LF(1, 2, 3), -4) == LX(3, 1, 2));
        REQUIRE(list::shift(LF(1, 2, 3), -5) == LX(2, 3, 1));
        REQUIRE(list::shift(LF(1, 2, 3), -6) == LX(1, 2, 3));

        // frac position
        REQUIRE(list::shift(LF(1, 2, 3), 0.1) == LX(1.1, 2.1, 3));
        REQUIRE(list::shift(LF(1, 2, 3), 0.5) == LX(1.5, 2.5, 3));
        REQUIRE(list::shift(LF(1, 2, 3), 0.9) == LX(1.9, 2.9, 3));
        REQUIRE(list::shift(LF(1, 2, 3), 1) == LX(2, 3, 1));
        REQUIRE(list::shift(LF(1, 2, 3), 1.1) == LX(2.1, 3, 1.1));
        REQUIRE(list::shift(LF(1, 2, 3), 1.5) == LX(2.5, 3, 1.5));
        REQUIRE(list::shift(LF(1, 2, 3), 2) == LX(3, 1, 2));
        REQUIRE(list::shift(LF(1, 2, 3), 2.5) == LX(3, 1.5, 2.5));
        REQUIRE(list::shift(LF(1, 2, 3), 3.5) == LX(1.5, 2.5, 3));

        REQUIRE(list::shift(LF(1, 2, 3), -0.5) == LX(3, 1.5, 2.5));
        REQUIRE(list::shift(LF(1, 2, 3), -0.1) == LX(3, 1.9, 2.9));
    }

    SECTION("shift")
    {
        // invalid
        REQUIRE(list::stretch({}, 2) == L());
        REQUIRE(list::stretch({ 1, 2, 3, 4 }, 0) == L());
        REQUIRE(list::stretch({ 1, 2, 3, 4 }, 1) == L());

        //  integer
        REQUIRE(list::stretch({ 1 }, 4) == LX({ 1, 1, 1, 1 }));
        REQUIRE(list::stretch({ 1, 2 }, 3) == LX({ 1, 1.5, 2 }));
        REQUIRE(list::stretch({ 1, 2, 3, 4 }, 4) == AtomList({ 1, 2, 3, 4 }));
        REQUIRE(list::stretch({ 1, 2, 3, 4 }, 2) == LX({ 1, 4 }));
        REQUIRE(list::stretch({ 1, 2, 3, 4 }, 7) == LX({ 1, 1.5, 2, 2.5, 3, 3.5, 4 }));
        REQUIRE(list::stretch({ 1, 2, 3, 4 }, 3) == LX({ 1, 2.5, 4 }));
    }

    SECTION("rleEncode")
    {
        typedef std::vector<std::pair<Atom, size_t>> RLE;

        REQUIRE(list::rleEncode({}) == RLE());
        REQUIRE(list::rleEncode({ 1 }) == RLE({ { Atom(1), 1 } }));
        REQUIRE(list::rleEncode({ 1 }) == RLE({ { Atom(1), 1 } }));
        REQUIRE(list::rleEncode({ 1, 1 }) == RLE({ { Atom(1), 2 } }));
        REQUIRE(list::rleEncode({ 1, 1, 1 }) == RLE({ { Atom(1), 3 } }));
        REQUIRE(list::rleEncode({ 1, 2 }) == RLE({ { Atom(1), 1 }, { Atom(2), 1 } }));
        REQUIRE(list::rleEncode({ 1, 1, 2 }) == RLE({ { Atom(1), 2 }, { Atom(2), 1 } }));
        REQUIRE(list::rleEncode({ 1, 2, 2 }) == RLE({ { Atom(1), 1 }, { Atom(2), 2 } }));
        REQUIRE(list::rleEncode({ 1, 1, 2, 2 }) == RLE({ { Atom(1), 2 }, { Atom(2), 2 } }));
        REQUIRE(list::rleEncode({ 1, 1, 2, 1 }) == RLE({ { Atom(1), 2 }, { Atom(2), 1 }, { Atom(1), 1 } }));
    }

    SECTION("rleDecode")
    {
        REQUIRE(list::rleDecode({}) == L());
        REQUIRE(list::rleDecode({ { 10, 0 } }) == L());
        REQUIRE(list::rleDecode({ { 10, 1 } }) == LF(10));
        REQUIRE(list::rleDecode({ { 10, 2 } }) == LF(10, 10));

        REQUIRE(list::rleDecode({ { 10, 2 }, { 11, 1 } }) == LF(10, 10, 11));
        REQUIRE(list::rleDecode({ { 10, 2 }, { 11, 1 }, { 10, 2 } }) == LF(10, 10, 11, 10, 10));

#define REQUIRE_RLE(l)                                               \
    {                                                                \
        REQUIRE(list::rleDecode(list::rleEncode(l)) == AtomList(l)); \
    }

        REQUIRE_RLE(L());
        REQUIRE_RLE(LF(1));
        REQUIRE_RLE(LF(1, 1, 1, 1));
        REQUIRE_RLE(LF(1, 2, 3, 3, 2, 2, 1, 1, 1));
        REQUIRE_RLE(LA("a", "b", "a", "a"));
    }

    SECTION("canConvertToType")
    {
        REQUIRE_FALSE(list::canConvertToType<bool>(L()));
        REQUIRE_FALSE(list::canConvertToType<bool>(LF(1, 2, 3)));
        REQUIRE_FALSE(list::canConvertToType<bool>(LA("test")));
        REQUIRE(list::canConvertToType<bool>(LF(1)));
        REQUIRE(list::canConvertToType<bool>(LF(0)));
        REQUIRE(list::canConvertToType<bool>(LF(-0.1)));
        REQUIRE(list::canConvertToType<bool>(LA("true")));
        REQUIRE(list::canConvertToType<bool>(LA("false")));

        REQUIRE_FALSE(list::canConvertToType<float>(L()));
        REQUIRE_FALSE(list::canConvertToType<float>(LF(1, 2)));
        REQUIRE_FALSE(list::canConvertToType<float>(LA("abc")));
        REQUIRE(list::canConvertToType<float>(LF(1.123)));

        REQUIRE_FALSE(list::canConvertToType<double>(L()));
        REQUIRE_FALSE(list::canConvertToType<double>(LF(1, 2)));
        REQUIRE_FALSE(list::canConvertToType<double>(LA("abc")));
        REQUIRE(list::canConvertToType<double>(LF(1.123)));

        REQUIRE_FALSE(list::canConvertToType<int>(L()));
        REQUIRE_FALSE(list::canConvertToType<int>(LF(1, 2)));
        REQUIRE_FALSE(list::canConvertToType<int>(LA("abc")));
        REQUIRE(list::canConvertToType<int>(LF(-100)));
        REQUIRE(list::canConvertToType<int>(LF(1.123)));

        REQUIRE_FALSE(list::canConvertToType<size_t>(L()));
        REQUIRE_FALSE(list::canConvertToType<size_t>(LF(1, 2)));
        REQUIRE_FALSE(list::canConvertToType<size_t>(LA("abc")));
        REQUIRE_FALSE(list::canConvertToType<size_t>(LF(-100)));
        REQUIRE(list::canConvertToType<size_t>(LF(100)));
        REQUIRE(list::canConvertToType<size_t>(LF(1.123)));

        REQUIRE_FALSE(list::canConvertToType<t_symbol*>(L()));
        REQUIRE_FALSE(list::canConvertToType<t_symbol*>(LA(1)));
        REQUIRE_FALSE(list::canConvertToType<t_symbol*>(LA(1, 2)));
        REQUIRE_FALSE(list::canConvertToType<t_symbol*>(LA("abc", 1)));
        REQUIRE_FALSE(list::canConvertToType<t_symbol*>(LA("abc", "def")));
        REQUIRE(list::canConvertToType<t_symbol*>(LA("A")));

        REQUIRE_FALSE(list::canConvertToType<Atom>(L()));
        REQUIRE_FALSE(list::canConvertToType<Atom>(LA(1, 2)));
        REQUIRE(list::canConvertToType<Atom>(LA(1)));
        REQUIRE(list::canConvertToType<Atom>(LA("test")));

        REQUIRE(list::canConvertToType<AtomList>(L()));
        REQUIRE(list::canConvertToType<AtomList>(LA(1)));
        REQUIRE(list::canConvertToType<AtomList>(LA(1, 2)));
        REQUIRE(list::canConvertToType<AtomList>(LA(1, 2, "ABC")));
    }
}
