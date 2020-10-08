/*****************************************************************************
 * Copyright 2019 Serge Poltavsky. All rights reserved.
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
#include "ceammc_units.h"
#include "ceammc_format.h"
#include "lex/fraction.lexer.h"

#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <sstream>

using namespace ceammc;
using namespace ceammc::units;

Either<TimeValue> TimeValue::parse(const AtomListView& lv)
{
    static t_symbol* SYM_MSEC = gensym("ms");
    static t_symbol* SYM_MSEC2 = gensym("msec");
    static t_symbol* SYM_SEC = gensym("sec");
    static t_symbol* SYM_SEC2 = gensym("s");
    static t_symbol* SYM_MIN = gensym("min");
    static t_symbol* SYM_HOUR = gensym("hour");
    static t_symbol* SYM_DAY = gensym("day");
    static t_symbol* SYM_SAMPLE = gensym("samp");
    static t_symbol* SYM_SAMPLE2 = gensym("sample");

    if (lv.empty())
        return UnitParseError("empty list");

    if (lv.size() == 2) {
        const bool ok = lv[0].isFloat() && lv[1].isSymbol();
        if (!ok)
            return UnitParseError("VALUE UNIT expected");

        auto v = lv[0].asFloat();

        if (HUGE_VALF == v) {
            std::ostringstream ss;
            ss << "too big value for float: " << lv;
            return UnitParseError(ss.str());
        }

        auto s = lv[1].asSymbol();
        if (s == SYM_MSEC || s == SYM_MSEC2)
            return TimeValue(v, TimeUnits::MS);
        else if (s == SYM_SEC || s == SYM_SEC2)
            return TimeValue(v, TimeUnits::SEC);
        else if (s == SYM_SAMPLE || s == SYM_SAMPLE2)
            return TimeValue(v, TimeUnits::SAMPLE);
        else if (s == SYM_MIN)
            return TimeValue(v, TimeUnits::MIN);
        else if (s == SYM_HOUR)
            return TimeValue(v, TimeUnits::HOUR);
        else if (s == SYM_DAY)
            return TimeValue(v, TimeUnits::DAY);
        else {
            std::ostringstream ss;
            ss << "unknown unit: " << s << ",  supported values are: ms, sec(s), min, hour, day";
            return UnitParseError(ss.str());
        }
    } else if (lv.isSymbol()) {
        auto s = lv[0].asSymbol();
        char* last = nullptr;
        const auto v = strtof(s->s_name, &last);

        if (HUGE_VALF == v) {
            std::ostringstream ss;
            ss << "too big value for float: " << lv;
            return UnitParseError(ss.str());
        }

        //no suffix
        if (last[0] == '\0')
            return TimeValue(v, TimeUnits::MS);

        t_symbol* suffix = gensym(last);

        if (suffix == SYM_MSEC || suffix == SYM_MSEC2)
            return TimeValue(v, TimeUnits::MS);
        else if (suffix == SYM_SEC || suffix == SYM_SEC2)
            return TimeValue(v, TimeUnits::SEC);
        else if (suffix == SYM_SAMPLE || suffix == SYM_SAMPLE2)
            return TimeValue(v, TimeUnits::SAMPLE);
        else if (suffix == SYM_MIN)
            return TimeValue(v, TimeUnits::MIN);
        else if (suffix == SYM_HOUR)
            return TimeValue(v, TimeUnits::HOUR);
        else if (suffix == SYM_DAY)
            return TimeValue(v, TimeUnits::DAY);
        else {
            std::ostringstream ss;
            ss << "unknown unit: " << suffix << ",  supported values are: ms, sec(s), min, hour, day, samp(le)";
            return UnitParseError(ss.str());
        }
    } else if (lv.isFloat()) {
        return TimeValue(lv[0].asFloat(), TimeUnits::MS);
    } else {
        std::ostringstream ss;
        ss << "unexpected time format: " << lv;
        return UnitParseError(ss.str());
    }
}

UnitParseError::UnitParseError(const char* s)
    : msg(s)
{
}

UnitParseError::UnitParseError(const std::string& s)
    : msg(s)
{
}

Either<FractionValue> FractionValue::parse(const AtomListView& lv)
{
    if (lv.isSymbol()) {
        auto cstr = lv.asT<t_symbol*>()->s_name;
        fraction::FractionLexer lexer(cstr);
        if (lexer.lex() != 1)
            return UnitParseError("invalid fraction");
        else
            return lexer.value;
    } else {
        return UnitParseError("invalid fraction");
    }
}

Either<FractionValue> FractionValue::match(const AtomListView& lv)
{
    if (lv.size() > 0)
        return parse(lv.subView(0, 1));
    else
        return UnitParseError("not a fraction");
}

FractionValue FractionValue::ratio(long num, long den)
{
    FractionValue res(num, FracUnits::RATIO);
    res.denom_ = den;
    return res;
}
