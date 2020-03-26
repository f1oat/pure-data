/*****************************************************************************
 * Copyright 2016 Serge Poltavsky. All rights reserved.
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
#include "ceammc_format.h"
#include "ceammc_atom.h"
#include "ceammc_atomlist.h"
#include "ceammc_atomlist_view.h"
#include "ceammc_data.h"
#include "ceammc_dataatom.h"
#include "ceammc_message.h"
#include "fmt/format.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace ceammc {

template <class T>
static inline T clip(T v, T min, T max)
{
    return std::min(max, std::max(min, v));
}

std::string to_string(const Atom& a)
{
    if (a.isSymbol())
        return std::string(a.asSymbol()->s_name);

    if (a.isData())
        return to_string(DataPtr(a));

    std::ostringstream ss;
    ss << a;
    return ss.str();
}

std::string to_string_quoted(const Atom& a)
{
    if (a.isSymbol())
        return to_string_quoted(a.asSymbol()->s_name);
    else if (a.isData())
        return to_string(a);
    return to_string(a);
}

std::string to_float_string(const Atom& a, const std::string& defaultValue)
{
    if (!a.isFloat())
        return defaultValue;
    else if (a.isInteger())
        return fmt::format("{}", a.asInt());
    else
        return fmt::format("{}", a.asFloat());
}

std::string to_hex_string(const Atom& a, const std::string& defaultValue)
{
    if (!a.isFloat())
        return defaultValue;

    return fmt::format("{:X}", static_cast<long>(a.asFloat()));
}

std::string to_string(const AtomListView v, const std::string& separator)
{
    if (v.empty())
        return "";

    std::string res;
    for (size_t i = 0; i < v.size(); i++) {
        if (i != 0)
            res += separator;

        res += to_string(v[i]);
    }
    return res;
}

std::string to_string(const AtomList& l, const std::string& separator)
{
    if (l.empty())
        return "";

    std::string res;
    for (size_t i = 0; i < l.size(); i++) {
        if (i != 0)
            res += separator;

        res += to_string(l[i]);
    }
    return res;
}

std::string to_string(const Message& m, const std::string& separator)
{
    if (m.isFloat() || m.isSymbol())
        return to_string(m.atomValue());

    if (m.isList())
        return to_string(m.listValue(), separator);

    if (m.isAny())
        return to_string(m.atomValue()) + separator + to_string(m.listValue(), separator);

    return "";
}

std::string to_string_quoted(const std::string& str)
{
    if (str.empty())
        return "\"\"";

    static const char SINGLE_QUOTE = '"';
    static const char ESCAPE = '`';

    // no spaces -> no quotes
    if (std::string::npos == str.find(' ', 0))
        return str;

    std::string res;
    res.reserve(str.size() + 2);
    res += SINGLE_QUOTE;

    // no inner quotes -> just outer quotes
    if (std::string::npos == str.find(SINGLE_QUOTE, 0)) {
        res += str;
    } else {
        std::string escaped;
        escaped.reserve(str.size() + 1);

        for (size_t i = 0; i < str.size(); i++) {
            char c = str[i];

            if (c == SINGLE_QUOTE) {
                escaped += ESCAPE;
                escaped += SINGLE_QUOTE;
            } else
                escaped += c;
        }

        res += escaped;
    }

    res += SINGLE_QUOTE;
    return res;
}

std::string to_string(const DataPtr& p)
{
    return p.isNull() ? "???" : p->toString();
}

std::string quote(const std::string& str, char q)
{
    std::string res;
    res.reserve(str.size() + 2);
    res.push_back(q);
    res += str;
    res.push_back(q);
    return res;
}

static std::string quote_json(const std::string& str)
{
    std::string res;
    res.reserve(str.length() + 4);
    res.push_back('"');
    for (auto c : str) {
        if (c == '"') {
            res.push_back('\\');
            res.push_back(c);
        } else
            res.push_back(c);
    }
    res.push_back('"');
    return res;
}

std::string to_json_string(const Atom& a)
{
    if (a.isInteger())
        return fmt::format("{}", a.asInt());
    else if (a.isFloat())
        return fmt::format("{}", a.asFloat());
    else if (a.isSymbol()) {
        return quote_json(a.asSymbol()->s_name);
    } else if (a.isData())
        return quote_json(to_string(DataPtr(a)));
    else if (a.isNone())
        return "null";
    else {
        std::cerr << "ATOM: " << a << std::endl;
        return "?????????";
    }
}

std::string to_json_string(const AtomList& l)
{
    std::string res;
    res.push_back('[');

    for (size_t i = 0; i < l.size(); i++) {
        if (i > 0)
            res += ", ";

        res += to_json_string(l[i]);
    }

    res.push_back(']');
    return res;
}

} // namespace ceammc
