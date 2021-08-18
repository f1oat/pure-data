/*****************************************************************************
 * Copyright 2021 Serge Poltavsky. All rights reserved.
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
#ifndef FLOW_RECORD_H
#define FLOW_RECORD_H

#include <deque>
#include <utility>

#include "ceammc_clock.h"
#include "ceammc_containers.h"
#include "ceammc_object.h"
#include "ceammc_proxy.h"
using namespace ceammc;

using FlowMessage = SmallMessageN<4>;
using FlowEvent = std::pair<FlowMessage*, double>;

class FlowRecord : public BaseObject {
public:
    using ControlProxy = InletProxy<FlowRecord>;
    using Events = std::deque<FlowEvent>;

    enum State {
        STOP,
        PLAY,
        RECORD
    };

private:
    ControlProxy control_;
    ClockLambdaFunction clock_;
    Events events_;
    double rec_start_, rec_len_ms_;
    IntProperty* max_size_;
    IntProperty* repeats_;
    BoolProperty* auto_start_;
    FloatProperty* speed_;
    State state_;
    size_t current_idx_;
    int repeat_counter_;

public:
    FlowRecord(const PdArgs& args);
    ~FlowRecord();

    void onBang() override;
    void onFloat(t_float v) override;
    void onSymbol(t_symbol* s) override;
    void onList(const AtomList& lst) override;
    void onAny(t_symbol* s, const AtomListView& lv) override;

    void m_play(const AtomListView& lv) { setState(lv.boolAt(0, true) ? PLAY : STOP); }
    void m_stop(const AtomListView& lv) { setState(lv.boolAt(0, true) ? STOP : PLAY); }
    void m_pause(const AtomListView& lv) { setState(STOP); }
    void m_record(const AtomListView& lv) { setState(lv.boolAt(0, true) ? RECORD : STOP); }
    void m_clear(const AtomListView& lv);
    void m_flush(const AtomListView& lv);
    void m_quant(const AtomListView& lv);
    void m_qlist(const AtomListView& lv);

    void dump() const override;

public:
    const Events& events() const { return events_; }

private:
    void appendMessage(FlowMessage* m);
    bool sizeInf() const { return max_size_->value() == 0; }
    void setState(State new_st);
    void clear();

    bool repeatAgain() const { return (repeats_->value() > 0 && repeat_counter_ < repeats_->value()) || repeats_->value() < 0; }

    void schedMs(t_float ms) { clock_.delay(ms / speed_->value()); }
};

void setup_flow_record();

#endif // FLOW_RECORD_H