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
#ifndef HOA_2D_PROJECTOR_H
#define HOA_2D_PROJECTOR_H

#include "hoa_common.h"

class Hoa2dProjector : public HoaBase<hoa::Hoa2d> {
    Buffer in_buf_;
    Buffer out_buf_;
    std::unique_ptr<DecoderRegular2d> processor_;
    IntProperty* plane_waves_;

public:
    Hoa2dProjector(const PdArgs& args);
    void initDone() override;
    void blockSizeChanged(size_t bs) override;
    void processBlock(const t_sample** in, t_sample** out) override;
};

void setup_spat_hoa_2d_projector();

#endif // HOA_2D_PROJECTOR_H
