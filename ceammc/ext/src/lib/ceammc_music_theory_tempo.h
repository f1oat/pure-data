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
#ifndef CEAMMC_MUSIC_THEORY_TEMPO_H
#define CEAMMC_MUSIC_THEORY_TEMPO_H

#include "ceammc_music_theory_duration.h"

#include <cstdint>

namespace ceammc {
namespace music {

    class Tempo {
        float bpm_ { 60 };
        Duration dur_;

    public:
        /**
         * creates tempo with: 1/4 = 60bpm
         */
        Tempo() noexcept;

        /**
         * Create tempo
         * @param bpm
         * @param div
         * @param dots
         * @throw std::invalid_argument
         */
        Tempo(float bpm, int div = 4, int dots = 0);

        /**
         * create tempo
         * @param bpm
         * @param dur
         * @throw std::invalid_argument
         */
        Tempo(float bpm, const Duration& dur);

        bool operator==(const Tempo& t) const;
        bool operator!=(const Tempo& t) const { return !operator==(t); }

        Tempo operator*(double v) const;

        bool strictEqual(const Tempo& t) const;

        Tempo normalized() const;

        float bpm() const { return bpm_; }
        bool setBpm(float bpm) noexcept;

        int dots() const { return dur_.dots(); }
        bool setDots(int dots) noexcept;

        int division() const { return dur_.division(); }
        bool setDivision(int div) noexcept;

        bool set(float bpm, int div, int dots = 0) noexcept;
        bool parse(const char* str);

        std::string toString() const;
        Duration beatDuration() const;
        Duration beatSubDivDuration() const;
        int beatSubDivision() const;

        double beatDurationMs() const;
        double subBeatDurationMs() const;
        double wholeNoteDurationMs() const;

        const Duration& duration() const { return dur_; }
        void setDuration(const Duration& d);

        bool isNull() const { return bpm_ == 0; }

        static Tempo intepolate(const Tempo& t0, const Tempo& t1, double t);
    };

    std::ostream& operator<<(std::ostream& os, const Tempo& t);
}
}

#endif // CEAMMC_MUSIC_THEORY_TEMPO_H
