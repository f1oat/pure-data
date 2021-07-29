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
#ifndef RAGEL_MUSIC_H
#define RAGEL_MUSIC_H

#include <array>
#include <cstddef>
#include <cstdint>

#include <boost/container/small_vector.hpp>

#include "ceammc_atomlist.h"
#include "ceammc_convert.h"
#include "ragel_common.h"

namespace ceammc {
namespace parser {

    enum OctaveType : int8_t {
        OCTAVE_ABS = 0,
        OCTAVE_REL
    };

    enum DurationType : int8_t {
        DURATION_ABS = 0,
        DURATION_REL
    };

    constexpr int8_t MIN_OCTAVE = -1;
    constexpr int8_t MAX_OCTAVE = 10;

    namespace fsm {
        // for ragel

        struct BpmData {
            int ival { 0 };
            int fnum { 0 };
            int fden { 1 };
            int dur_num { 1 };
            int dur_den { 4 };
        };

        struct SpnData {
            int pitch { 0 };
            int note { 0 };
            int alt { 0 };
            int oct { 0 };
            int dev { 0 };
            int sign { 1 };
            OctaveType octtype { OCTAVE_ABS };
        };

        struct NotationData {
            int dots { 0 };
            int num { 0 };
            int den { 1 };
            int rest { 0 };
            int tnum { 0 };
            int tden { 0 };
            int repeats { 0 };
            DurationType durtype { DURATION_ABS };
        };
    }

    struct Bpm {
        float bpm;
        float beatlen;

        Bpm(float bpm_ = 0, float beatlen_ = 0.25)
            : bpm(bpm_)
            , beatlen(beatlen_)
        {
        }

        Bpm(const fsm::BpmData& d)
            : bpm(d.ival + double(d.fnum) / double(d.fden))
            , beatlen(double(d.dur_num) / double(d.dur_den))
        {
        }

        bool operator==(const Bpm& b) const
        {
            return bpm == b.bpm
                && beatlen == b.beatlen;
        }

        bool operator!=(const Bpm& b) const
        {
            return !operator==(b);
        }

        t_float value() const { return bpm; }
        t_float freqHz() const { return value() / 60.0; }

        t_float periodMs(t_float def = 0) const
        {
            return (bpm > 0) ? 60000.0 / value() : def;
        }

        t_float periodSamp(t_float sr, t_float def = 0) const
        {
            return (bpm > 0) ? sr * 60.0 / value() : def;
        }

        t_float periodWholeMs(t_float def = 0) const
        {
            return (beatlen > 0) ? (periodMs(def) / beatlen) : def;
        }
    };

    template <size_t N>
    using SmallBpmVecN = boost::container::small_vector<Bpm, N>;
    using SmallBpmVec = SmallBpmVecN<8>;

    class BpmFullMatch {
        int cs { 0 };
        Bpm bpm_ { 0, 0.25 };

    public:
        BpmFullMatch();
        void reset();

        const Bpm& bpm() const { return bpm_; }

        bool parse(const char* str);
        bool parse(const Atom& a);
        size_t parse(const AtomListView& lv, SmallBpmVec& out);

        template <size_t N>
        std::array<Bpm, N> parseN(const AtomListView& lv, const Bpm& def = { 0, 0.25 })
        {
            std::array<Bpm, N> res;

            const auto M = lv.size();
            for (size_t i = 0; i < N; i++) {
                if (i < M && parse(lv[i]))
                    res[i] = bpm_;
                else
                    res[i] = def;
            }

            return res;
        }
    };

    struct Spn {
        int8_t p;
        int8_t note;
        int8_t alt;
        int8_t oct;
        int8_t dev;
        OctaveType octtype;

        Spn(int8_t pitch = 0, int8_t alt_ = 0, int8_t oct_ = 0, int8_t dev_ = 0)
            : p(pitch)
            , alt(alt_)
            , oct(oct_)
            , dev(dev_)
            , octtype(OCTAVE_ABS)
        {
            note = (p + (p > 4)) >> 1;
        }

        Spn(const fsm::SpnData& sd)
            : Spn(sd.pitch, sd.alt, sd.oct, sd.dev)
        {
            octtype = sd.octtype;
        }

        bool operator==(const Spn& spn) const
        {
            return p == spn.p
                && alt == spn.alt
                && oct == spn.oct
                && dev == spn.dev;
        }

        bool operator!=(const Spn& spn) const
        {
            return !operator==(spn);
        }

        t_float midi(int rel_octave = 4) const
        {
            int noct = 0;

            switch (octtype) {
            case OCTAVE_ABS:
                noct = oct;
                break;
            case OCTAVE_REL:
            default:
                noct = rel_octave + oct;
                break;
            }

            return (clip<int8_t, MIN_OCTAVE, MAX_OCTAVE>(noct) + 1) * 12 + p + alt + t_float(dev) / 100;
        }

        bool isRest() const { return p < 0; }
        bool isAbsOctave() const { return octtype == OCTAVE_ABS; }

        void setAbsOctave(int8_t noct)
        {
            oct = clip<int8_t, MIN_OCTAVE, MAX_OCTAVE>(noct);
            octtype = OCTAVE_ABS;
        }

        static int8_t nextIntervalOctave(int8_t notePrev, int8_t noteNext)
        {
            const auto a = notePrev;
            const auto b = noteNext;

            if ((b - a) > 4)
                return -1;
            else if ((a - b) > 3)
                return 1;
            else
                return 0;
        }

        static void setNextOctave(const Spn& spn, Spn& next)
        {
            if (!spn.isAbsOctave())
                return;

            switch (next.octtype) {
            case OCTAVE_REL:
                next.oct = clip<int8_t, MIN_OCTAVE, MAX_OCTAVE>(spn.oct + next.oct);
                break;
            case OCTAVE_ABS:
            default:
                return;
            }

            next.octtype = OCTAVE_ABS;
        }
    };

    template <size_t N>
    using SmallSpnVecN = boost::container::small_vector<Spn, N>;
    using SmallSpnVec = SmallSpnVecN<8>;

    class SpnFullMatch {
        int cs { 0 };
        Spn spn_;

    public:
        SpnFullMatch();
        void reset();

        const Spn& spn() const { return spn_; }

        bool parse(const char* str);
        bool parse(const Atom& a);
        size_t parse(const AtomListView& lv, SmallSpnVec& out);

        template <size_t N>
        std::array<Spn, N> parseN(const AtomListView& lv, const Spn& def = { 0, 0, 0, 0 })
        {
            std::array<Spn, N> res;

            const auto M = lv.size();
            for (size_t i = 0; i < N; i++) {
                if (i < M && parse(lv[i]))
                    res[i] = spn_;
                else
                    res[i] = def;
            }

            return res;
        }

        static void calcAbsOctaves(SmallSpnVec& spn, int oct = 4)
        {
            if (spn.size() > 0 && !spn[0].isAbsOctave())
                spn[0].setAbsOctave(oct);

            for (size_t i = 0; i + 1 < spn.size(); i++) {
                const auto& p0 = spn[i];
                auto& p1 = spn[i + 1];
                Spn::setNextOctave(p0, p1);
            }
        }
    };

    struct Duration {
        int16_t num;
        int16_t den;
        int8_t dots;
        int8_t repeats;
        DurationType durtype;

        Duration(int16_t n = 1, int16_t d = 4, int8_t ndots = 0, DurationType dt = DURATION_ABS)
            : num(n)
            , den(d)
            , dots(ndots)
            , repeats(1)
            , durtype(dt)
        {
        }

        Duration(const fsm::NotationData& d)
            : Duration(d.num, d.den, d.dots, d.durtype)
        {
            repeats = d.repeats;
        }

        t_float ratio() const { return t_float(num) / den; }

        t_float timeMs(const Bpm& bpm = { 60, 0.25 }) const
        {
            return bpm.periodMs() * ratio() / bpm.beatlen;
        }

        t_float timeSamp(t_float sr, const Bpm& bpm = { 60, 0.25 }) const
        {
            return bpm.periodSamp(sr) * ratio() * den;
        }

        bool isAbs() const { return durtype == DURATION_ABS; }

        friend std::ostream& operator<<(std::ostream& os, const Duration& dur);
    };

    std::ostream& operator<<(std::ostream& os, const Duration& dur);

    struct Notation {
        Spn spn;
        Duration dur;

        Notation() { }

        Notation(const fsm::SpnData& fs, const fsm::NotationData& n)
            : spn(fs)
            , dur(n)
        {
            if (n.rest) {
                spn.p = -1;
                spn.alt = 0;
                spn.oct = 0;
            }
        }

        bool isRest() const { return spn.isRest(); }
    };

    template <size_t N>
    using SmallNoteVecN = boost::container::small_vector<Notation, N>;
    using NoteVec = SmallNoteVecN<16>;

    class NotationSingle {
        int cs { 0 };
        Notation note_;

    public:
        NotationSingle();
        void reset();

        const Notation& note() const { return note_; }

        bool parse(const char* str);
        bool parse(const Atom& a);
        size_t parse(const AtomListView& lv, NoteVec& out);

        static void calcAbsOctaves(NoteVec& notes, int oct = 4)
        {
            // find first note
            for (size_t i = 0; i < notes.size(); i++) {
                if (notes[i].isRest())
                    continue;

                // if relative: set abs octave
                if (!notes[i].spn.isAbsOctave())
                    notes[i].spn.setAbsOctave(oct);

                break;
            }

            Spn* p0 = nullptr;
            Spn* p1 = nullptr;

            for (size_t i = 0; i < notes.size(); i++) {
                if (notes[i].isRest())
                    continue;

                auto& p = notes[i].spn;

                std::swap(p0, p1);
                p1 = &p;

                if (p0 && p1)
                    Spn::setNextOctave(*p0, *p1);
            }
        }
    };

    template <size_t N>
    using DurationVecN = boost::container::small_vector<Duration, N>;
    using DurationVec = DurationVecN<16>;

    class DurationFullMatch {
        int cs { 0 };
        Duration dur_;

    public:
        DurationFullMatch();
        void reset();

        const Duration& result() const { return dur_; }

        bool parse(const char* str);
        bool parse(const Atom& a);
        size_t parse(const AtomListView& lv, DurationVec& out);
    };
}
}

#endif // RAGEL_NUMERIC_H
