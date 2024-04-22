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
#include "catch.hpp"
#include "test_base.h"
#include "test_macro.h"

#include "ceammc_atomlist_view.h"
using namespace ceammc;

#include <algorithm>
#include <boost/optional/optional_io.hpp>

namespace {

Atom atomAdd(const Atom& a, const Atom& b) { return Atom(a.asFloat() + b.asFloat()); }
float atomSum(const Atom& a, const Atom& b) { return a.asFloat(0) + b.asFloat(0); }
t_float floatSum(t_float a, t_float b) { return a + b; }
float atomMul(const Atom& a, const Atom& b) { return a.asFloat(1) * b.asFloat(1); }
t_float floatMul(t_float a, t_float b) { return a * b; }
t_float mul10(t_float v) { return v * 10; }
t_float neg(t_float v) { return -v; }

t_symbol* toUpper(t_symbol* s)
{
    std::string str(s->s_name);
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return gensym(str.c_str());
}

/** *= 100 or upper case */
Atom testMap1(const Atom& a)
{
    Atom res(a);

    if (a.isFloat())
        res.setFloat(a.asFloat() * 100);

    if (a.isSymbol())
        res.setSymbol(toUpper(a.asSymbol()));

    return res;
}

}

TEST_CASE("AtomListView", "core")
{
    SECTION("init")
    {
        SECTION("empty")
        {
            AtomListView av;
            REQUIRE(av.empty());
            REQUIRE(av.size() == 0);
            REQUIRE(av.isNull());
            REQUIRE_FALSE(av.isBool());
            REQUIRE_FALSE(av.isFloat());
            REQUIRE_FALSE(av.isSymbol());
            REQUIRE_FALSE(av.isData());
            REQUIRE_FALSE(av.isA<bool>());
            REQUIRE_FALSE(av.isA<t_int>());
            REQUIRE_FALSE(av.isA<t_float>());
            REQUIRE_FALSE(av.isA<t_symbol*>());
        }

        SECTION("bool")
        {
            AtomList l0(LA("true"));
            AtomListView lv(l0);

            REQUIRE(lv.isBool());
            REQUIRE(lv.isAtom());
            REQUIRE(lv.isSymbol());
            REQUIRE_FALSE(lv.isInteger());
            REQUIRE_FALSE(lv.isNull());

            REQUIRE(lv == true);
        }

        SECTION("float")
        {
            AtomList l(1.5);
            AtomListView lv(l);
            REQUIRE(lv.asInt() == 1);

            REQUIRE(lv.isFloat());
            REQUIRE(lv.isAtom());
            REQUIRE_FALSE(lv.isInteger());
            REQUIRE_FALSE(lv.isBool());
            REQUIRE_FALSE(lv.isNull());
            REQUIRE_FALSE(lv.isSymbol());
            REQUIRE(lv == t_float(1.5));
            REQUIRE_FALSE(lv == t_float(1.51));
        }

        SECTION("int")
        {
            AtomList l(100);
            AtomListView lv(l);

            REQUIRE(lv.isFloat());
            REQUIRE(lv.isAtom());
            REQUIRE(lv.isInteger());
            REQUIRE_FALSE(lv.isBool());
            REQUIRE_FALSE(lv.isNull());
            REQUIRE_FALSE(lv.isSymbol());
            REQUIRE(lv == 100);
            REQUIRE_FALSE(lv == 101);
        }

        SECTION("symbol")
        {
            AtomList l(LA("ABC"));
            AtomListView lv(l);

            REQUIRE_FALSE(lv.isFloat());
            REQUIRE(lv.isAtom());
            REQUIRE_FALSE(lv.isInteger());
            REQUIRE_FALSE(lv.isBool());
            REQUIRE_FALSE(lv.isNull());
            REQUIRE(lv.isSymbol());
            REQUIRE(lv == SYM("ABC"));
            REQUIRE_FALSE(lv == SYM("AB"));
        }

        SECTION("AtomList")
        {
            AtomList l({ 0, 1, 2, 3, 4 });
            AtomListView lv(l);
            REQUIRE(lv.size() == 5);
            REQUIRE_FALSE(lv.isNull());
            REQUIRE_FALSE(lv.empty());
            REQUIRE_FALSE(lv.isBool());
            REQUIRE_FALSE(lv.isFloat());
            REQUIRE_FALSE(lv.isSymbol());
            REQUIRE(lv[0] == A(0.));
            REQUIRE(lv[1] == A(1));
            REQUIRE(lv[2] == A(2));
            REQUIRE(lv[3] == A(3));
            REQUIRE(lv[4] == A(4));
            REQUIRE(lv.boolAt(0, true) == false);
            REQUIRE(lv.boolAt(0, true) == false);
            REQUIRE(lv.boolAt(1, true) == true);
            REQUIRE(lv.boolAt(1, false) == true);
            REQUIRE(lv.intAt(2, 0) == 2);
            REQUIRE(lv.floatAt(3, 0) == 3);
            REQUIRE(lv.symbolAt(0, nullptr) == nullptr);

            REQUIRE(lv.boolAt(5, true) == true);
            REQUIRE(lv.boolAt(5, false) == false);
            REQUIRE(lv.intAt(5, -1) == -1);
            REQUIRE(lv.floatAt(5, -1) == -1);
            REQUIRE(lv.symbolAt(5, SYM("?")) == SYM("?"));

            REQUIRE(std::all_of(lv.begin(), lv.end(), [](const Atom& a) { return a.isFloat(); }));
            REQUIRE(std::none_of(lv.begin(), lv.end(), [](const Atom& a) { return a.isData(); }));

            // check iteration
            for (auto a : lv) {
            }

            REQUIRE(lv == lv);
            REQUIRE_FALSE(lv != lv);
            // subView
            REQUIRE(lv.subView(0) == l);
            REQUIRE(lv.subView(0) == lv.subView(0, 5));
            REQUIRE(lv.subView(0) == LF(0, 1, 2, 3, 4));
            REQUIRE(lv.subView(1) == LF(1, 2, 3, 4));
            REQUIRE(lv.subView(2) == LF(2, 3, 4));
            REQUIRE(lv.subView(3) == LF(3, 4));
            REQUIRE(lv.subView(4) == LF(4));
            REQUIRE(lv.subView(5) == L());

            REQUIRE(lv.subView(0, 6) == LF(0, 1, 2, 3, 4));
            REQUIRE(lv.subView(0, 5) == LF(0, 1, 2, 3, 4));
            REQUIRE(lv.subView(0, 4) == LF(0, 1, 2, 3));
            REQUIRE(lv.subView(0, 3) == LF(0, 1, 2));
            REQUIRE(lv.subView(0, 2) == LF(0, 1));
            REQUIRE(lv.subView(0, 1) == LF(0));
            REQUIRE(lv.subView(0, 0) == L());

            REQUIRE(lv.subView(1, 5) == LF(1, 2, 3, 4));
            REQUIRE(lv.subView(1, 4) == LF(1, 2, 3, 4));
            REQUIRE(lv.subView(1, 3) == LF(1, 2, 3));
            REQUIRE(lv.subView(1, 2) == LF(1, 2));
            REQUIRE(lv.subView(1, 1) == LF(1));
            REQUIRE(lv.subView(1, 0) == L());

            REQUIRE(lv.subView(2, 4) == LF(2, 3, 4));
            REQUIRE(lv.subView(2, 3) == LF(2, 3, 4));
            REQUIRE(lv.subView(2, 2) == LF(2, 3));
            REQUIRE(lv.subView(2, 1) == LF(2));
            REQUIRE(lv.subView(2, 0) == L());

            REQUIRE(lv.subView(3, 3) == LF(3, 4));
            REQUIRE(lv.subView(3, 2) == LF(3, 4));
            REQUIRE(lv.subView(3, 1) == LF(3));
            REQUIRE(lv.subView(3, 0) == L());

            REQUIRE(lv.subView(4, 2) == LF(4));
            REQUIRE(lv.subView(4, 1) == LF(4));
            REQUIRE(lv.subView(4, 0) == L());

            REQUIRE(lv.subView(5, 1) == L());
            REQUIRE(lv.subView(5, 0) == L());
        }
    }

    SECTION("contains")
    {
        AtomList l({ 1, 2, 3, 4 });
        REQUIRE(l.view().contains(A(1)));
        REQUIRE_FALSE(AtomListView().contains(A(100)));
    }

    SECTION("allOf")
    {

#define REQUIRE_ALL_OF(l0, p)       \
    {                               \
        AtomList l = l0;            \
        REQUIRE(l.view().allOf(p)); \
    }

#define REQUIRE_NOT_ALL_OF(l0, p)         \
    {                                     \
        AtomList l = l0;                  \
        REQUIRE_FALSE(l.view().allOf(p)); \
    }

        REQUIRE_ALL_OF(L(), AtomPredicate(isFloat));
        REQUIRE_ALL_OF(LF(1, 2, 3), AtomPredicate(isFloat));
        REQUIRE_ALL_OF(LF(1), AtomPredicate(isFloat));
        REQUIRE_NOT_ALL_OF(LA("a"), AtomPredicate(isFloat));
        REQUIRE_NOT_ALL_OF(LA(1, 2, 3, "a"), AtomPredicate(isFloat));
    }

    SECTION("allOf")
    {

#define REQUIRE_ANY_OF(l0, p)       \
    {                               \
        AtomList l = l0;            \
        REQUIRE(l.view().anyOf(p)); \
    }

#define REQUIRE_NOT_ANY_OF(l0, p)         \
    {                                     \
        AtomList l = l0;                  \
        REQUIRE_FALSE(l.view().anyOf(p)); \
    }

        REQUIRE_NOT_ANY_OF(L(), isFloat);
        REQUIRE_NOT_ANY_OF(LA("a"), isFloat);
        REQUIRE_ANY_OF(LA(1, "a"), isFloat);
        REQUIRE_ANY_OF(LF(1, 2), isFloat);
    }

    SECTION("noneOf")
    {

#define REQUIRE_NONE_OF(l0, p)       \
    {                                \
        AtomList l = l0;             \
        REQUIRE(l.view().noneOf(p)); \
    }

        REQUIRE_NONE_OF(L(), isFloat);
        REQUIRE_NONE_OF(LA("a"), isFloat);
        REQUIRE_NONE_OF(LA(1, 2), isSymbol);
    }

    SECTION("ranges")
    {
        AtomList l = LA("a", -100, 0., 100);
        AtomListView v(l);

        REQUIRE(v.floatLessThenAt(0, 0, -1024) == -1024);
        REQUIRE(v.floatLessThenAt(1, 0, -1024) == -100);
        REQUIRE(v.floatLessThenAt(2, 0, -1024) == -1024);
        REQUIRE(v.floatLessThenAt(3, 0, -1024) == -1024);
        REQUIRE(v.floatLessThenAt(4, 0, -1024) == -1024);
        REQUIRE(v.floatLessThenAt(5, 0, -1024) == -1024);

        REQUIRE(v.floatLessEqualAt(0, 0, -1024) == -1024);
        REQUIRE(v.floatLessEqualAt(1, 0, -1024) == -100);
        REQUIRE(v.floatLessEqualAt(2, 0, -1024) == 0);
        REQUIRE(v.floatLessEqualAt(3, 0, -1024) == -1024);
        REQUIRE(v.floatLessEqualAt(4, 0, -1024) == -1024);
        REQUIRE(v.floatLessEqualAt(5, 0, -1024) == -1024);

        REQUIRE(v.floatGreaterThenAt(0, 0, 1024) == 1024);
        REQUIRE(v.floatGreaterThenAt(1, 0, 1024) == 1024);
        REQUIRE(v.floatGreaterThenAt(2, 0, 1024) == 1024);
        REQUIRE(v.floatGreaterThenAt(3, 0, 1024) == 100);
        REQUIRE(v.floatGreaterThenAt(4, 0, 1024) == 1024);
        REQUIRE(v.floatGreaterThenAt(5, 0, 1024) == 1024);

        REQUIRE(v.floatGreaterEqualAt(0, 0, 1024) == 1024);
        REQUIRE(v.floatGreaterEqualAt(1, 0, 1024) == 1024);
        REQUIRE(v.floatGreaterEqualAt(2, 0, 1024) == 0);
        REQUIRE(v.floatGreaterEqualAt(3, 0, 1024) == 100);
        REQUIRE(v.floatGreaterEqualAt(4, 0, 1024) == 1024);
        REQUIRE(v.floatGreaterEqualAt(5, 0, 1024) == 1024);

        REQUIRE(v.intLessThenAt(0, 0, -1024) == -1024);
        REQUIRE(v.intLessThenAt(1, 0, -1024) == -100);
        REQUIRE(v.intLessThenAt(2, 0, -1024) == -1024);
        REQUIRE(v.intLessThenAt(3, 0, -1024) == -1024);
        REQUIRE(v.intLessThenAt(4, 0, -1024) == -1024);
        REQUIRE(v.intLessThenAt(5, 0, -1024) == -1024);

        REQUIRE(v.intLessEqualAt(0, 0, -1024) == -1024);
        REQUIRE(v.intLessEqualAt(1, 0, -1024) == -100);
        REQUIRE(v.intLessEqualAt(2, 0, -1024) == 0);
        REQUIRE(v.intLessEqualAt(3, 0, -1024) == -1024);
        REQUIRE(v.intLessEqualAt(4, 0, -1024) == -1024);
        REQUIRE(v.intLessEqualAt(5, 0, -1024) == -1024);

        REQUIRE(v.intGreaterThenAt(0, 0, 1024) == 1024);
        REQUIRE(v.intGreaterThenAt(1, 0, 1024) == 1024);
        REQUIRE(v.intGreaterThenAt(2, 0, 1024) == 1024);
        REQUIRE(v.intGreaterThenAt(3, 0, 1024) == 100);
        REQUIRE(v.intGreaterThenAt(4, 0, 1024) == 1024);
        REQUIRE(v.intGreaterThenAt(5, 0, 1024) == 1024);

        REQUIRE(v.intGreaterEqualAt(0, 0, 1024) == 1024);
        REQUIRE(v.intGreaterEqualAt(1, 0, 1024) == 1024);
        REQUIRE(v.intGreaterEqualAt(2, 0, 1024) == 0);
        REQUIRE(v.intGreaterEqualAt(3, 0, 1024) == 100);
        REQUIRE(v.intGreaterEqualAt(4, 0, 1024) == 1024);
        REQUIRE(v.intGreaterEqualAt(5, 0, 1024) == 1024);
    }

    SECTION("back")
    {
        REQUIRE(LF(1, 2, 3, 4, 5).view().back() == 5);
        REQUIRE(LF(1, 2, 3, 4, 5).view().front() == 1);
        REQUIRE(LF(1, 2, 3, 4, 5).view().at(0) == 1);
        REQUIRE(LF(1, 2, 3, 4, 5).view().at(1) == 2);
        REQUIRE(LF(1, 2, 3, 4, 5).view().at(2) == 3);
        REQUIRE(LF(1, 2, 3, 4, 5).view().at(3) == 4);
        REQUIRE(LF(1, 2, 3, 4, 5).view().at(4) == 5);
        REQUIRE(LF(1, 2, 3, 4, 5).view()[0] == 1);
    }

    SECTION("test reduce")
    {
        AtomList l;
        REQUIRE(l.view().reduceFloat(1.f, &floatSum) == boost::none);
        REQUIRE(l.view().reduceFloat(1.f, &floatMul) == boost::none);
        l.append(1.f);
        l.append(2.f);
        REQUIRE(l.view().reduceFloat(0.5f, &floatSum) == MaybeFloat(3.5));
        REQUIRE(l.view().reduceFloat(2.f, &floatMul) == MaybeFloat(4));
        l.append(gensym("a"));
        REQUIRE(l.view().reduceFloat(0.5f, &floatSum) == MaybeFloat(3.5));
        REQUIRE(l.view().reduceFloat(2.f, &floatMul) == MaybeFloat(4));
        l.append(2.f);
        REQUIRE(l.view().reduceFloat(0.5f, &floatSum) == MaybeFloat(5.5));
        REQUIRE(l.view().reduceFloat(2.f, &floatMul) == MaybeFloat(8));
    }

    SECTION("test map")
    {
#define REQUIRE_MAP_FLOAT(lst, fn, res) \
    {                                   \
        AtomList out;                   \
        lst.view().mapFloat(fn, out);   \
        REQUIRE(out == res);            \
    }

#define REQUIRE_MAP_SYM(lst, fn, res)  \
    {                                  \
        AtomList out;                  \
        lst.view().mapSymbol(fn, out); \
        REQUIRE(out == res);           \
    }

#define REQUIRE_MAP_ATOM(lst, fn, res) \
    {                                  \
        AtomList out;                  \
        lst.view().map(fn, out);       \
        REQUIRE(out == res);           \
    }

        SECTION("float")
        {
            REQUIRE_MAP_FLOAT(LF(1, 2, 3), mul10, LF(10, 20, 30));
            REQUIRE_MAP_FLOAT(LA("a", "b", "@c"), neg, LA("a", "b", "@c"));
            REQUIRE_MAP_FLOAT(LA("a", 100, "c"), neg, LA("a", -100, "c"));
        }

        SECTION("symbol")
        {
            REQUIRE_MAP_SYM(LF(1, 2, 3), toUpper, LF(1, 2, 3));
            REQUIRE_MAP_SYM(LA("a", "b", "c"), toUpper, LA("A", "B", "C"));
            REQUIRE_MAP_SYM(LA("@a", "b", "c"), toUpper, LA("@A", "B", "C"));
            REQUIRE_MAP_SYM(LA("A", "B", "C"), toUpper, LA("A", "B", "C"));
            REQUIRE_MAP_SYM(LA("a", 100, "c"), toUpper, LA("A", 100, "C"));
        }

        SECTION("atom")
        {
            REQUIRE_MAP_ATOM(LF(1, 2, 3), &testMap1, LF(100, 200, 300));
            REQUIRE_MAP_ATOM(LA("a", 0.5, "@c"), &testMap1, LAX("A", 50, "@C"));
        }

        SECTION("map atom")
        {
            auto fn = [](const Atom& a) { return Atom(a.isFloat()); };
            REQUIRE_MAP_ATOM(LA(1, "a", 3), fn, LX(1, 0, 1));
        }

        SECTION("map float")
        {
            auto fn = [](t_float f) { return f * 2; };
            REQUIRE_MAP_FLOAT(LF(1, 2, 3), fn, LF(2, 4, 6));
        }
    }

    SECTION("filtered")
    {
#define REQUIRE_FILTER_ATOM(lst, fn, res) \
    {                                     \
        AtomList out;                     \
        lst.view().filter(fn, out);       \
        CHECK(out == res);                \
    }

        REQUIRE_FILTER_ATOM(LA("a", 2, 3, "b"), nullptr, LA("a", 2, 3, "b"));
        REQUIRE_FILTER_ATOM(LA("a", 2, 3, "b"), isFloat, LF(2, 3));
        REQUIRE_FILTER_ATOM(LA("a", 2, 3, "b"), isSymbol, LA("a", "b"));
    }

    SECTION("getProperty")
    {
        AtomListView res;
        REQUIRE_FALSE(AtomListView().getProperty(SYM("@p"), res));
        REQUIRE(res.empty());
        REQUIRE(AtomListView(LA("@p")).getProperty(SYM("@p"), res));
        REQUIRE(res.empty());
        REQUIRE(AtomListView(LA(1, 2, 3, "@p")).getProperty(SYM("@p"), res));
        REQUIRE(res.empty());
        REQUIRE(AtomListView(LA(1, 2, 3, "@p", 4)).getProperty(SYM("@p"), res));
        REQUIRE(res == LF(4));
        REQUIRE(AtomListView(LA(1, 2, 3, "@p", 4, 5)).getProperty(SYM("@p"), res));
        REQUIRE(res == LF(4, 5));
        REQUIRE(AtomListView(LA(1, 2, 3, "@p", 4, 5, 6, "@other")).getProperty(SYM("@p"), res));
        REQUIRE(res == LF(4, 5, 6));
        REQUIRE(AtomListView(LA(1, 2, 3, "@p", "@other")).getProperty(SYM("@p"), res));
        REQUIRE(res.empty());
    }

    SECTION("argSubView")
    {
        REQUIRE(AtomListView(LA(1, 2, 3, 4)).arguments(0) == LF(1, 2, 3, 4));
        REQUIRE(AtomListView(LA(1, 2, 3, 4)).arguments(1) == LF(2, 3, 4));
        REQUIRE(AtomListView(LA(1, 2, 3, 4)).arguments(2) == LF(3, 4));
        REQUIRE(AtomListView(LA(1, 2, 3, 4)).arguments(3) == LF(4));
        REQUIRE(AtomListView(LA(1, 2, 3, 4)).arguments(4) == L());

        REQUIRE(AtomListView(LA(1, 2, "@a", 4)).arguments(0) == LF(1, 2));
        REQUIRE(AtomListView(LA(1, 2, "@a", 4)).arguments(1) == LF(2));
        REQUIRE(AtomListView(LA(1, 2, "@a", 4)).arguments(2) == L());
        REQUIRE(AtomListView(LA(1, 2, "@a", 4)).arguments(3) == L());
        REQUIRE(AtomListView(LA(1, 2, "@a", 4)).arguments(4) == L());

        REQUIRE(AtomListView(LA("@a", 4)).arguments(0) == L());
        REQUIRE(AtomListView(LA("@a", 4)).arguments(1) == L());
    }

    SECTION("propSubView")
    {
        REQUIRE(AtomListView(L()).properties() == L());
        REQUIRE(AtomListView(LF(1)).properties() == L());
        REQUIRE(AtomListView(LA("a")).properties() == L());
        REQUIRE(AtomListView(LA("@a")).properties() == LA("@a"));
        REQUIRE(AtomListView(LA("@a", 1)).properties() == LA("@a", 1));
        REQUIRE(AtomListView(LA("@a", 1, "@b")).properties() == LA("@a", 1, "@b"));
        REQUIRE(AtomListView(LA(1, "@a", 1, "@b")).properties() == LA("@a", 1, "@b"));
        REQUIRE(AtomListView(LA(1, 2, 3, 4, 5, "@a", 1, "@b")).properties() == LA("@a", 1, "@b"));
    }

    SECTION("expandDollarArgs")
    {
        CanvasPtr cnv = PureData::instance().createTopCanvas(TEST_DATA_DIR "/patch_cnv_current", LF(100, 200, 300));
        char buf[32];
        sprintf(buf, "%d", cnv->dollarZero());

        AtomList res;
        REQUIRE(AtomListView(L()).expandDollarArgs(cnv->pd_canvas(), res));
        REQUIRE(res == L());
        REQUIRE(AtomListView(LF(1, 2, 3)).expandDollarArgs(cnv->pd_canvas(), res));
        REQUIRE(res == LF(1, 2, 3));

        REQUIRE(AtomListView(LF(Atom::dollar(1))).expandDollarArgs(cnv->pd_canvas(), res));
        REQUIRE(res == LF(1, 2, 3, 100));
        REQUIRE(AtomListView(LF(Atom::dollar(3))).expandDollarArgs(cnv->pd_canvas(), res));
        REQUIRE(res == LF(1, 2, 3, 100, 300));
        REQUIRE(AtomListView(LF(Atom::dollar(4))).expandDollarArgs(cnv->pd_canvas(), res));
        REQUIRE(res == LF(1, 2, 3, 100, 300, 0));
        REQUIRE(AtomListView(LF(Atom::dollar(4))).expandDollarArgs(cnv->pd_canvas(), res));
        REQUIRE(res == LF(1, 2, 3, 100, 300, 0, 0));
        REQUIRE(AtomListView(LF(Atom::dollar(10))).expandDollarArgs(cnv->pd_canvas(), res, AtomListView::InvalidDollarArgPolicy::DROP_ARG));
        REQUIRE(res == LF(1, 2, 3, 100, 300, 0, 0));
        REQUIRE(AtomListView(LF(Atom::dollar(10))).expandDollarArgs(cnv->pd_canvas(), res, AtomListView::InvalidDollarArgPolicy::DEFAULT_ARG, Atom(-100)));
        REQUIRE(res == LF(1, 2, 3, 100, 300, 0, 0, -100));
        REQUIRE(AtomListView(LF(Atom::dollar(10))).expandDollarArgs(cnv->pd_canvas(), res, AtomListView::InvalidDollarArgPolicy::KEEP_RAW));
        REQUIRE(res == LA(1, 2, 3, 100, 300, 0., 0., -100, Atom::dollar(10)));
    }

    SECTION("restorePrimitive")
    {
#define REQUIRE_PRIM(v0, v1)                       \
    {                                              \
        AtomList data = v0;                        \
        AtomList out;                              \
        AtomListView(data).restorePrimitives(out); \
        REQUIRE(out == v1);                        \
    }

        REQUIRE_PRIM(L(), L());
        REQUIRE_PRIM(LF(1, 2, 3), LF(1, 2, 3));
        REQUIRE_PRIM(LA(";"), LA(Atom::semicolon()));
        REQUIRE_PRIM(LA(","), LA(Atom::comma()));
        REQUIRE_PRIM(LA("$1"), LA(Atom::dollar(1)));
        REQUIRE_PRIM(LA("$9"), LA(Atom::dollar(9)));
        REQUIRE_PRIM(LA("$1-test"), LA(Atom::dollarSymbol(gensym("$1-test"))));
    }

    SECTION("split")
    {
        using Result = std::vector<AtomList>;
        Result res;
        auto fn = [&res](const AtomListView& lv) { res.push_back(lv); };

        AtomListView().split(Atom(1), {});

#define REQUIRE_SPLIT(lv, a, ...)               \
    do {                                        \
        res.clear();                            \
        lv.view().split(a, fn);                 \
        REQUIRE(res == Result { __VA_ARGS__ }); \
    } while (0)

        REQUIRE_SPLIT(L(), A(1), L());
        REQUIRE_SPLIT(LF(1, 2, 3), A(100), LF(1, 2, 3));
        REQUIRE_SPLIT(LF(1, 2, 3), A(2), LF(1), LF(3));
        REQUIRE_SPLIT(LF(1, 2, 3), A(1), L(), LF(2, 3));
        REQUIRE_SPLIT(LF(1, 2, 3), A(3), LF(1, 2), L());
        REQUIRE_SPLIT(LA(1, Atom::comma(), 3), Atom::comma(), LF(1), LF(3));
    }

    SECTION("compare operators")
    {
        REQUIRE_FALSE(L().view() < 0);
        REQUIRE_FALSE(L().view() <= 0);
        REQUIRE_FALSE(L().view() > 0);
        REQUIRE_FALSE(L().view() >= 0);

        REQUIRE_FALSE(LF(0).view() < 0);
        REQUIRE(LF(0).view() <= 0);
        REQUIRE_FALSE(LF(0).view() > 0);
        REQUIRE(LF(0).view() >= 0);

        REQUIRE_FALSE(LF(0.5).view() < 0.5);
        REQUIRE(LF(0.5).view() <= 0.5);
        REQUIRE_FALSE(LF(0.5).view() > 0.5);
        REQUIRE(LF(0.5).view() >= 0.5);

        REQUIRE_FALSE(LA("A").view() < 0);
        REQUIRE_FALSE(LA("B").view() <= 0);
        REQUIRE_FALSE(LA("C").view() > 0);
        REQUIRE_FALSE(LA("D").view() >= 0);

        REQUIRE_FALSE(LA("A", "A").view() < 0);
        REQUIRE_FALSE(LA("B", "B").view() <= 0);
        REQUIRE_FALSE(LA("C", "C").view() > 0);
        REQUIRE_FALSE(LA("D", "D").view() >= 0);

        REQUIRE_FALSE(LF(1, 2).view() < 0);
        REQUIRE_FALSE(LF(1, 2).view() <= 0);
        REQUIRE_FALSE(LF(1, 2).view() > 0);
        REQUIRE_FALSE(LF(1, 2).view() >= 0);

        REQUIRE_FALSE(L().view().isIntInClosedInterval(1, 10));
        REQUIRE_FALSE(L().view().isIntGreaterEqual(1));
        REQUIRE_FALSE(L().view().isIntGreaterThen(1));
        REQUIRE_FALSE(L().view().isIntLessEqual(1));
        REQUIRE_FALSE(L().view().isIntLessThen(1));

        REQUIRE_FALSE(LA("A").view().isIntInClosedInterval(1, 10));
        REQUIRE_FALSE(LA("A").view().isIntGreaterEqual(1));
        REQUIRE_FALSE(LA("A").view().isIntGreaterThen(1));
        REQUIRE_FALSE(LA("A").view().isIntLessEqual(1));
        REQUIRE_FALSE(LA("A").view().isIntLessThen(1));

        REQUIRE_FALSE(LA(1, 2).view().isIntInClosedInterval(1, 10));
        REQUIRE_FALSE(LA(1, 2).view().isIntGreaterEqual(1));
        REQUIRE_FALSE(LA(1, 2).view().isIntGreaterThen(1));
        REQUIRE_FALSE(LA(1, 2).view().isIntLessEqual(1));
        REQUIRE_FALSE(LA(1, 2).view().isIntLessThen(1));
    }

    SECTION("allFloatsOf")
    {
        REQUIRE(L().view().allFloatsOf([](t_float x) { return x > 10; }));
        REQUIRE(LF(11).view().allFloatsOf([](t_float x) { return x > 10; }));
        REQUIRE_FALSE(LA("A").view().allFloatsOf([](t_float x) { return x > 10; }));
        REQUIRE(LF(11, 12, 13).view().allFloatsOf([](t_float x) { return x > 10; }));
        REQUIRE_FALSE(LA(11, "A", 13).view().allFloatsOf([](t_float x) { return x > 10; }));
        REQUIRE_FALSE(LF(11, 12, 13, 9).view().allFloatsOf([](t_float x) { return x > 10; }));
    }
}
