/*****************************************************************************
 * Copyright 2022 Serge Poltavsky. All rights reserved.
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
#ifndef ARRAY_RISSET_RHYTHM_TILDE_H
#define ARRAY_RISSET_RHYTHM_TILDE_H

#include "array_base.h"
using namespace ceammc;

class ArrayRissetRhythmTilde : public ArraySoundBase {
    size_t T_;
    t_sample tl0_, tl1_, tl2_, tl3_, tl4_, te_;
    IntProperty* bandwidth_;
    IntProperty* length_;
    bool run_;

public:
    ArrayRissetRhythmTilde(const PdArgs& args);

    void onFloat(t_float f) override;
    void setupDSP(t_signal** sp) override;
    void processBlock(const t_sample** in, t_sample** out) override;
};

void setup_array_risset_rhythm_tilde();

#endif // ARRAY_RISSET_RHYTHM_TILDE_H
