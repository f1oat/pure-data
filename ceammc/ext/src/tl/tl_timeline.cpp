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
#include "tl_timeline.h"
#include "ceammc_containers.h"
#include "ceammc_factory.h"
#include "ceammc_format.h"

#include <cstdio>

constexpr const char* SYM_START = "begin";
constexpr const char* SYM_END = "end";
constexpr const char* SYM_INF = "inf";
constexpr const char* SYM_INF2 = "infinite";
constexpr const char* SYM_FIXED = "fixed";

TlTimeLine::TlTimeLine(const PdArgs& args)
    : TlTimeLineBase(args)
    , tl_(this, 0)
    , cmd_parser_(this)
{
    createOutlet();

    length_ = new FloatProperty("@length", 60);
    length_->setInitOnly();
    length_->setUnitsSec();
    length_->checkMin(0);
    length_->setArgIndex(0);
    length_->setSuccessFn([this](Property*) {
        tl_.setLength(length_->value() * 1000);
    });
    addProperty(length_);

    createCbBoolProperty("@is_running",
        [this]() -> bool { return tl_.state() == tl::STATE_RUN; });

    createCbFloatProperty("@phase",
        [this]() -> t_float { return (tl_.length() == 0) ? 0 : (tl_.currentTime() / tl_.length()); })
        ->setFloatCheck(PropValueConstraints::CLOSED_RANGE, 0, 1);

    createCbIntProperty("@size", [this]() -> int { return tl_.events().size(); })
        ->checkNonNegative();

    createCbFloatProperty("@current",
        [this]() -> t_float { return tl_.currentTime(); })
        ->setUnitsMs();

    createCbFloatProperty("@current_sec",
        [this]() -> t_float { return std::round(tl_.currentTime() * 0.001); })
        ->setUnitsSec();

    createCbProperty("@events", &TlTimeLine::propEvents);

    createCbBoolProperty(
        "@loop",
        [this]() -> bool { return tl_.isLoop(); },
        [this](bool v) -> bool { tl_.setLoop(v); return true; });

    auto res = createCbSymbolProperty(
        "@mode",
        [this]() -> t_symbol* {
            switch (tl_.mode()) {
            case tl::MODE_FIXED:
                return gensym(SYM_FIXED);
            case tl::MODE_INFINITE:
            default:
                return gensym(SYM_INF);
            } },
        [this](t_symbol* s) -> bool {
            if (s == gensym(SYM_INF) || s == gensym(SYM_INF2))
                tl_.setMode(tl::MODE_INFINITE);
            else if (s == gensym(SYM_FIXED))
                tl_.setMode(tl::MODE_FIXED);
            else {
                OBJ_ERR << gensym(SYM_INF) << " or " << gensym(SYM_FIXED) << " expected";
                return false;
            }

            return true;
        });

    res->infoT().setConstraints(PropValueConstraints::ENUM);
    if (!res->infoT().addEnums({ SYM_INF, SYM_FIXED }))
        OBJ_ERR << "can't set enums";
}

void TlTimeLine::initDone()
{
    length_->callSuccessFn();
}

void TlTimeLine::dump() const
{
    OBJ_DBG << "absolute events:";
    int idx = 0;
    for (auto& t : tl_.events()) {
        OBJ_DBG << "    [" << (idx++) << "] " << t.name << " " << t.abs_time << " " << t.next_time;
    }

    BaseObject::dump();
}

void TlTimeLine::onFloat(t_float v)
{
    (v != 0) ? tl_.start() : tl_.stop();
}

void TlTimeLine::event(size_t n, const tl::Event& e)
{
    AtomArray<3> lst { n + 1, e.name, e.abs_time };
    listTo(0, lst.view());
}

void TlTimeLine::eventStart()
{
    AtomArray<3> lst { 0., gensym(SYM_START), 0.f };
    listTo(0, lst.view());
}

void TlTimeLine::eventEnd()
{
    AtomArray<3> lst { tl_.events().size(), gensym(SYM_END), tl_.length() };
    listTo(0, lst.view());
}

void TlTimeLine::m_add(t_symbol* s, const AtomListView& lv)
{
    if (!cmd_parser_.parse(to_string(Message(s, lv))))
        METHOD_ERR(s) << "can't add event: " << lv;
}

void TlTimeLine::m_remove(t_symbol* s, const AtomListView& lv)
{
    if (!cmd_parser_.parse(to_string(Message(s, lv))))
        METHOD_ERR(s) << "can't remove event: " << lv;
}

void TlTimeLine::m_clear(t_symbol* s, const AtomListView& lv)
{
    tl_.clear();
}

void TlTimeLine::m_start(t_symbol* s, const AtomListView& lv)
{
    tl_.start();
}

void TlTimeLine::m_stop(t_symbol* s, const AtomListView& lv)
{
    tl_.stop();
}

void TlTimeLine::m_pause(t_symbol* s, const AtomListView& lv)
{
    tl_.pause();
}

void TlTimeLine::m_reset(t_symbol* s, const AtomListView& lv)
{
    tl_.reset();
}

void TlTimeLine::m_to_event(t_symbol* s, const AtomListView& lv)
{
    if (!cmd_parser_.parse(to_string(Message(s, lv))))
        METHOD_ERR(s) << "can't move to event: " << lv;
}

void TlTimeLine::m_to_time(t_symbol* s, const AtomListView& lv)
{
    if (!cmd_parser_.parse(to_string(Message(s, lv))))
        METHOD_ERR(s) << "can't move to time: " << lv;
}

AtomList TlTimeLine::propNumEvents() const
{
    return Atom(tl_.events().size());
}

AtomList TlTimeLine::propCurrentTime() const
{
    return Atom(tl_.currentTime());
}

AtomList TlTimeLine::propPhase() const
{
    return Atom(tl_.currentTime() / tl_.length());
}

AtomList TlTimeLine::propEvents() const
{
    AtomList res;
    for (auto& e : tl_.events())
        res.append(Atom(e.name));

    return res;
}

tl::RunState TlTimeLine::state() const
{
    return tl_.state();
}

bool TlTimeLine::addAbsEvent(const std::string& name, double time)
{
    t_symbol* sym_name;
    if (name.empty()) {
        char buf[32];
        sprintf(buf, "event%d", (int)tl_.events().size());
        sym_name = gensym(buf);
    } else
        sym_name = gensym(name.c_str());

    return tl_.add(time, sym_name);
}

bool TlTimeLine::addRelEvent(const std::string& name, double time, const std::string& relName)
{
    if (name == relName) {
        OBJ_ERR << "same name and target: " << name;
        return false;
    }

    return tl_.addRelative(gensym(name.c_str()), time, gensym(relName.c_str()));
}

bool TlTimeLine::removeEvent(const std::string& name)
{
    return tl_.removeByName(gensym(name.c_str()));
}

bool TlTimeLine::removeEventAtTime(double time_ms)
{
    return tl_.removeAtTime(t_float(time_ms));
}

bool TlTimeLine::removeEventAtPos(int pos)
{
    auto res = tl_.removeAtPos(pos);
    if (!res)
        OBJ_ERR << "can't remove event from position: " << pos;

    return res;
}

bool TlTimeLine::moveToEvent(long idx)
{
    auto res = tl_.toEvent(idx);
    if (!res)
        OBJ_ERR << "can't move to event at: " << idx;

    return res;
}

bool TlTimeLine::moveToEvent(const std::string& name)
{
    auto res = tl_.toEvent(gensym(name.c_str()));
    if (!res)
        OBJ_ERR << "can't move to event:  " << name;

    return res;
}

bool TlTimeLine::moveToTime(double time_ms)
{
    return tl_.toTime(time_ms);
}

void TlTimeLine::clearAll()
{
    tl_.clear();
}

void TlTimeLine::error(const std::string& msg)
{
    OBJ_ERR << msg;
}

const tl::EventList& TlTimeLine::events() const
{
    return tl_.events();
}

void setup_tl_timeline()
{
    ObjectFactory<TlTimeLine> obj("tl.timeline");
    obj.addAlias("timeline");
    obj.addMethod("add", &TlTimeLine::m_add);
    obj.addMethod("remove", &TlTimeLine::m_remove);
    obj.addMethod("remove_at", &TlTimeLine::m_remove);
    obj.addMethod("clear", &TlTimeLine::m_clear);

    obj.addMethod("start", &TlTimeLine::m_start);
    obj.addMethod("stop", &TlTimeLine::m_stop);
    obj.addMethod("pause", &TlTimeLine::m_pause);

    obj.addMethod("reset", &TlTimeLine::m_reset);
    obj.addMethod("to_event", &TlTimeLine::m_to_event);
    obj.addMethod("to_time", &TlTimeLine::m_to_time);
}
