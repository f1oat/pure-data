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
#ifndef CEAMMC_CONTAINERS_H
#define CEAMMC_CONTAINERS_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <initializer_list>

#include <boost/container/small_vector.hpp>
#include <boost/container/static_vector.hpp>
#include <boost/iterator/filter_iterator.hpp>

#include "ceammc_atom.h"
#include "ceammc_atomlist_view.h"

namespace ceammc {

template <size_t N = 4>
class SmallAtomListN : public boost::container::small_vector<Atom, N> {
public:
    SmallAtomListN() { }

    SmallAtomListN(std::initializer_list<Atom> atoms)
        : boost::container::small_vector<Atom, N>(atoms.begin(), atoms.end())
    {
    }

    explicit SmallAtomListN(const AtomListView& lv)
        : boost::container::small_vector<Atom, N>(lv.begin(), lv.end())
    {
    }

    template <typename... Args>
    explicit SmallAtomListN(Args... args)
        : boost::container::small_vector<Atom, N>({ atomFrom(args)... })
    {
    }

    AtomListView view() const
    {
        if (this->empty())
            return {};
        else
            return { this->data(), this->size() };
    }

    template <typename It>
    void insert_back(It begin, It end)
    {
        this->insert(this->end(), begin, end);
    }

    template <typename Cont>
    void insert_back(const Cont& c)
    {
        this->insert(this->end(), std::begin(c), std::end(c));
    }
};

using SmallAtomList = SmallAtomListN<8>;
using AtomList16 = SmallAtomListN<16>;
using AtomList32 = SmallAtomListN<32>;
using AtomList64 = SmallAtomListN<64>;
using AtomList128 = SmallAtomListN<128>;
using AtomList256 = SmallAtomListN<256>;
using AtomList512 = SmallAtomListN<512>;

template <size_t MAX_SIZE>
class StaticAtomList : public boost::container::static_vector<Atom, MAX_SIZE> {
public:
    StaticAtomList() { }

    StaticAtomList(std::initializer_list<Atom> atoms)
        : boost::container::static_vector<Atom, MAX_SIZE>(atoms.begin(), atoms.end())
    {
    }

    template <typename... Args>
    explicit StaticAtomList(Args... args)
        : boost::container::static_vector<Atom, MAX_SIZE>({ atomFrom(args)... })
    {
    }

    AtomListView view() const
    {
        if (this->empty())
            return {};
        else
            return { this->data(), this->size() };
    }
};

template <size_t SIZE>
class AtomArray : public std::array<Atom, SIZE> {
public:
    AtomArray() { this->fill(Atom(0.)); }

    template <typename... Args>
    explicit AtomArray(Args... args)
        : std::array<Atom, SIZE>({ atomFrom(args)... })
    {
        constexpr auto NARGS = sizeof...(Args);
        static_assert(NARGS == SIZE, "");
    }

    AtomListView view() const
    {
        if (this->empty())
            return {};
        else
            return { this->data(), SIZE };
    }
};

template <size_t N = 4>
class SmallMessageN {
public:
    enum Type {
        BANG,
        FLOAT,
        SYMBOL,
        POINTER,
        LIST,
        ANY
    };

    SmallMessageN()
        : type_(BANG)
    {
    }

    SmallMessageN(t_float f)
        : type_(FLOAT)
    {
        atoms_.push_back(f);
    }

    SmallMessageN(t_symbol* s)
        : type_(SYMBOL)
    {
        atoms_.push_back(s);
    }

    SmallMessageN(const Atom& a)
    {
        switch (a.type()) {
        case Atom::FLOAT:
            type_ = FLOAT;
            break;
        case Atom::PROPERTY:
        case Atom::SYMBOL:
            type_ = SYMBOL;
            break;
        case Atom::POINTER:
            type_ = POINTER;
            break;
        case Atom::DATA:
        default:
            type_ = FLOAT;
            atoms_.push_back(0.f);
            return;
            break;
        }

        atoms_.push_back(a);
    }

    SmallMessageN(const AtomListView& lv)
        : type_(LIST)
    {
        atoms_.reserve(lv.size());
        std::copy(lv.begin(), lv.end(), std::back_inserter(atoms_));
    }

    SmallMessageN(t_symbol* s, const AtomListView& lv)
        : type_(ANY)
    {
        atoms_.reserve(lv.size() + 1);
        atoms_.push_back(s);
        std::copy(lv.begin(), lv.end(), std::back_inserter(atoms_));
    }

    Type type() const { return type_; }
    size_t size() const { return atoms_.size(); }

    AtomListView view() const { return AtomListView(&atoms_[0].atom(), atoms_.size()); }

public:
    void outputTo(t_outlet* x) const
    {
        switch (type_) {
        case FLOAT:
            outlet_float(x, atoms_[0].template asT<t_float>());
            break;
        case SYMBOL:
            outlet_symbol(x, atoms_[0].template asT<t_symbol*>());
            break;
        case POINTER:
            outlet_pointer(x, atoms_[0].atom().a_w.w_gpointer);
            break;
        case LIST:
            outlet_list(x, &s_list, atoms_.size(), const_cast<t_atom*>(&atoms_[0].atom()));
            break;
        case ANY:
            outlet_anything(x, atoms_[0].template asT<t_symbol*>(), atoms_.size() - 1, const_cast<t_atom*>(&atoms_[1].atom()));
            break;
        case BANG:
        default:
            outlet_bang(x);
            break;
        }
    }

private:
    SmallAtomListN<N> atoms_;
    Type type_;
};

template <typename T>
class SingletonMeyers : public T {
public:
    static SingletonMeyers<T>& instance()
    {
        static SingletonMeyers<T> instance_;
        return instance_;
    }

    SingletonMeyers(const SingletonMeyers&) = delete;
    SingletonMeyers(SingletonMeyers&&) = delete;
    SingletonMeyers& operator=(const SingletonMeyers&) = delete;
    SingletonMeyers& operator=(SingletonMeyers&&) = delete;

protected:
    SingletonMeyers() { }
};

template <typename Cont>
inline boost::filter_iterator<AtomPredicate, typename Cont::iterator> atom_filter_it_begin(const Cont& c, AtomPredicate pred)
{
    return boost::filter_iterator<AtomPredicate, typename Cont::const_iterator>(pred, std::begin(c), std::end(c));
}

template <typename Cont>
inline boost::filter_iterator<AtomPredicate, typename Cont::iterator> atom_filter_it_end(const Cont& c)
{
    return boost::filter_iterator<AtomPredicate, typename Cont::const_iterator>(nullptr, std::end(c), std::end(c));
}

template <typename Cont>
inline boost::filter_iterator<AtomPredicate, typename Cont::iterator> atom_filter_it_begin(Cont& c, AtomPredicate pred)
{
    return boost::filter_iterator<AtomPredicate, typename Cont::iterator>(pred, std::begin(c), std::end(c));
}

template <typename Cont>
inline boost::filter_iterator<AtomPredicate, typename Cont::iterator> atom_filter_it_end(Cont& c)
{
    return boost::filter_iterator<AtomPredicate, typename Cont::iterator>(nullptr, std::end(c), std::end(c));
}

}

#endif // CEAMMC_CONTAINERS_H
