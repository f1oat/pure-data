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
#include "ceammc_atomlist_view.h"
#include "ceammc_atomlist.h"
#include "ceammc_convert.h"
#include "ceammc_numeric.h"

#include <cmath>
#include <functional>

namespace ceammc {

AtomListView::AtomListView() noexcept
    : data_(nullptr)
    , n_(0)
{
}

AtomListView::AtomListView(const t_atom* a, size_t n) noexcept
    : data_(n ? reinterpret_cast<const Atom*>(a) : nullptr)
    , n_(n)
{
}

AtomListView::AtomListView(const Atom* a, size_t n) noexcept
    : data_(n ? a : nullptr)
    , n_(n)
{
}

AtomListView::AtomListView(const Atom& a) noexcept
    : data_(&a)
    , n_(1)
{
}

AtomListView::AtomListView(const AtomList& l) noexcept
    : AtomListView(l.toPdData(), l.size())
{
}

AtomListView::AtomListView(const t_binbuf* b) noexcept
{
    if (b) {
        n_ = binbuf_getnatom(b);
        data_ = n_ ? reinterpret_cast<const Atom*>(binbuf_getvec(b)) : nullptr;
    }
}

void AtomListView::set(t_atom* a, size_t n)
{
    data_ = reinterpret_cast<Atom*>(a);
    n_ = n;
}

void AtomListView::set(const Atom& a)
{
    data_ = &a;
    n_ = 1;
}

void AtomListView::set(const AtomList& l)
{
    set(l.toPdData(), l.size());
}

bool AtomListView::get(t_atom*& a, int& n)
{
    if (!data_)
        return false;

    a = const_cast<t_atom*>(&data_->atom());
    n = static_cast<int>(n_);
    return true;
}

bool AtomListView::operator==(const AtomList& l) const
{
    if (n_ != l.size())
        return false;

    if (l.atoms_.data() == data_)
        return true;

    for (size_t i = 0; i < n_; i++) {
        if (data_[i] != l[i])
            return false;
    }

    return true;
}

bool AtomListView::operator==(t_float v) const
{
    return isFloat() && math::float_compare(asFloat(), v);
}

bool AtomListView::operator==(const AtomListView& v) const
{
    if (v.n_ != n_)
        return false;

    if (v.data_ == data_)
        return true;

    if (data_ == nullptr)
        return true;

    for (size_t i = 0; i < n_; i++) {
        if (data_[i] != v.data_[i])
            return false;
    }

    return true;
}

bool AtomListView::isBool() const
{
    return (isFloat() && (asFloat() == 1 || asFloat() == 0))
        || ((isSymbol() && (asSymbol() == gensym("true") || asSymbol() == gensym("false"))));
}

bool AtomListView::isInteger() const
{
    return isFloat() && math::is_integer(asFloat());
}

bool AtomListView::isIntLessThen(t_int x) const noexcept
{
    return data_ && n_ == 1 && data_[0].isIntLessThen(x);
}

bool AtomListView::isIntLessEqual(t_int x) const noexcept
{
    return data_ && n_ == 1 && data_[0].isIntLessEqual(x);
}

bool AtomListView::isIntGreaterThen(t_int x) const noexcept
{
    return data_ && n_ == 1 && data_[0].isIntGreaterThen(x);
}

bool AtomListView::isIntGreaterEqual(t_int x) const noexcept
{
    return data_ && n_ == 1 && data_[0].isIntGreaterEqual(x);
}

bool AtomListView::isIntInClosedInterval(t_int min, t_int max) const noexcept
{
    return data_ && n_ == 1 && data_[0].isIntInClosedInterval(min, max);
}

const Atom& AtomListView::relativeAt(long pos) const
{
    return at(relativeIndex(pos, n_));
}

bool AtomListView::boolAt(size_t pos, bool def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asBool(def);
}

t_float AtomListView::floatAt(size_t pos, t_float def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asFloat(def);
}

int AtomListView::intAt(size_t pos, int def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asInt(def);
}

t_symbol* AtomListView::symbolAt(size_t pos, t_symbol* def) const
{
    if (pos >= n_)
        return def;
    else if (at(pos).isSymbol())
        return at(pos).asSymbol();
    else
        return def;
}

t_float AtomListView::floatGreaterThenAt(size_t pos, t_float min, t_float def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asFloatGreaterThen(min, def);
}

t_float AtomListView::floatGreaterEqualAt(size_t pos, t_float min, t_float def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asFloatGreaterEqual(min, def);
}

t_float AtomListView::floatLessThenAt(size_t pos, t_float max, t_float def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asFloatLessThen(max, def);
}

t_float AtomListView::floatLessEqualAt(size_t pos, t_float max, t_float def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asFloatLessEqual(max, def);
}

t_int AtomListView::intGreaterThenAt(size_t pos, t_int min, t_int def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asIntGreaterThen(min, def);
}

t_int AtomListView::intGreaterEqualAt(size_t pos, t_int min, t_int def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asIntGreaterEqual(min, def);
}

t_int AtomListView::intLessThenAt(size_t pos, t_int max, t_int def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asIntLessThen(max, def);
}

t_int AtomListView::intLessEqualAt(size_t pos, t_int max, t_int def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asIntLessEqual(max, def);
}

t_int AtomListView::intInClosedIntervalAt(size_t pos, t_int min, t_int max, t_int def) const
{
    if (pos >= n_)
        return def;
    else
        return at(pos).asIntInClosedInterval(min, max, def);
}

AtomListView AtomListView::subView(size_t from) const
{
    return subView(from, n_);
}

AtomListView AtomListView::subView(size_t from, size_t len) const
{
    if (!data_)
        return {};

    if (from >= n_)
        return {};

    return AtomListView(&data_[from].atom(), std::min(n_ - from, len));
}

AtomListView AtomListView::arguments(size_t from) const
{
    if (!data_)
        return {};

    if (from >= n_)
        return {};

    for (size_t i = 0; i < n_; i++) {
        auto& a = data_[i];
        if (a.isProperty())
            return (i < from) ? AtomListView {} : AtomListView(&data_[from].atom(), i - from);
    }

    return AtomListView(&data_[from].atom(), n_ - from);
}

AtomListView AtomListView::properties() const
{
    if(!data_)
        return {};

    for (size_t i = 0; i < n_; i++) {
        auto& a = data_[i];
        if (a.isProperty())
            return AtomListView(&data_[i].atom(), n_ - i);
    }

    return {};
}

bool AtomListView::contains(const Atom& a) const
{
    if (empty() || isNull())
        return false;

    return std::find(begin(), end(), a) != end();
}

bool AtomListView::allOf(const AtomPredicate& pred) const
{
    return std::all_of(begin(), end(), pred);
}

bool AtomListView::allFloatsOf(const FloatPredicate& pred) const
{
    return std::all_of(begin(), end(), [pred](const Atom& a) {
        return a.checkFloat(pred);
    });
}

bool AtomListView::allSymbolsOf(const SymbolPredicate& pred) const
{
    return std::all_of(begin(), end(), [pred](const Atom& a) {
        return a.checkSymbol(pred);
    });
}

bool AtomListView::anyOf(const AtomPredicate& pred) const
{
    return std::any_of(begin(), end(), pred);
}

bool AtomListView::noneOf(const AtomPredicate& pred) const
{
    return std::none_of(begin(), end(), pred);
}

MaybeFloat AtomListView::sum() const noexcept
{
    return reduceFloat(0, [](t_float a, t_float b) { return a + b; });
}

MaybeFloat AtomListView::product() const noexcept
{
    return reduceFloat(1, [](t_float a, t_float b) { return a * b; });
}

MaybeFloat AtomListView::reduceFloat(t_float init, std::function<t_float(t_float, t_float)> fn) const
{
    t_float accum = init;
    size_t n = 0;

    for (size_t i = 0; i < n_; i++) {
        auto& el = data_[i];
        if (el.isFloat()) {
            accum = fn(accum, el.asFloat());
            n++;
        }
    }

    return (n > 0) ? MaybeFloat(accum) : boost::none;
}

const Atom* AtomListView::relativeAt(int pos) const
{
    auto idx = relativeIndex<int>(pos, n_);
    if (idx < 0)
        return nullptr;

    return data_ + static_cast<size_t>(idx);
}

const Atom* AtomListView::clipAt(int pos) const
{
    if (n_ == 0)
        return nullptr;

    auto idx = static_cast<size_t>(clip<long>(pos, 0, long(n_ - 1)));
    return data_ + idx;
}

const Atom* AtomListView::wrapAt(int pos) const
{
    if (n_ == 0)
        return nullptr;

    return data_ + wrapInteger<long>(pos, n_);
}

const Atom* AtomListView::foldAt(int pos) const
{
    if (n_ == 0)
        return nullptr;

    return data_ + foldInteger<long>(pos, n_);
}

bool AtomListView::range(Atom& min, Atom& max) const noexcept
{
    if (!n_ || !data_)
        return false;

    auto res = std::minmax_element(begin(), end());
    min = *res.first;
    max = *res.second;
    return true;
}

const Atom* AtomListView::min() const
{
    auto it = std::min_element(begin(), end());
    if (it == end())
        return nullptr;
    else
        return &(*it);
}

const Atom* AtomListView::max() const
{
    auto it = std::max_element(begin(), end());
    if (it == end())
        return nullptr;
    else
        return &(*it);
}

size_t AtomListView::count(const Atom& a) const noexcept
{
    return static_cast<size_t>(std::count(begin(), end(), a));
}

size_t AtomListView::count(AtomPredicate pred) const noexcept
{
    return static_cast<size_t>(std::count_if(begin(), end(), pred));
}

bool AtomListView::getProperty(t_symbol* name, AtomListView& res) const
{
    constexpr size_t NPOS = std::numeric_limits<size_t>::max();
    size_t pos = NPOS;
    for (size_t i = 0; i < n_; i++) {
        if (data_[i] == name) {
            pos = i;
            break;
        }
    }

    if (pos == NPOS)
        return false;

    for (size_t i = pos + 1; i < n_; i++) {
        if (data_[i].isProperty()) {
            res = subView(pos + 1, i - (pos + 1));
            return true;
        }
    }

    res = subView(pos + 1);
    return true;
}

bool AtomListView::expandDollarArgs(const t_canvas* cnv, AtomList& res, InvalidDollarArgPolicy policy, const Atom& def) const
{
    res.reserve(res.size() + n_);

    for (size_t i = 0; i < n_; i++) {
        auto atom = data_[i].expandDollarArgs(cnv, true);

        switch (policy) {
        case InvalidDollarArgPolicy::DEFAULT_ARG:
            res.append(atom ? *atom : def);
            break;
        case InvalidDollarArgPolicy::KEEP_RAW:
            res.append(atom ? *atom : data_[i]);
            break;
        case InvalidDollarArgPolicy::DROP_ARG:
        default:
            break;
        }
    }

    return true;
}

void AtomListView::restorePrimitives(AtomList& res) const
{
    if (empty())
        return;

    res.reserve(res.size() + n_);
    auto bb = binbuf_new();
    if (!bb)
        return;

    binbuf_restore(bb, n_, toPdData());

    for (int i = 0; i < binbuf_getnatom(bb); i++)
        res.append(binbuf_getvec(bb)[i]);

    binbuf_free(bb);
}

void AtomListView::split(const Atom& sep, const std::function<void(const AtomListView&)>& msg) const
{
    if (!msg)
        return;

    size_t start = 0;

    for (size_t i = 0; i < n_; i++) {
        if (data_[i] == sep) {
            msg(subView(start, i - start));
            start = i + 1;
        }
    }

    msg(subView(start));
}

std::ostream& operator<<(std::ostream& os, const AtomListView& l)
{
    os << "( ";
    for (size_t i = 0; i < l.size(); i++) {
        if (i != 0)
            os << " ";

        os << l.at(i);
    }

    os << " )";
    return os;
}

}
