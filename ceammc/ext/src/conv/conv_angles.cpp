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
#include "conv_angles.h"
#include "ceammc_convert.h"
#include "ceammc_factory.h"

Phase2Rad::Phase2Rad(const PdArgs& args)
    : SimpleConverter(args, convert::phase2rad<t_float>)
{
}

Rad2Phase::Rad2Phase(const PdArgs& args)
    : SimpleConverter(args, convert::rad2phase<t_float>)
{
}

Phase2RadTilde::Phase2RadTilde(const PdArgs& args)
    : SimpleConverterTilde(args, convert::phase2rad<t_float>)
{
}

Rad2PhaseTilde::Rad2PhaseTilde(const PdArgs& args)
    : SimpleConverterTilde(args, convert::rad2phase<t_float>)
{
}

Car2Pol::Car2Pol(const PdArgs& args)
    : BaseObject(args)
    , positive_range_(nullptr)
{
    positive_range_ = new BoolProperty("@positive", false);
    addProperty(positive_range_);

    createOutlet();
}

void Car2Pol::onList(const AtomList& lst)
{
    static const t_float m_pi = std::acos(t_float(-1));

    if (!checkArgs(lst, ARG_FLOAT, ARG_FLOAT)) {
        OBJ_ERR << "X Y coordinates expected: " << lst;
        return;
    }

    const auto x = lst[0].asFloat();
    const auto y = lst[1].asFloat();

    if (true)
        listTo(0, { std::hypot(x, y), wrapFloatMinMax<t_float>(std::atan2(y, x), 0, 2 * m_pi) });
    else
        listTo(0, { std::hypot(x, y), std::atan2(y, x) });
}

Pol2Car::Pol2Car(const PdArgs& args)
    : BaseObject(args)
{
    createOutlet();
}

void Pol2Car::onList(const AtomList& lst)
{
    if (!checkArgs(lst, ARG_FLOAT, ARG_FLOAT)) {
        OBJ_ERR << "R THETA coordinates expected: " << lst;
        return;
    }

    const auto r = lst[0].asFloat();
    const auto theta = lst[1].asFloat();

    listTo(0, { r * std::cos(theta), r * std::sin(theta) });
}

void setup_conv_angles()
{
    ObjectFactory<Phase2Rad> p2r("conv.phase2rad");
    p2r.addAlias("phase->rad");

    ObjectFactory<Rad2Phase> r2p("conv.rad2phase");
    r2p.addAlias("rad->phase");

    ObjectFactory<Pol2Car> pol2car("conv.pol2car");
    pol2car.addAlias("pol->car");

    ObjectFactory<Car2Pol> car2pol("conv.car2pol");
    car2pol.addAlias("car->pol");

    SoundExternalFactory<Phase2RadTilde> p2rt("conv.phase2rad~");
    p2rt.addAlias("phase->rad~");

    SoundExternalFactory<Rad2PhaseTilde> r2pt("conv.rad2phase~");
    r2pt.addAlias("rad->phase~");
}
