/*****************************************************************************
 * Copyright 2016 Serge Poltavski. All rights reserved.
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
#include "ceammc_atom.h"
#include "ceammc_datastorage.h"
#include "ceammc_log.h"
#include "ceammc_string.h"
#include "fmt/format.h"

#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>

namespace ceammc {

#define REF_PTR a_w.w_symbol
struct t_ref {
    AbstractData* data;
    uint32_t counter;
};

#define TRACE_DATA 1
#ifdef TRACE_DATA
#define TRACE(fn)                     \
    {                                 \
        std::cerr << fn << std::endl; \
    }
#else
#define TRACE(fn)
#endif

// safe-check
static_assert(sizeof(Atom) == sizeof(t_atom), "Atom and t_atom size mismatch");

constexpr t_atomtype TYPE_DATA = static_cast<t_atomtype>(A_CANT + 1);

Atom::Atom() noexcept
{
    a_type = A_NULL;
}

Atom::Atom(t_float v) noexcept
{
    a_type = A_FLOAT;
    a_w.w_float = v;
}

Atom::Atom(t_symbol* s) noexcept
{
    a_type = A_SYMBOL;
    a_w.w_symbol = s;
}

Atom::Atom(const t_atom& a)
    : t_atom(a)
{
    if (a_type == A_DEFFLOAT)
        a_type = A_FLOAT;
    else if (a_type == A_DEFSYMBOL)
        a_type = A_SYMBOL;
    else if (a_type == TYPE_DATA) {
        auto ref = reinterpret_cast<t_ref*>(REF_PTR);
        if (ref) {
            ref->counter++;
        } else {
            LIB_ERR << "nullref dataatom: " << __FUNCTION__;
            setNull();
        }
    }
}

Atom::Atom(AbstractData* d)
{
    if (d == nullptr) {
        LIB_ERR << "attempt to create NULL dataatom: " << __FUNCTION__;
        setNull();
        return;
    } else {
        a_type = TYPE_DATA;

        try {
            REF_PTR = reinterpret_cast<decltype(REF_PTR)>(new t_ref { d, 1 });

            TRACE(fmt::format("+ data {}", (void*)d));
        } catch (std::exception& e) { // for std::bad_alloc
            delete d;
            setNull();
        }
    }
}

Atom::Atom(const Atom& x)
{
    if (x.a_type == TYPE_DATA) {
        a_type = TYPE_DATA;
        a_w = x.a_w;
        REF_PTR = x.REF_PTR;

        auto ref = reinterpret_cast<t_ref*>(REF_PTR);
        if (ref) {
            ref->counter++;
        } else {
            LIB_ERR << "invalid null-ref dataatom: " << __FUNCTION__;
            setNull();
        }
    } else {
        a_type = x.a_type;
        a_w = x.a_w;
    }
}

Atom& Atom::operator=(const Atom& x)
{
    // self-assign check
    if (this == &x)
        return *this;

    if (x.a_type == TYPE_DATA) {
        // release previous
        if (a_type == TYPE_DATA) {
            auto ref = reinterpret_cast<t_ref*>(REF_PTR);
            if (ref != nullptr) {
                release();
            } else {
                LIB_ERR << "nullref dataatom: " << __FUNCTION__;
                setNull();
            }
        }

        // copy new datadata
        a_type = TYPE_DATA;
        // update ref
        a_w = x.a_w;

        // update reference counter
        acquire();

    } else {
        a_type = x.a_type;
        a_w = x.a_w;
    }

    return *this;
}

Atom::Atom(Atom&& x) noexcept
    : t_atom(x)
{
    if (x.a_type == TYPE_DATA) {
        a_type = TYPE_DATA;
        REF_PTR = x.REF_PTR;

        x.a_type = A_NULL;
        x.REF_PTR = nullptr;
    } else {
        x.a_type = A_NULL;
    }
}

Atom& Atom::operator=(Atom&& x) noexcept
{
    // self-move check
    if (this == &x)
        return *this;

    if (x.a_type == TYPE_DATA) {
        // release previous
        if (a_type == TYPE_DATA) {
            auto ref = reinterpret_cast<t_ref*>(REF_PTR);
            if (ref != nullptr) {
                release();
            } else {
                LIB_ERR << "nullref dataatom: " << __FUNCTION__;
                setNull();
            }
        }

        a_type = std::move(x.a_type);
        a_w = std::move(x.a_w);

        x.a_type = A_NULL;
        x.REF_PTR = nullptr;
    } else {
        a_type = x.a_type;
        a_w = x.a_w;
        x.a_type = A_NULL;
    }

    return *this;
}

Atom::~Atom() noexcept
{
    if (a_type == TYPE_DATA)
        release();
}

int Atom::dataType() const noexcept
{
    if (a_type == TYPE_DATA) {
        auto ref = reinterpret_cast<t_ref*>(REF_PTR);
        if (ref) {
            if (ref->data) {
                return ref->data->type();
            } else {
                LIB_ERR << "nullptr dataatom: " << __FUNCTION__;
                return 0;
            }
        } else {
            LIB_ERR << "nullref dataatom: " << __FUNCTION__;
            return 0;
        }
    } else
        return 0;
}

bool Atom::isData() const noexcept
{
    return (a_type == TYPE_DATA)
        && (REF_PTR != nullptr);
}

int Atom::refCount() const noexcept
{
    if (a_type == TYPE_DATA) {
        auto ref = reinterpret_cast<t_ref*>(REF_PTR);
        if (ref)
            return ref->counter;
        else {
            LIB_ERR << "nullref dataatom: " << __FUNCTION__;
            return 0;
        }
    } else
        return 0;
}

Atom Atom::parseQuoted() const
{
    if (a_type != A_SYMBOL)
        return *this;

    std::string m;
    if (string::pd_string_parse(a_w.w_symbol->s_name, m))
        return gensym(m.c_str());
    else
        return *this;
}

bool Atom::isQuoted() const
{
    if (a_type != A_SYMBOL)
        return false;
    else
        return string::is_pd_string(a_w.w_symbol->s_name);
}

bool Atom::beginQuote() const
{
    if (a_type != A_SYMBOL)
        return false;

    return a_w.w_symbol->s_name[0] == '"';
}

bool Atom::endQuote() const
{
    if (a_type != A_SYMBOL)
        return false;

    return string::pd_string_end_quote(a_w.w_symbol->s_name);
}

bool Atom::is_data(const t_atom& a) noexcept
{
    return a.a_type == TYPE_DATA;
}

bool Atom::is_data(t_atom* a) noexcept
{
    return a && a->a_type == TYPE_DATA;
}

bool Atom::acquire() noexcept
{
    if (a_type == TYPE_DATA) {
        auto ref = reinterpret_cast<t_ref*>(REF_PTR);
        if (ref) {
            if (ref->data == nullptr) {
                LIB_ERR << "dataatom with NULL data pointer: " << __FUNCTION__;
                return false;
            } else {
                ref->counter++;
                return true;
            }
        } else {
            LIB_ERR << "nullref dataatom: " << __FUNCTION__;
            return false;
        }
    } else {
        LIB_ERR << "attempt to acquire non dataatom: " << __FUNCTION__;
        return false;
    }
}

bool Atom::release() noexcept
try {
    if (a_type == TYPE_DATA) {
        auto ref = reinterpret_cast<t_ref*>(REF_PTR);
        if (ref) {
            if (ref->counter > 0) {
                ref->counter--;
                // delete data
                if (ref->counter == 0) {
                    if (ref->data) {
                        delete ref->data;

                        TRACE(fmt::format("- data {}", (void*)ref->data));

                        ref->data = nullptr;
                    } else {
                        LIB_ERR << "nullptr dataatom: " << __FUNCTION__;
                    }

                    delete ref;
                    REF_PTR = nullptr;
                }
            } else {
                LIB_ERR << "zero reference counter: " << __FUNCTION__;
            }
        } else {
            LIB_ERR << "nullref dataatom: " << __FUNCTION__;
        }
    } else
        LIB_ERR << "attempt to release non-data atom: " << __FUNCTION__;

    return false;
} catch (std::exception& e) {
    std::cerr << e.what();
    return false;
}

void Atom::setNull()
{
    a_type = A_NULL;
    REF_PTR = nullptr;
}

bool Atom::isBool() const noexcept
{
    static t_symbol* SYM_TRUE = gensym("true");
    static t_symbol* SYM_FALSE = gensym("false");

    switch (a_type) {
    case A_FLOAT: {
        return std::equal_to<t_float>()(a_w.w_float, 1)
            || std::equal_to<t_float>()(a_w.w_float, 0);
    }
    case A_SYMBOL: {
        t_symbol* s = a_w.w_symbol;
        return (s == SYM_TRUE) || (s == SYM_FALSE);
    };
    default:
        return false;
    }
}

bool Atom::isInteger() const noexcept
{
    return isFloat()
        && std::equal_to<t_float>()(std::ceil(a_w.w_float), a_w.w_float);
}

Atom::Type Atom::type() const noexcept
{
    switch (a_type) {
    case A_SYMBOL:
        if (a_w.w_symbol == 0)
            return NONE;

        return (a_w.w_symbol->s_name[0] == PROP_PREFIX) ? PROPERTY : SYMBOL;
    case A_FLOAT:
        return FLOAT;
    case TYPE_DATA:
        return DATA;
    default:
        return NONE;
    }
}

bool Atom::getFloat(t_float* v) const noexcept
{
    if (!v)
        return false;

    if (!isFloat())
        return false;

    *v = this->a_w.w_float;
    return true;
}

bool Atom::getSymbol(t_symbol** s) const noexcept
{
    if (!s)
        return false;

    if (!isSymbol())
        return false;

    *s = this->a_w.w_symbol;
    return true;
}

bool Atom::setFloat(t_float v, bool force) noexcept
{
    if (!force && !isFloat())
        return false;

    release();

    SETFLOAT(this, v);
    return true;
}

bool Atom::setSymbol(t_symbol* s, bool force) noexcept
{
    if (!force && !isSymbol())
        return false;

    release();

    SETSYMBOL(this, s);
    return true;
}

bool Atom::asBool(bool def) const noexcept
{
    static t_symbol* SYM_TRUE = gensym("true");
    static t_symbol* SYM_FALSE = gensym("false");

    switch (a_type) {
    case A_DEFFLOAT:
    case A_FLOAT:
        return !std::equal_to<t_float>()(a_w.w_float, 0);
    case A_SYMBOL:
        if (a_w.w_symbol == SYM_TRUE)
            return true;
        else if (a_w.w_symbol == SYM_FALSE)
            return false;
        else
            return def;
    default:
        return def;
    }
}

size_t Atom::asSizeT(size_t def) const noexcept
{
    if (!isFloat())
        return def;

    t_float v = a_w.w_float;
    return (v < 0) ? def : static_cast<size_t>(v);
}

const AbstractData* Atom::asData() const noexcept
{
    if (a_type == TYPE_DATA) {
        auto ref = reinterpret_cast<t_ref*>(REF_PTR);
        if (ref) {
            if (ref->data) {
                return ref->data;
            } else {
                LIB_ERR << "nullptr dataatom: " << __FUNCTION__;
                return nullptr;
            }
        } else {
            LIB_ERR << "nullref dataatom: " << __FUNCTION__;
            return nullptr;
        }
    } else
        return nullptr;
}

bool Atom::operator<(const Atom& b) const noexcept
{
    if (this == &b)
        return false;

    const auto t = type();

    if (t == b.type()) {
        // same logical type from here
        switch (t) {
        case FLOAT:
            return a_w.w_float < b.a_w.w_float;
        case SYMBOL:
        case PROPERTY: {
            if (a_w.w_symbol == b.a_w.w_symbol)
                return false;

            if (a_w.w_symbol == 0 || b.a_w.w_symbol == 0)
                return false;

            if (a_w.w_symbol->s_name == 0 || b.a_w.w_symbol->s_name == 0)
                return false;

            return strcmp(a_w.w_symbol->s_name, b.a_w.w_symbol->s_name) < 0;
        }
        case NONE:
            return false;
        case DATA: {
            if (dataType() == b.dataType()) {
                if (asData() == b.asData())
                    return false;
                else if (asData() != nullptr) {

                    static_assert(noexcept(asData()->isLess(b.asData())), "isLess should be noexcept");

                    return asData()->isLess(b.asData());
                } else
                    return false;
            } else
                return dataType() < b.dataType();
        }
        }
    } else
        return t < b.type();
}

Atom& Atom::operator+=(t_float v) noexcept
{
    if (isFloat())
        a_w.w_float += v;

    return *this;
}

Atom& Atom::operator-=(t_float v) noexcept
{
    if (isFloat())
        a_w.w_float -= v;

    return *this;
}

Atom& Atom::operator*=(t_float v) noexcept
{
    if (isFloat())
        a_w.w_float *= v;

    return *this;
}

Atom& Atom::operator/=(t_float v) noexcept
{
    if (isFloat() && v != 0.0)
        a_w.w_float /= v;

    return *this;
}

Atom Atom::operator+(t_float v) const
{
    return Atom(*this) += v;
}

Atom Atom::operator-(t_float v) const
{
    return Atom(*this) -= v;
}

Atom Atom::operator*(t_float v) const
{
    return Atom(*this) *= v;
}

Atom Atom::operator/(t_float v) const
{
    return Atom(*this) /= v;
}

bool Atom::operator==(const Atom& x) const noexcept
{
    if (this == &x)
        return true;

    // compare logical types
    auto t = type();
    if (t != x.type())
        return false;

    // same logical types here
    switch (t) {
    case FLOAT:
        return std::equal_to<t_float>()(a_w.w_float, x.a_w.w_float);
    case PROPERTY:
    case SYMBOL:
        return a_w.w_symbol == x.a_w.w_symbol;
    case DATA: {
        if (dataType() != x.dataType())
            return false;
        else if (asData() == x.asData())
            return true;
        else if (asData() != nullptr) {

            static_assert(noexcept(asData()->isEqual(x.asData())), "isEqual should be noexcept");

            return asData()->isEqual(x.asData());
        } else
            return false;
    }
    case NONE:
        return true;
    }
}

std::ostream& operator<<(std::ostream& os, const Atom& a)
{
    if (a.isFloat())
        os << a.asFloat();
    else if (a.isSymbol())
        os << a.asSymbol()->s_name;
    else if (a.isNone())
        os << "NONE";
    else if (a.isData()) {
        auto dptr = a.asData();

        if (dptr) {
            auto name = DataStorage::instance().nameByType(dptr->type());

            os << fmt::format(
                "{}(type={},id=0x{}) {}",
                (name.empty()) ? "Data???" : name,
                a.dataType(),
                (void*)dptr, dptr->toString());
        } else {
            os << "NULL data pointer";
        }
    } else
        os << "???";

    return os;
}
}
