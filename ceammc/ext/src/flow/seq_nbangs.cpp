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
#include "seq_nbangs.h"
#include "ceammc_convert.h"
#include "ceammc_crc32.h"
#include "ceammc_factory.h"

CEAMMC_DEFINE_SYM(done)
CEAMMC_DEFINE_SYM(i)

SeqNBangs::SeqNBangs(const PdArgs& args)
    : BaseObject(args)
    , n_(nullptr)
    , interval_(nullptr)
    , counter_(0)
    , clock_([this]() {
        if (tick())
            clock_.delay(calcStepDelay(counter_));
    })
{
    createInlet();
    createOutlet();
    createOutlet();

    n_ = new IntProperty("@n", 0);
    n_->checkNonNegative();
    n_->setArgIndex(0);
    addProperty(n_);

    interval_ = new SeqTimeGrain("@t", 0);
    interval_->setArgIndex(1);
    addProperty(interval_);

    beat_division_ = new IntProperty("@div", 1);
    beat_division_->checkClosedRange(1, 64);
    addProperty(beat_division_);

    accel_ = new FloatProperty("@accel", 1);
    accel_->checkClosedRange(0.25, 4);
    addProperty(accel_);

    accel_curve_ = new FloatProperty("@curve", 1);
    accel_curve_->checkClosedRange(-8, 8);
    addProperty(accel_curve_);

    createCbFloatProperty(
        "@dur",
        [this]() -> t_float { return calcTotalDur(); },
        [this](t_float f) -> bool {
            const auto N = n_->value();
            if (N == 0) {
                OBJ_ERR << "empty sequence";
                return false;
            }

            return interval_->setValue(calcIntervalByDur(f));
        })
        ->checkNonNegative();
}

void SeqNBangs::onBang()
{
    reset();
    start();
}

void SeqNBangs::onFloat(t_float f)
{
    reset();

    if (n_->setValue(f))
        clock_.exec();
}

void SeqNBangs::onList(const AtomListView& lv)
{
    if (lv.empty())
        return onBang();
    else if (lv.size() == 1)
        return onFloat(lv[0].asFloat());
    else if (lv.size() == 2) {
        reset();
        if (n_->set(lv.subView(0, 1)) && interval_->set(lv.subView(1, 1)))
            clock_.exec();
    } else
        OBJ_ERR << "usage: NUM INTERVAL";
}

void SeqNBangs::onInlet(size_t n, const AtomListView& lv)
{
    n_->set(lv);
}

void SeqNBangs::start()
{
    clock_.exec();
}

void SeqNBangs::stop()
{
    clock_.unset();
}

void SeqNBangs::reset()
{
    counter_ = 0;
    clock_.unset();
}

bool SeqNBangs::tick()
{
    if ((int)counter_ >= n_->value()) {
        anyTo(1, sym_done(), AtomListView());
        return false;
    } else {
        Atom l[2] = { counter_, n_->value() };
        anyTo(1, sym_i(), AtomListView(l, 2));
        bangTo(0);

        counter_++;
        return true;
    }
}

t_float SeqNBangs::calcIntervalByDur(t_float dur) const
{
    const auto N = n_->value();

    if (accel_->value() == 1) {
        return dur / N;
    } else if (accel_curve_->value() == 0) {
        // T = (i0+i0/a) * (N-1) / 2
        // 2T/(N-1) == (i0 + i0/a)
        // 2T/(N-1) == (i0 + i0/a)
        // 2T/(N-1)*a == ai0 + i0
        // 2T/(N-1)*a == i0(a + 1)
        // 2T/(N-1)*a/(a+1) == i0
        auto a = accel_->value();
        return ((dur * 2) / (N - 1)) * (a / (1 + a));
    } else {
        auto k = dur / calcTotalDur();
        return interval_->value() * k;
    }
}

t_float SeqNBangs::calcTotalDur() const
{
    const auto N = n_->value();

    if (N == 0)
        return 0;
    else if (accel_->value() == 1)
        return N * interval_->value();
    else if (accel_curve_->value() == 0) {
        auto i0 = interval_->value();
        auto in = i0 / accel_->value();
        return ((i0 + in) * (N - 1) / 2);
    } else {
        t_float res = 0;
        for (int i = 0; i < N - 1; i++)
            res += calcStepDelay(i);

        return res;
    }
    return 0;
}

t_float SeqNBangs::calcStepDelay(int n) const
{
    if (accel_->value() == 1) {
        return interval_->value() / beat_division_->value();
    } else {
        auto interval = interval_->value() / beat_division_->value();
        auto speed = convert::lin2curve(n, 0, n_->value() - 2, 1, 1 / accel_->value(), accel_curve_->value());
        return interval * speed;
    }
}

void setup_seq_nbangs()
{
    SequencerIFaceFactory<ObjectFactory, SeqNBangsT> obj("seq.nbangs");
    obj.addAlias("seq.nb");

    obj.setXletsInfo({ "bang: start\n"
                       "float: set number of bangs then start\n"
                       "list: NUM INTERVAL set number and interval then start\n"
                       "start 1|0: start/stop sequence\n"
                       "stop 1|0:  stop/start sequence",
                         "float: set number of bangs" },
        { "bang", "\\[i IDX N( - sequence iteration\n"
                  "\\[done( - when done" });

    obj.setDescription("output specified number of bang with time intervals");
    obj.setCategory("seq");
    obj.setKeywords({ "seq", "bang", "until" });
}
