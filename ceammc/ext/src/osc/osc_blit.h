/*****************************************************************************
 * Copyright 2018 Serge Poltavsky. All rights reserved.
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
#ifndef OSC_BLIT_H
#define OSC_BLIT_H

#include "ceammc_sound_external.h"

using namespace ceammc;

namespace stk {
class Blit;
}

class OscBlit : public SoundExternal {
    stk::Blit* osc_;
    t_float freq_;
    size_t num_harmonics_;

public:
    OscBlit(const PdArgs& args);
    ~OscBlit() override;

    void processBlock(const t_sample** in, t_sample** out) override;
    void setupDSP(t_signal** sp) override;

    void onInlet(size_t, const AtomList&) override;

    AtomList propFreq() const;
    void propSetFreq(const AtomList& lst);
    AtomList propHarm() const;
    void propSetHarm(const AtomList& lst);
};

void setup_osc_blit();

#endif // OSC_BLIT_H
