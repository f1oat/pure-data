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
#include "flow_select.h"
#include "ceammc_factory.h"
#include "ceammc_format.h"
#include "lex/select.lexer.h"
#include "lex/select.parser.hpp"

#include <algorithm>

class SelectMatch {
public:
    using Data = SelectLexer::MatchData;
    std::vector<Data> data;
};

FlowSelect::FlowSelect(const PdArgs& args)
    : BaseObject(args)
    , patterns_(new SelectMatch())
    , keep_value_(nullptr)
{
    try {
        keep_value_ = new BoolProperty("@keep_value", false);
        addProperty(keep_value_);

        addProperty(new AliasProperty<BoolProperty>("@v", keep_value_, true));

        SelectLexer l(to_string(unparsedPosArgs()));
        SelectParser p(l);

        auto err = p.parse();
        if (err) {
            OBJ_ERR << "parse error: ";
        } else {
            for (size_t i = 0; i < l.numMatches(); i++) {
                auto& m = l[i];
                patterns_->data.push_back(m);
                createOutlet();
            }

            createOutlet();
        }

    } catch (std::exception& e) {
        OBJ_ERR << "parser error: " << e.what();
    }
}

// for unique_ptr
FlowSelect::~FlowSelect() { }

void FlowSelect::onFloat(t_float v)
{
    if (patterns_->data.empty())
        return floatTo(0, v);

    auto it = patterns_->data.crbegin();
    auto end = patterns_->data.crend();

    using l = SelectLexer;
    const size_t LAST = numOutlets() - 1;
    bool matched = false;

    auto output = [this, &matched](t_float v, size_t idx) {
        matched = true;
        if (keep_value_->value())
            floatTo(idx, v);
        else
            bangTo(idx);
    };

    for (size_t idx = LAST - 1; it != end; ++it, --idx) {
        switch (it->type()) {
        case l::MATCH_EQUAL:
            if (it->at(0) == Atom(v))
                output(v, idx);

            break;
        case l::MATCH_GREATER:
            if (v > it->at(0).asT<t_float>())
                output(v, idx);

            break;
        case l::MATCH_GREATER_EQ:
            if (v >= it->at(0).asT<t_float>())
                output(v, idx);

            break;
        case l::MATCH_LESS:
            if (v < it->at(0).asT<t_float>())
                output(v, idx);

            break;
        case l::MATCH_LESS_EQ:
            if (v <= it->at(0).asT<t_float>())
                output(v, idx);

            break;
        case l::MATCH_RANGE_CC: {
            const auto a = it->at(0).asT<t_float>();
            const auto b = it->at(1).asT<t_float>();

            if (a <= v && v <= b)
                output(v, idx);

        } break;
        case l::MATCH_RANGE_CO: {
            const auto a = it->at(0).asT<t_float>();
            const auto b = it->at(1).asT<t_float>();

            if (a <= v && v < b)
                output(v, idx);

        } break;
        case l::MATCH_RANGE_OO: {
            const auto a = it->at(0).asT<t_float>();
            const auto b = it->at(1).asT<t_float>();

            if (a < v && v < b)
                output(v, idx);

        } break;
        case l::MATCH_RANGE_OC: {
            const auto a = it->at(0).asT<t_float>();
            const auto b = it->at(1).asT<t_float>();

            if (a < v && v <= b)
                output(v, idx);

        } break;
        case l::MATCH_EPSILON: {
            const auto x = it->at(0).asT<t_float>();
            const auto e = it->at(1).asT<t_float>();

            if (std::fabs(x - v) < e)
                output(v, idx);

        } break;
        case l::MATCH_SET: {
            auto mit = std::find_if(it->begin(), it->end(), [v](const Atom& a) { return a == Atom(v); });
            if (mit != it->end())
                output(v, idx);

        } break;
        default:
            break;
        }
    }

    if (!matched)
        floatTo(LAST, v);
}

void FlowSelect::onSymbol(t_symbol* s)
{
    if (patterns_->data.empty())
        return symbolTo(0, s);

    auto it = patterns_->data.crbegin();
    auto end = patterns_->data.crend();

    using l = SelectLexer;
    const size_t LAST = numOutlets() - 1;
    bool matched = false;

    auto output = [this, &matched](t_symbol* s, size_t idx) {
        matched = true;
        if (keep_value_->value())
            symbolTo(idx, s);
        else
            bangTo(idx);
    };

    for (size_t idx = LAST - 1; it != end; ++it, --idx) {
        switch (it->type()) {
        case l::MATCH_EQUAL:
            if (it->at(0) == Atom(s))
                output(s, idx);
            break;
        case l::MATCH_SET: {
            auto mit = std::find_if(it->begin(), it->end(), [s](const Atom& a) { return a == Atom(s); });
            if (mit != it->end())
                output(s, idx);

        } break;
        default:
            break;
        }
    }

    if (!matched)
        symbolTo(LAST, s);
}

void setup_flow_select()
{
    ObjectFactory<FlowSelect> obj("flow.select");
    obj.addAlias("flow.sel");
    obj.noArgsDataParsing();
}
