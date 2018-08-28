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
#include "math_round_tilde.h"
#include "ceammc_factory.h"

#include <cmath>

MathRoundTilde::MathRoundTilde(const PdArgs& a)
    : SoundExternal(a)
{
    createSignalOutlet();
}

void MathRoundTilde::processBlock(const t_sample** in, t_sample** out)
{
    const size_t bs = blockSize();

    for (size_t i = 0; i < bs; i += 8) {
        out[0][i] = roundf(in[0][i]);
        out[0][i + 1] = roundf(in[0][i + 1]);
        out[0][i + 2] = roundf(in[0][i + 2]);
        out[0][i + 3] = roundf(in[0][i + 3]);
        out[0][i + 4] = roundf(in[0][i + 4]);
        out[0][i + 5] = roundf(in[0][i + 5]);
        out[0][i + 6] = roundf(in[0][i + 6]);
        out[0][i + 7] = roundf(in[0][i + 7]);
    }
}

void setup_math_round_tilde()
{
    SoundExternalFactory<MathRoundTilde> obj("math.round~");
    obj.addAlias("round~");
}
