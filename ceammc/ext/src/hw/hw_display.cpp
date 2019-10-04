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
#include "hw_display.h"
#include "ceammc_factory.h"

HwDisplay::HwDisplay(const PdArgs& args)
    : BaseObject(args)
{
    createOutlet();
    createCbProperty("@brightness", &HwDisplay::propBrightness, &HwDisplay::setPropBrightness)
        ->info()
        .setType(PropertyInfoType::FLOAT);
}

AtomList HwDisplay::propBrightness() const
{
#ifdef WITH_IODISPLAY
    float v = 0;
    if (!display_.getBrightness(&v))
        OBJ_ERR << "can't get brightness";

    return AtomList(v);
#endif

#ifdef WITH_X11DISPLAY
    float v = 0;
    if (!display_.getBrightness(&v))
        OBJ_ERR << "can't get brightness";

    return AtomList(v);
#endif

    return AtomList(-1);
}

void HwDisplay::setPropBrightness(const AtomList& v)
{
#ifdef WITH_IODISPLAY
    if (!checkArgs(v, ARG_FLOAT))
        return;

    if (!display_.setBrightness(v.floatAt(0, 0)))
        OBJ_ERR << "can't set brightness";
#endif

#ifdef WITH_X11DISPLAY
    if (!checkArgs(v, ARG_FLOAT))
        return;

    if (!display_.setBrightness(v.floatAt(0, 0)))
        OBJ_ERR << "can't set brightness";
#endif
}

void setup_hw_display()
{
    ObjectFactory<HwDisplay> obj("hw.display");
}
