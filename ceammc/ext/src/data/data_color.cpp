/*****************************************************************************
 * Copyright 2023 Serge Poltavski. All rights reserved.
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
#include "data_color.h"
#include "args/argcheck2.h"
#include "ceammc_factory.h"

DataColor::DataColor(const PdArgs& args)
    : BaseObject(args)
    , color_(nullptr)
{
    color_ = new DataPropertyT<DataTypeColor>("@value", DataTypeColor());
    color_->setArgIndex(0);
    addProperty(color_);

    createOutlet();
}

void DataColor::onBang()
{
    atomTo(0, color_->asDataAtom());
}

void DataColor::onDataT(const ColorAtom& a)
{
    color_->setValue(*a);
}

void DataColor::m_brighten(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("f[-1,1]");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().brighten(lv.floatAt(0, 0));
}

void DataColor::m_darken(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("f[-1,1]");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().darken(lv.floatAt(0, 0));
}

void DataColor::m_grayscale(t_symbol* s, const AtomListView&)
{
    color_->value().grayscale();
}

void DataColor::m_saturate(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("AMOUNT:f[-1,1]");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().saturate(lv.floatAt(0, 0));
}

void DataColor::m_desaturate(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("AMOUNT:f[-1,1]");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().desaturate(lv.floatAt(0, 0));
}

void DataColor::m_invert(t_symbol* s, const AtomListView&)
{
    color_->value().invert();
}

void DataColor::m_rotate(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("ANGLE:f[0,360]");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().rotate(lv.floatAt(0, 0));
}

void DataColor::m_flip(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("f[0,360]");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().flip(lv.floatAt(0, 0));
}

void DataColor::m_hex(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("HEX:s");
    if (!chk.check(lv, this))
        return chk.usage(this);

    if (!color_->value().setHex(lv[0].asSymbol()->s_name)) {
        OBJ_ERR << "can't parse color value";
        return;
    }
}

void DataColor::m_rgb8(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("R:i[0,255] G:i[0,255] B:i[0,255] A:i[0,255]?");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().setRgb8(lv.intAt(0, 0), lv.intAt(1, 0), lv.intAt(2, 0), lv.intAt(3, 255));
}

void DataColor::m_rgb(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("R:f[0,1] G:f[0,1] B:f[0,1] A:f[0,1]?");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().setRgbf(lv.floatAt(0, 0), lv.floatAt(1, 0), lv.floatAt(2, 0), lv.floatAt(3, 1));
}

void DataColor::m_hsl(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("H:f[0,360] S:f[0,1] L:f[0,1] A:f[0,1]?");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().setHsl(lv.floatAt(0, 0), lv.floatAt(1, 0), lv.floatAt(2, 0), lv.floatAt(3, 1));
}

void DataColor::m_hwb(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("H:f[0,360] W:f[0,1] B:f[0,1] A:f[0,1]?");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().setHwb(lv.floatAt(0, 0), lv.floatAt(1, 0), lv.floatAt(2, 0), lv.floatAt(3, 1));
}

void DataColor::m_oklab(t_symbol* s, const AtomListView& lv)
{
    static const args::ArgChecker chk("L:f[0,1] A:f[-1,1] B:f[-1,1] A:f[0,1]?");
    if (!chk.check(lv, this))
        return chk.usage(this);

    color_->value().setOkLab(lv.floatAt(0, 0), lv.floatAt(1, 0), lv.floatAt(2, 0), lv.floatAt(3, 1));
}

void setup_data_color()
{
    ObjectFactory<DataColor> obj("data.color");
    obj.addAlias("color");
    obj.processData<DataTypeColor>();

    obj.addMethod("brighten", &DataColor::m_brighten);
    obj.addMethod("darken", &DataColor::m_darken);
    obj.addMethod("grayscale", &DataColor::m_grayscale);
    obj.addMethod("saturate", &DataColor::m_saturate);
    obj.addMethod("desaturate", &DataColor::m_desaturate);
    obj.addMethod("rotate", &DataColor::m_rotate);
    obj.addMethod("invert", &DataColor::m_invert);
    obj.addMethod("flip", &DataColor::m_flip);

    obj.addMethod("hex", &DataColor::m_hex);
    obj.addMethod("rgb8", &DataColor::m_rgb8);
    obj.addMethod("rgb", &DataColor::m_rgb);
    obj.addMethod("hsl", &DataColor::m_hsl);
    obj.addMethod("hwb", &DataColor::m_hwb);
    obj.addMethod("oklab", &DataColor::m_oklab);
}
