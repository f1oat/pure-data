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
#include "fx_freeverb2.h"
#include "ceammc_factory.h"

#include "ceammc_factory.h"

using namespace ceammc;

static t_symbol* SYM_PROP_ROOM = gensym("@room");
static t_symbol* SYM_PROP_DRYWET = gensym("@drywet");

class FxFreeverb2 : public faust_fx_freeverb2_tilde {
public:
    FxFreeverb2(const PdArgs& args)
        : faust_fx_freeverb2_tilde(args)
    {
        bindPositionalArgsToProps({ SYM_PROP_ROOM, SYM_PROP_DRYWET });
    }

    void m_reset(t_symbol*, const AtomList&)
    {
        dsp_->instanceClear();
    }
};

void setup_fx_freeverb2_tilde()
{
    SoundExternalFactory<FxFreeverb2> obj("fx.freeverb2~");
    obj.addMethod("reset", &FxFreeverb2::m_reset);
}
