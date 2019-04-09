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
#include "fx_freeverb.h"
#include "ceammc_factory.h"

#include "ceammc_factory.h"

using namespace ceammc;

class FxFreeverb : public faust_fx_freeverb_tilde {
public:
    FxFreeverb(const PdArgs& args)
        : faust_fx_freeverb_tilde(args)
    {
        static t_symbol* SYM_PROP_ROOM = gensym("@room");
        static t_symbol* SYM_PROP_DRYWET = gensym("@drywet");

        bindPositionalArgsToProps({ SYM_PROP_ROOM, SYM_PROP_DRYWET });
    }

    void m_reset(t_symbol*, const AtomList&)
    {
        dsp_->instanceClear();
    }
};

void setup_fx_freeverb_tilde()
{
    SoundExternalFactory<FxFreeverb> obj("fx.freeverb~");
    obj.addMethod("reset", &FxFreeverb::m_reset);
}
