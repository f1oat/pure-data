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
#include "ceammc_data.h"
#include "ceammc_fn_list.h"
#include "ceammc_log.h"
#include "test_common.h"

using namespace ceammc;

template <class T0, class T1>
static std::ostream& operator<<(std::ostream& os, const std::pair<T0, T1>& p)
{
    os << "<" << p.first << ", " << p.second << ">";
    return os;
}

TEST_CASE("list functions2", "[core]")
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

        using ID = DataAtom<IntData>;
        ID i1(1);
        ID i2(2);
        REQUIRE(list::uniqueSorted(LA(i1, i2, i1, i2, i1, i1)) == LA(i1, ID(2)));
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
        using AL = AtomList;
        // invalid
        REQUIRE(list::stretch(AL {}, 2) == L());
        REQUIRE(list::stretch(AL { 1, 2, 3, 4 }, 0) == L());
        REQUIRE(list::stretch(AL { 1, 2, 3, 4 }, 1) == L());

        //  integer
        REQUIRE(list::stretch(AL { 1 }, 4) == LX({ 1, 1, 1, 1 }));
        REQUIRE(list::stretch(AL { 1, 2 }, 3) == LX({ 1, 1.5, 2 }));
        REQUIRE(list::stretch(AL { 1, 2, 3, 4 }, 4) == AtomList({ 1, 2, 3, 4 }));
        REQUIRE(list::stretch(AL { 1, 2, 3, 4 }, 2) == LX({ 1, 4 }));
        REQUIRE(list::stretch(AL { 1, 2, 3, 4 }, 7) == LX({ 1, 1.5, 2, 2.5, 3, 3.5, 4 }));
        REQUIRE(list::stretch(AL { 1, 2, 3, 4 }, 3) == LX({ 1, 2.5, 4 }));
    }

    SECTION("rleEncode")
    {
        using AL = AtomList;
        using RLE = std::vector<std::pair<Atom, size_t>>;

        REQUIRE(list::rleEncode(AL {}) == RLE());
        REQUIRE(list::rleEncode(AL { 1 }) == RLE({ { Atom(1), 1 } }));
        REQUIRE(list::rleEncode(AL { 1 }) == RLE({ { Atom(1), 1 } }));
        REQUIRE(list::rleEncode(AL { 1, 1 }) == RLE({ { Atom(1), 2 } }));
        REQUIRE(list::rleEncode(AL { 1, 1, 1 }) == RLE({ { Atom(1), 3 } }));
        REQUIRE(list::rleEncode(AL { 1, 2 }) == RLE({ { Atom(1), 1 }, { Atom(2), 1 } }));
        REQUIRE(list::rleEncode(AL { 1, 1, 2 }) == RLE({ { Atom(1), 2 }, { Atom(2), 1 } }));
        REQUIRE(list::rleEncode(AL { 1, 2, 2 }) == RLE({ { Atom(1), 1 }, { Atom(2), 2 } }));
        REQUIRE(list::rleEncode(AL { 1, 1, 2, 2 }) == RLE({ { Atom(1), 2 }, { Atom(2), 2 } }));
        REQUIRE(list::rleEncode(AL { 1, 1, 2, 1 }) == RLE({ { Atom(1), 2 }, { Atom(2), 1 }, { Atom(1), 1 } }));
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

    SECTION("exctactByType")
    {
        auto src = LA("A", "C", "true", 0.0, 1, 1.5, 2);

        std::vector<int> int_res_expected { 0, 1, 2 };
        REQUIRE(list::extractByType<int>(src) == int_res_expected);

        std::vector<t_float> float_res_expected { 0, 1, 1.5, 2 };
        REQUIRE(list::extractByType<t_float>(src) == float_res_expected);

        std::vector<bool> bool_res_expected { true, false, true };
        REQUIRE(list::extractByType<bool>(src) == bool_res_expected);

        std::vector<t_symbol*> symbol_res_expected { SYM("A"), SYM("C"), SYM("true") };
        REQUIRE(list::extractByType<t_symbol*>(src) == symbol_res_expected);
    }

    SECTION("resample")
    {
        AtomList res;
        REQUIRE(list::resample(L(), res, 1));
        REQUIRE(res.empty());

        REQUIRE(list::resample(LF(1, 2), res, 1));
        REQUIRE(res == LF(1, 2));

        REQUIRE_FALSE(list::resample(LF(1, 2, 3), res, 0));
        REQUIRE(res == LF(1, 2));

        res.clear();
        REQUIRE(list::resample(LF(0, 1, 0, -1, 0, 1, 0, -1), res, 2));

#define REQUIRE_FLOAT(a, b) REQUIRE(a.asFloat() == Approx(b).margin(0.00001))

        REQUIRE_FLOAT(res[0], 0.0951107);
        REQUIRE_FLOAT(res[1], 0.5723056);
        REQUIRE_FLOAT(res[2], 0.9112480);
        REQUIRE_FLOAT(res[3], 0.7417413);
        REQUIRE_FLOAT(res[4], 0.0687988);
        REQUIRE_FLOAT(res[5], -0.6919080);
        REQUIRE_FLOAT(res[6], -1.0375848);
        REQUIRE_FLOAT(res[7], -0.7455387);
        REQUIRE_FLOAT(res[8], 0);
        REQUIRE_FLOAT(res[9], 0.7455388);
        REQUIRE_FLOAT(res[10], 1.0375848);
        REQUIRE_FLOAT(res[11], 0.6919080);
        REQUIRE_FLOAT(res[12], -0.0687987);
        REQUIRE_FLOAT(res[13], -0.7417413);
        REQUIRE_FLOAT(res[14], -0.9112480);
        REQUIRE_FLOAT(res[15], -0.5723056);
    }

    SECTION("view find props")
    {
        Atom pa = A("@a");
        AtomListView pav(pa);
        Atom pb = A("@b");
        AtomListView pbv(pb);
        AtomList pav1 = LA("@a", 1, 2, 3);
        AtomList pav2 = LA("NON", "@a", 1, 2, 3);
        AtomList pav3 = LA(1, 2, 3, "@a", 1, 2, 3);
        AtomList pav4 = LA(1, 2, 3, "@a", 1, 2, 3, "@b");
        AtomList pav5 = LA(-1, "@a", 1, 2, 3, "@b", 10, 20, 30);
        AtomList pav6 = LA(-1, "@a", 1, 2, 3, "@b", 10, 20, 30, "@c", 100);
        AtomList pbv1 = LA("@b", 10, 20, 30);
        AtomList pbv2 = LA("@b", 10, 20, 30, "@c", 100);
        AtomList pcv1 = LA("@c", 100);

        REQUIRE(list::findProperty(L()) == list::ViewSlice {});
        REQUIRE(list::findProperty(LF(1)) == list::ViewSlice {});
        REQUIRE(list::findProperty(LF(1, 2, 3)) == list::ViewSlice {});
        REQUIRE(list::findProperty(LA("ABC")) == list::ViewSlice {});
        REQUIRE(list::findProperty(pav) == list::ViewSlice { pav, {} });
        REQUIRE(list::findProperty(pav1) == list::ViewSlice { pav1, {} });
        REQUIRE(list::findProperty(pav2) == list::ViewSlice { pav1, {} });
        REQUIRE(list::findProperty(pav3) == list::ViewSlice { pav1, {} });
        REQUIRE(list::findProperty(pav4) == list::ViewSlice { pav1, pb });
        REQUIRE(list::findProperty(pav5) == list::ViewSlice { pav1, pbv1 });
        REQUIRE(list::findProperty(pav6) == list::ViewSlice { pav1, pbv2 });

        auto res = list::findProperty(pav6);
        while (!res.first.empty()) {
            REQUIRE((res.first == pav1 || res.first == pbv1 || res.first == pcv1));
            res = list::findProperty(res.second);
        }
    }

    SECTION("hexbeat")
    {
        REQUIRE(list::hexbeat_bin("") == L());
        REQUIRE(list::hexbeat_bin("?") == L());
        REQUIRE(list::hexbeat_bin("#") == L());
        REQUIRE(list::hexbeat_bin("#?") == L());
        REQUIRE(list::hexbeat_bin("0x0") == LF(0, 0, 0, 0));
        REQUIRE(list::hexbeat_bin("#0") == LF(0, 0, 0, 0));
        REQUIRE(list::hexbeat_bin("#1") == LF(0, 0, 0, 1));
        REQUIRE(list::hexbeat_bin("#2") == LF(0, 0, 1, 0));
        REQUIRE(list::hexbeat_bin("#3") == LF(0, 0, 1, 1));
        REQUIRE(list::hexbeat_bin("#4") == LF(0, 1, 0, 0));
        REQUIRE(list::hexbeat_bin("#5") == LF(0, 1, 0, 1));
        REQUIRE(list::hexbeat_bin("#6") == LF(0, 1, 1, 0));
        REQUIRE(list::hexbeat_bin("#7") == LF(0, 1, 1, 1));
        REQUIRE(list::hexbeat_bin("#8") == LF(1, 0, 0, 0));
        REQUIRE(list::hexbeat_bin("#9") == LF(1, 0, 0, 1));
        REQUIRE(list::hexbeat_bin("#A") == LF(1, 0, 1, 0));
        REQUIRE(list::hexbeat_bin("#B") == LF(1, 0, 1, 1));
        REQUIRE(list::hexbeat_bin("#C") == LF(1, 1, 0, 0));
        REQUIRE(list::hexbeat_bin("#D") == LF(1, 1, 0, 1));
        REQUIRE(list::hexbeat_bin("#E") == LF(1, 1, 1, 0));
        REQUIRE(list::hexbeat_bin("#F") == LF(1, 1, 1, 1));
        REQUIRE(list::hexbeat_bin("#a") == LF(1, 0, 1, 0));
        REQUIRE(list::hexbeat_bin("#b") == LF(1, 0, 1, 1));
        REQUIRE(list::hexbeat_bin("#c") == LF(1, 1, 0, 0));
        REQUIRE(list::hexbeat_bin("#d") == LF(1, 1, 0, 1));
        REQUIRE(list::hexbeat_bin("#e") == LF(1, 1, 1, 0));
        REQUIRE(list::hexbeat_bin("#f") == LF(1, 1, 1, 1));
        REQUIRE(list::hexbeat_bin("#BEEF") == LF(1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1));
        REQUIRE(list::hexbeat_bin("0xBEEF") == LF(1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1));
        REQUIRE(list::hexbeat_bin("#B?E?E?F???") == LF(1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1));
        REQUIRE(list::hexbeat_bin("#кириллица") == L());
    }

    SECTION("hexbeat_dur")
    {
        AtomList values;
        int shift = 0;
        REQUIRE(!list::hexbeat_dur("", values, shift));
        REQUIRE(list::hexbeat_dur("0xF", values, shift));
        REQUIRE(shift == 0);
        REQUIRE(values == LF(1, 1, 1, 1));
        // 0 1 1 1 -> 1 1 2 with 1 upbeat
        REQUIRE(list::hexbeat_dur("0x7", values, shift));
        REQUIRE(shift == 1);
        REQUIRE(values == LF(1, 1, 2));
        // 0 0 1 1 -> 1 3 with 2 upbeat
        REQUIRE(list::hexbeat_dur("0x3", values, shift));
        REQUIRE(shift == 2);
        REQUIRE(values == LF(1, 3));
        // 0 0 0 1 -> 4 with 3 upbeat
        REQUIRE(list::hexbeat_dur("0x1", values, shift));
        REQUIRE(shift == 3);
        REQUIRE(values == LF(4));
        // 0 0 1 0 -> 4 with 2 upbeat
        REQUIRE(list::hexbeat_dur("0x2", values, shift));
        REQUIRE(shift == 2);
        REQUIRE(values == LF(4));
        // 0 1 1 0 -> 1 3 with 1 upbeat
        REQUIRE(list::hexbeat_dur("0x6", values, shift));
        REQUIRE(shift == 1);
        REQUIRE(values == LF(1, 3));
        // 0 1 0 1 -> 2 2 with 1 upbeat
        REQUIRE(list::hexbeat_dur("0x5", values, shift));
        REQUIRE(shift == 1);
        REQUIRE(values == LF(2, 2));
    }

    SECTION("interpolate lin")
    {
        SECTION("default")
        {
            REQUIRE(list::interpolate_lin(L(), L(), 0) == L());
            REQUIRE(list::interpolate_lin(L(), L(), 1) == L());
            REQUIRE(list::interpolate_lin(LF(1), L(), 0) == L());
            REQUIRE(list::interpolate_lin(L(), LF(1), 0) == L());
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 1) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 2) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), -1) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.5) == LF(1.5, 2.5, 3.5));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.25) == LF(1.25, 2.25, 3.25));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.25) == LF(1.25, 2.25));
        }

        SECTION("minsize")
        {
            REQUIRE(list::interpolate_lin(L(), L(), 0, AtomList::MINSIZE) == L());
            REQUIRE(list::interpolate_lin(L(), L(), 1, AtomList::MINSIZE) == L());
            REQUIRE(list::interpolate_lin(LF(1), L(), 0, AtomList::MINSIZE) == L());
            REQUIRE(list::interpolate_lin(L(), LF(1), 0, AtomList::MINSIZE) == L());
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0, AtomList::MINSIZE) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 1, AtomList::MINSIZE) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 2, AtomList::MINSIZE) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), -1, AtomList::MINSIZE) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.5, AtomList::MINSIZE) == LF(1.5, 2.5, 3.5));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.25, AtomList::MINSIZE) == LF(1.25, 2.25, 3.25));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.25, AtomList::MINSIZE) == LF(1.25, 2.25));
        }

        SECTION("padzero")
        {
            REQUIRE(list::interpolate_lin(L(), L(), 0, AtomList::PADZERO) == L());
            REQUIRE(list::interpolate_lin(L(), L(), 1, AtomList::PADZERO) == L());
            REQUIRE(list::interpolate_lin(LF(1), L(), 0, AtomList::PADZERO) == LF(1));
            REQUIRE(list::interpolate_lin(L(), LF(1), 0, AtomList::PADZERO) == LF(0));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0, AtomList::PADZERO) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 1, AtomList::PADZERO) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 2, AtomList::PADZERO) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), -1, AtomList::PADZERO) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.5, AtomList::PADZERO) == LF(1.5, 2.5, 3.5));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.25, AtomList::PADZERO) == LF(1.25, 2.25, 3.25));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.25, AtomList::PADZERO) == LF(1.25, 2.25, 2.25));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.5, AtomList::PADZERO) == LF(1.5, 2.5, 1.5));
            REQUIRE(list::interpolate_lin(LF(2, 3), LF(1, 2, 3), 0.5, AtomList::PADZERO) == LF(1.5, 2.5, 1.5));
        }

        SECTION("clip")
        {
            REQUIRE(list::interpolate_lin(L(), L(), 0, AtomList::CLIP) == L());
            REQUIRE(list::interpolate_lin(L(), L(), 1, AtomList::CLIP) == L());
            REQUIRE(list::interpolate_lin(LF(1), L(), 0, AtomList::CLIP) == L());
            REQUIRE(list::interpolate_lin(L(), LF(1), 0, AtomList::CLIP) == L());
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0, AtomList::CLIP) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 1, AtomList::CLIP) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 2, AtomList::CLIP) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), -1, AtomList::CLIP) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.5, AtomList::CLIP) == LF(1.5, 2.5, 3.5));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.25, AtomList::CLIP) == LF(1.25, 2.25, 3.25));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.25, AtomList::CLIP) == LF(1.25, 2.25, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.5, AtomList::CLIP) == LF(1.5, 2.5, 3));
            REQUIRE(list::interpolate_lin(LF(2, 3), LF(1, 2, 3), 0.5, AtomList::CLIP) == LF(1.5, 2.5, 3));
        }

        SECTION("wrap")
        {
            REQUIRE(list::interpolate_lin(L(), L(), 0, AtomList::WRAP) == L());
            REQUIRE(list::interpolate_lin(L(), L(), 1, AtomList::WRAP) == L());
            REQUIRE(list::interpolate_lin(LF(1), L(), 0, AtomList::WRAP) == L());
            REQUIRE(list::interpolate_lin(L(), LF(1), 0, AtomList::WRAP) == L());
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0, AtomList::WRAP) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 1, AtomList::WRAP) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 2, AtomList::WRAP) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), -1, AtomList::WRAP) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.5, AtomList::WRAP) == LF(1.5, 2.5, 3.5));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.25, AtomList::WRAP) == LF(1.25, 2.25, 3.25));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.25, AtomList::WRAP) == LF(1.25, 2.25, 2.75));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.5, AtomList::WRAP) == LF(1.5, 2.5, 2.5));
            REQUIRE(list::interpolate_lin(LF(2, 3), LF(1, 2, 3), 0.5, AtomList::WRAP) == LF(1.5, 2.5, 2.5));
        }

        SECTION("fold")
        {
            REQUIRE(list::interpolate_lin(L(), L(), 0, AtomList::FOLD) == L());
            REQUIRE(list::interpolate_lin(L(), L(), 1, AtomList::FOLD) == L());
            REQUIRE(list::interpolate_lin(LF(1), L(), 0, AtomList::FOLD) == L());
            REQUIRE(list::interpolate_lin(L(), LF(1), 0, AtomList::FOLD) == L());
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0, AtomList::FOLD) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 1, AtomList::FOLD) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 2, AtomList::FOLD) == LF(2, 3, 4));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), -1, AtomList::FOLD) == LF(1, 2, 3));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.5, AtomList::FOLD) == LF(1.5, 2.5, 3.5));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3, 4), 0.25, AtomList::FOLD) == LF(1.25, 2.25, 3.25));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.25, AtomList::FOLD) == LF(1.25, 2.25, 2.75));
            REQUIRE(list::interpolate_lin(LF(1, 2, 3), LF(2, 3), 0.5, AtomList::FOLD) == LF(1.5, 2.5, 2.5));
            REQUIRE(list::interpolate_lin(LF(2, 3), LF(1, 2, 3), 0.5, AtomList::FOLD) == LF(1.5, 2.5, 2.5));
        }
    }

    SECTION("foreachProperties")
    {
        std::vector<AtomList> res;
        auto fn = [&res](const AtomListView& lv) { res.push_back(lv); };
        REQUIRE(list::foreachProperty(L(), fn) == 0);
        REQUIRE(list::foreachProperty(LF(1, 2, 3, 4), fn) == 0);

        REQUIRE(list::foreachProperty(LA("@a"), fn) == 1);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == LA("@a"));
        res.clear();

        REQUIRE(list::foreachProperty(LA(1, 2, 3, "@a"), fn) == 1);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == LA("@a"));
        res.clear();

        REQUIRE(list::foreachProperty(LA(1, 2, 3, "@a", 1), fn) == 1);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == LA("@a", 1));
        res.clear();

        REQUIRE(list::foreachProperty(LA("@a", 2), fn) == 1);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == LA("@a", 2));
        res.clear();

        REQUIRE(list::foreachProperty(LA("@a", 2, 3), fn) == 1);
        REQUIRE(res.size() == 1);
        REQUIRE(res[0] == LA("@a", 2, 3));
        res.clear();

        REQUIRE(list::foreachProperty(LA("@a", "@b", "@c"), fn) == 3);
        REQUIRE(res.size() == 3);
        REQUIRE(res[0] == LA("@a"));
        REQUIRE(res[1] == LA("@b"));
        REQUIRE(res[2] == LA("@c"));
        res.clear();

        REQUIRE(list::foreachProperty(LA(1, 2, "@a", "@b", "@c"), fn) == 3);
        REQUIRE(res.size() == 3);
        REQUIRE(res[0] == LA("@a"));
        REQUIRE(res[1] == LA("@b"));
        REQUIRE(res[2] == LA("@c"));
        res.clear();

        REQUIRE(list::foreachProperty(LA(1, 2, "@a", "@b", "@c", -1, -2), fn) == 3);
        REQUIRE(res.size() == 3);
        REQUIRE(res[0] == LA("@a"));
        REQUIRE(res[1] == LA("@b"));
        REQUIRE(res[2] == LA("@c", -1, -2));
        res.clear();

        REQUIRE(list::foreachProperty(LA(1, 2, "@a", "D", "@b", 1, 2, 3, 4, "@c", -1, -2), fn) == 3);
        REQUIRE(res.size() == 3);
        REQUIRE(res[0] == LA("@a", "D"));
        REQUIRE(res[1] == LA("@b", 1, 2, 3, 4));
        REQUIRE(res[2] == LA("@c", -1, -2));
        res.clear();

        using sv = std::vector<std::string>;
        sv keys;
        REQUIRE(list::foreachProperty(LA(1, 2, "@a", "D", "@b", 1, 2, 3, 4, "@c", -1, -2),
                    [&keys, &res](t_symbol* k, const AtomListView& v) {
                        keys.push_back(k->s_name);
                        res.push_back(v);
                    })
            == 3);
        REQUIRE(keys.size() == 3);
        REQUIRE(res.size() == 3);
        REQUIRE(keys == sv { "@a", "@b", "@c" });
        REQUIRE(res[0] == LA("D"));
        REQUIRE(res[1] == LF(1, 2, 3, 4));
        REQUIRE(res[2] == LF(-1, -2));
        res.clear();
    }
}
