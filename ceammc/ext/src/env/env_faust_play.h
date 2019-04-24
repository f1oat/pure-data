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
#ifndef ENV_FAUST_PLAY_H
#define ENV_FAUST_PLAY_H

#include "ceammc_clock.h"
#include "ceammc_object.h"

namespace ceammc {

template <class T>
class EnvAutoplay : public T {
    ClockMemberFunction<EnvAutoplay<T>> auto_play_clock_;

public:
    EnvAutoplay(const PdArgs& args)
        : T(args)
        , auto_play_clock_(this, &EnvAutoplay<T>::playDone)
    {
    }

    void m_play(t_symbol* sel, const AtomList& dur)
    {
        bool ok = !dur.empty() && dur[0].isFloat() && dur[0].asFloat() > 0;
        if (!ok) {
            METHOD_ERR(sel) << "DURATION(ms) expected: " << dur;
            return;
        }

        // reset envelope
        sendReset();
        // activate timer
        auto_play_clock_.delay(dur[0].asFloat());
        // gate on
        sendGate(1);
    }

private:
    void playDone()
    {
        sendGate(0);
    }

    void sendReset()
    {
        static t_symbol* SYM_RESET = gensym("reset");
        pd_typedmess(&(this->owner()->te_g.g_pd), SYM_RESET, 0, nullptr);
    }

    void sendGate(bool on)
    {
        static t_symbol* SYM_PROP_GATE2 = gensym("@gate");

        // send @gate message
        t_atom v;
        SETFLOAT(&v, on ? 1 : 0);
        pd_typedmess(&(this->owner()->te_g.g_pd), SYM_PROP_GATE2, 1, &v);
    }
};
}

#endif // ENV_FAUST_PLAY_H
