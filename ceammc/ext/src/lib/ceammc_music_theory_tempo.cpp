/*****************************************************************************
 * Copyright 2023 Serge Poltavski. All rights reserved.
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
#include "ceammc_music_theory_tempo.h"
#include "ceammc_log.h"
#include "fmt/core.h"
#include "lex/parser_music.h"

#include <iostream>

using namespace ceammc::music;

constexpr int TEMPO_MAX_DOTS = 3;

Tempo::Tempo() noexcept
    : bpm_(60)
    , dur_ { 1, 4 }
{
}

Tempo::Tempo(float bpm, int div, int dots)
{
    if (!setBpm(bpm))
        throw std::invalid_argument(fmt::format("invalid BPM value: {}", bpm));

    if (!setDivision(div))
        throw std::invalid_argument("invalid tempo beat division");

    if (!setDots(dots))
        throw std::invalid_argument(fmt::format("invalid dots: {}", dots));
}

Tempo::Tempo(float bpm, const Duration& dur)
    : bpm_(60)
    , dur_(dur)
{
    if (!setBpm(bpm))
        throw std::invalid_argument(fmt::format("invalid BPM value: {}", bpm));
}

bool Tempo::operator==(const Tempo& t) const
{
    return t.bpm_ == bpm_;
}

Tempo Tempo::operator*(double v) const
{
    auto res = *this;
    res.bpm_ *= v;
    return res;
}

bool Tempo::strictEqual(const Tempo& t) const
{
    return dur_.strictEqual(t.dur_);
}

Tempo Tempo::normalized() const
{
    auto dur = beatDuration().normalized();
    return { bpm_ * dur.numerator(), dur.division(), 0 };
}

bool Tempo::setBpm(float bpm) noexcept
{
    if (bpm < 0) {
        return false;
    } else {
        bpm_ = bpm;
        return true;
    }
}

bool Tempo::setDots(int dots) noexcept
{
    return dur_.setDots(dots);
}

bool Tempo::setDivision(int div) noexcept
{
    return dur_.setDivision(div);
}

bool Tempo::set(float bpm, int div, int dots) noexcept
{
    if (!setBpm(bpm))
        return false;

    return dur_.set(1, div, dots);
}

bool Tempo::parse(const char* str)
{
    return parser::parse_tempo(str, *this);
    //        return false;

    //    Tempo tmp;
    //    if (!tmp.setBpm(t.bpm))
    //        return false;

    //    if (!tmp.dur_.set(t.beat_num, t.beat_div, 0))
    //        return false;

    //    *this = tmp;
    //    return true;
}

std::string Tempo::toString() const
{
    const char* DOTS = &"..."[TEMPO_MAX_DOTS - dur_.dots()];
    if (dur_.numerator() == 1)
        return fmt::format("{}|{}{}bpm", bpm_, dur_.division(), DOTS);
    else
        return fmt::format("{}|{}bpm", bpm_, dur_.toString());
}

Duration Tempo::beatDuration() const
{
    return dur_;
}

Duration Tempo::beatSubDivDuration() const
{
    return dur_.subDivision();
}

int Tempo::beatSubDivision() const
{
    return beatDuration().normalized().numerator();
}

double Tempo::beatDurationMs() const
{
    if (bpm_ == 0) {
        LIB_ERR << "zero BPM duration";
        return 0;
    } else
        return 60000.0 / bpm_;
}

double Tempo::subBeatDurationMs() const
{
    return beatDurationMs() / beatSubDivision();
}

double Tempo::wholeNoteDurationMs() const
{
    return Duration { 1, 1 }.timeMs(*this);
}

void Tempo::setDuration(const Duration& d)
{
    dur_ = d;
}

Tempo Tempo::intepolate(const Tempo& t0, const Tempo& t1, double t)
{
    float k = t0.dur_ / t1.dur_;
    float bpm0 = t0.bpm() * k;
    float bpm1 = t1.bpm();
    return { convert::lin2lin_clip<float, 0, 1>(t, bpm0, bpm1), t1.dur_ };
}

std::ostream& ceammc::music::operator<<(std::ostream& os, const Tempo& t)
{
    os << t.toString();
    return os;
}
