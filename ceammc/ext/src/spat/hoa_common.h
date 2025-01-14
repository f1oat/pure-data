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
#ifndef HOA_COMMON_H
#define HOA_COMMON_H

#include <vector>

// MSYS temp define hack
#ifdef _WIN32
#ifndef _WINDOWS
#define _WINDOWS 1
#endif
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wsign-conversion"
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wextra"
#include "Hoa.hpp"
// temporal fix: this file not exists in newest Hoa-Library-Light
#include "Hoa_Meter.hpp"
#include "Hoa_Scope.hpp"
#include "Hoa_Tools.hpp"
#include "Hoa_Vector.hpp"
#pragma clang diagnostic pop

#include "ceammc_sound_external.h"
using namespace ceammc;

namespace ceammc {

constexpr size_t HOA_MIN_PLANEWAVES = 1;
constexpr size_t HOA_MAX_PLANEWAVES = 128;
constexpr size_t HOA_MIN_ORDER = 1;
constexpr size_t HOA_MAX_2D_ORDER = 63;
constexpr size_t HOA_MAX_3D_ORDER = 10;
constexpr size_t HOA_DEFAULT_ORDER = 1;
constexpr size_t HOA_DEFAULT_BLOCK_SIZE = 64;
constexpr size_t HOA_MAXBLKSIZE = 8192;

using Decoder2d = hoa::Decoder<hoa::Hoa2d, t_sample>;
using Decoder3d = hoa::Decoder<hoa::Hoa3d, t_sample>;
using DecoderBinaural2d = hoa::DecoderBinaural<hoa::Hoa2d, t_sample, hoa::hrir::Sadie_D2_2D>;
using DecoderBinaural3d = hoa::DecoderBinaural<hoa::Hoa3d, t_sample, hoa::hrir::Sadie_D2_3D>;
using DecoderIrregular2d = hoa::DecoderIrregular<hoa::Hoa2d, t_sample>;
using DecoderRegular2d = hoa::DecoderRegular<hoa::Hoa2d, t_sample>;
using DecoderRegular3d = hoa::DecoderRegular<hoa::Hoa3d, t_sample>;
using Encoder2d = hoa::Encoder<hoa::Hoa2d, t_sample>;
using Encoder3d = hoa::Encoder<hoa::Hoa3d, t_sample>;
using Exchanger2d = hoa::Exchanger<hoa::Hoa2d, t_sample>;
using Exchanger3d = hoa::Exchanger<hoa::Hoa3d, t_sample>;
using Meter3d = hoa::Meter<hoa::Hoa3d, t_sample>;
using MultiEncoder2d = hoa::MultiEncoder<hoa::Hoa2d, t_sample>;
using MultiEncoder3d = hoa::MultiEncoder<hoa::Hoa3d, t_sample>;
using Optim2d = hoa::Optim<hoa::Hoa2d, t_sample>;
using Optim3d = hoa::Optim<hoa::Hoa3d, t_sample>;
using PolarLines2d = hoa::PolarLines<hoa::Hoa2d, t_sample>;
using PolarLines3d = hoa::PolarLines<hoa::Hoa3d, t_sample>;
using Rotate2d = hoa::Rotate<hoa::Hoa2d, t_sample>;
using Scope2d = hoa::Scope<hoa::Hoa2d, t_sample>;
using Scope3d = hoa::Scope<hoa::Hoa3d, t_sample>;
using Vector3d = hoa::Vector<hoa::Hoa3d, t_sample>;
using Wider2d = hoa::Wider<hoa::Hoa2d, t_sample>;
using Wider3d = hoa::Wider<hoa::Hoa3d, t_sample>;

using Signal = hoa::Signal<t_sample>;
using Buffer = std::vector<t_sample>;

template <hoa::Dimension D>
struct HOA_ORDER {
    static constexpr size_t MAX_VALUE = 1;
};

template <>
struct HOA_ORDER<hoa::Hoa2d> {
    static constexpr size_t MAX_VALUE = HOA_MAX_2D_ORDER;
};

template <>
struct HOA_ORDER<hoa::Hoa3d> {
    static constexpr size_t MAX_VALUE = HOA_MAX_3D_ORDER;
};
}

template <hoa::Dimension D>
class HoaBase : public SoundExternal {
    IntProperty* order_ { nullptr };

public:
    HoaBase(const PdArgs& args)
        : SoundExternal(args)
    {
        order_ = new IntProperty("@order", HOA_DEFAULT_ORDER);
        order_->setInitOnly();
        order_->checkClosedRange(HOA_MIN_ORDER, HOA_ORDER<D>::MAX_VALUE);
        order_->setArgIndex(0);
        addProperty(order_);
    }

    int order() const { return order_->value(); }

    void createSignalInlets(size_t n)
    {
        for (size_t i = numInputChannels(); i < n; i++)
            createSignalInlet();
    }

    void createSignalOutlets(size_t n)
    {
        for (size_t i = 0; i < n; i++)
            createSignalOutlet();
    }

protected:
    IntProperty* prop_order() { return order_; }
};

#endif // HOA_COMMON_H
