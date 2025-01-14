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
#ifndef ARRAY_BASE_H
#define ARRAY_BASE_H

#include "ceammc_array.h"
#include "ceammc_object.h"
#include "ceammc_signal.h"
#include "ceammc_sound_external.h"

using namespace ceammc;

class ArrayPositionProperty : public Property {
    t_sample v_;
    Array* array_;

public:
    ArrayPositionProperty(Array* arr, const std::string& name, t_float value, PropValueAccess access = PropValueAccess::READWRITE);

    AtomList get() const override { return { (t_float)v_ }; }
    bool setList(const AtomListView& lv) override;

    bool setFloat(t_float v) override;
    bool setInt(t_int v) override;
    bool getFloat(t_float& v) const override;

    t_sample value() { return v_; }
    void setValue(t_sample v) { v_ = v; }

    /**
     * return absolute array position in [0..N) range, where N - array size
     */
    t_float samples() const;

    /**
     * return absolute array position in seconds
     * @param sr
     */
    t_float seconds(t_float sr) const { return samples() / ((sr == 0) ? 1 : sr); }

    /**
     * return absolute array position in milliseconds
     * @param sr
     */
    t_sample ms(t_float sr) const { return seconds(sr) * 1000; }

    /**
     * return array phase position in [0..1) range
     */
    t_sample phase() const;

    /**
     * set array position in samples
     * @param pos - relative position (values <0 - offset from the end of the array)
     * @param check - if true do a range check
     * @return - if check is false, always return true, if check is true returns result of range check
     */
    bool setSamples(t_float pos, bool check = true);
    bool setSeconds(t_float pos, t_float sr, bool check = true);
    bool setMs(t_float pos, t_float sr, bool check = true);
    bool setPhase(t_float v);

    bool setBegin()
    {
        v_ = 0;
        return true;
    }

    bool setEnd()
    {
        v_ = -1;
        return true;
    }

public:
    using value_type = t_float;
};

template <class Base>
class ArrayReadIFace : public Base {
    t_symbol* array_name_;

public:
    ArrayReadIFace(const PdArgs& a);

    /**
     * @brief set new array and tries to open it
     * @param s - array name
     * @return true on success, false on error
     */
    virtual bool setArray(t_symbol* s);

    /**
     * @brief updates array information (if array exists, updates size info)
     * @note should be called before any read/write operation
     *       if array could be modified or deleted previously
     * @param log - log on error to Pd Window
     * @return true if array exists and valid, otherwise false
     */
    bool checkArray(bool log = true);

    // range check required
    t_sample readUnsafe0(double fpos) const { return array_.readUnsafe0(fpos); }
    t_sample readUnsafe1(double fpos) const { return array_.readUnsafe1(fpos); }
    t_sample readUnsafe3(double fpos) const { return array_.readUnsafe3(fpos); }

    // range check not required
    // on invalid position returns 0
    t_sample readSafe0(double fpos) const { return array_.readSafe0(fpos); }
    t_sample readSafe1(double fpos) const { return array_.readSafe1(fpos); }
    t_sample readSafe3(double fpos) const { return array_.readSafe3(fpos); }

protected:
    Array array_;
};

using ArrayBase = ArrayReadIFace<BaseObject>;

class ArraySoundBase : public ArrayReadIFace<SoundExternal> {
public:
    ArraySoundBase(const PdArgs& args);
    bool setArray(t_symbol* aname) override;

    void setupDSP(t_signal** sp) override
    {
        if (!checkArray(true))
            return dsp_add_zero(sp[0]->s_vec, sp[0]->s_n);

        array_.useInDSP();
        SoundExternal::setupDSP(sp);
    }
};

class ArrayMod : public ArrayBase {
    BoolProperty* redraw_;

public:
    ArrayMod(const PdArgs& a);
    bool shouldRedraw() const;
};

template <class Base>
ArrayReadIFace<Base>::ArrayReadIFace(const PdArgs& a)
    : Base(a)
    , array_name_(&s_)
{
    Base::createCbSymbolProperty(
        "@array",
        [this]() -> t_symbol* { return array_name_; },
        [this](t_symbol* s) -> bool { setArray(s); return true; })
        ->setArgIndex(0);
}

template <class Base>
bool ArrayReadIFace<Base>::checkArray(bool log)
{
    if (array_name_ == &s_ || !array_.open(array_name_)) {
        if (log && array_name_ != &s_ && !Base::isPatchLoading())
            OBJ_ERR << "invalid array: '" << array_name_->s_name << '\'';

        return false;
    }

    return true;
}

template <class Base>
bool ArrayReadIFace<Base>::setArray(t_symbol* s)
{
    array_name_ = s;
    if (!array_.open(array_name_)) {
        if (!Base::isPatchLoading())
            OBJ_ERR << "array not found: '" << s->s_name << '\'';

        return false;
    }

    return true;
}

#endif // ARRAY_BASE_H
