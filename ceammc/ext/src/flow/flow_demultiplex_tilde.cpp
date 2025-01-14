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
#include "flow_demultiplex_tilde.h"
#include "ceammc_convert.h"
#include "ceammc_factory.h"
#include "fmt/core.h"

constexpr int DEFAULT_OUTLETS = 2;
constexpr int MIN_OUTLETS = 2;

static char INFO_OUT[DEMUX_MAX_OUTLETS][8] = {};

static size_t chMultiplier(const PdArgs& args)
{
    return (args.flags & DEMULTIPLEX_STEREO) ? 2 : 1;
}

DemultiplexTilde::DemultiplexTilde(const PdArgs& args)
    : SoundExternal(args)
{
    n_ = new IntProperty("@n", DEFAULT_OUTLETS, PropValueAccess::INITONLY);
    n_->checkClosedRange(MIN_OUTLETS, DEMUX_MAX_OUTLETS);
    n_->setArgIndex(0);
    addProperty(n_);

    createCbListProperty(
        "@value",
        [this]() -> AtomList {
            AtomList res;
            res.reserve(gain_.size());

            for (size_t i = 0; i < gain_.size(); i++)
                res.append(gain_[i].target());

            return res;
        },
        [this](const AtomListView& lv) -> bool { onList(lv);return true; });
}

void DemultiplexTilde::initDone()
{
    const auto N = n_->value();
    for (int i = 0; i < N * chMultiplier(pdArgs()); i++)
        createSignalOutlet();

    if (pdArgs().flags == DEMULTIPLEX_STEREO)
        createSignalInlet();

    createInlet();

    gain_.assign(N, t_smooth(0));
    gain_[0].setTargetValue(1);
}

void DemultiplexTilde::processBlock(const t_sample** in, t_sample** out)
{
    const size_t BS = blockSize();
    const size_t N = gain_.size();

    for (size_t i = 0; i < BS; i++) {
        t_sample v = in[0][i];
        for (size_t j = 0; j < N; j++)
            out[j][i] = gain_[j]() * v;
    }
}

void DemultiplexTilde::setupDSP(t_signal** in)
{
    SoundExternal::setupDSP(in);

    const double SR = samplerate();

    const size_t N = gain_.size();
    for (size_t i = 0; i < N; i++) {
        gain_[i].setDurationMs(20, SR);
    }
}

void DemultiplexTilde::onInlet(size_t /*n*/, const AtomListView& lv)
{
    const int idx = lv.intAt(0, -1);
    const t_float fidx = lv.floatAt(0, -1);

    if (idx < 0 || idx >= gain_.size()) {
        OBJ_ERR << "invalid index: " << lv;
        return;
    }

    // pow xfade
    const t_float k = fidx - idx;
    const t_float k0 = 1 - k * k;
    const t_float k1 = 1 - (k - 1) * (k - 1);

    for (size_t i = 0; i < gain_.size(); i++) {
        if (i == idx)
            gain_[i].setTargetValue(k0);
        else if (i == idx + 1)
            gain_[i].setTargetValue(k1);
        else
            gain_[i].setTargetValue(0);
    }
}

void DemultiplexTilde::onList(const AtomListView& lv)
{
    const auto N = std::min<size_t>(lv.size(), gain_.size());
    for (size_t i = 0; i < N; i++)
        gain_[i].setTargetValue(clip<t_float>(lv[i].asFloat(), 0, 1));
}

const char* DemultiplexTilde::annotateOutlet(size_t n) const
{
    if (n < DEMUX_MAX_OUTLETS)
        return INFO_OUT[n];
    else
        return "?";
}

void setup_flow_demultiplex_tilde()
{
    for (int i = 0; i < DEMUX_MAX_OUTLETS; i++)
        fmt::format_to(INFO_OUT[i], "\\[{}\\]\0", i);

    SoundExternalFactory<DemultiplexTilde> obj("flow.demultiplex~");
    obj.addAlias("flow.demux~");
    obj.addAlias("demux~");

    obj.setDescription("audio stream demultiplexer");
    obj.setCategory("flow");
    obj.setKeywords({ "flow", "demultiplex" });
    obj.setXletsInfo({ "input", "int: output select" }, {});
}
