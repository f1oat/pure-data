/*****************************************************************************
 * Copyright 2020 Serge Poltavsky. All rights reserved.
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
#include "math_cabs_tilde.h"
#include "ceammc_factory.h"

#include <cmath>
#include <complex>

MathComplexAbsTilde::MathComplexAbsTilde(const PdArgs& args)
    : SoundExternal(args)
{
    createSignalInlet();
    createSignalOutlet();
}

void MathComplexAbsTilde::processBlock(const t_sample** in, t_sample** out)
{
    const size_t BS = blockSize();

    for (size_t i = 0; i < BS; i++) {
        out[0][i] = std::abs(std::complex<t_sample>(in[0][i], in[1][i]));
    }
}

void setup_math_cabs_tilde()
{
    SoundExternalFactory<MathComplexAbsTilde> obj("math.cabs~");
    obj.addAlias("cabs~");
}
