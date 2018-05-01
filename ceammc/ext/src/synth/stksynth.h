#ifndef STKSYNTH_H
#define STKSYNTH_H

#include "ceammc_sound_external.h"

using namespace ceammc;

namespace stk {
class Instrmnt;
}

class StkSynth : public SoundExternal {
protected:
    stk::Instrmnt* synth_;

private:
    FloatPropertyMin* freq_;
    t_float gate_;

public:
    StkSynth(const PdArgs& args, stk::Instrmnt* instr);
    ~StkSynth() override;

    void setupDSP(t_signal** sp) override;
    void processBlock(const t_sample**, t_sample** out) override;

    AtomList propGate() const;
    void propSetGate(const AtomList& lst);

    void m_cc(t_symbol* s, const AtomList& lst);
    t_float getCC(int n) const;

    template <class T>
    T* synth() { return static_cast<T*>(synth_); }
};

template <class T>
class ControlChangeProperty : public Property {
    StkSynth& synth_;
    int channel_;
    t_float value_;

public:
    ControlChangeProperty(const char* name, int ch, StkSynth& synth)
        : Property(name)
        , synth_(synth)
        , channel_(ch)
        , value_(0)
    {
        value_ = synth_.synth<T>()->getControlChange(ch);
    }

    AtomList get() const override
    {
        return Atom(value_);
    }

    bool set(const AtomList& lst) override
    {
        if (lst.empty())
            return false;

        value_ = lst[0].asFloat();
        synth_.m_cc(gensym("cc"), AtomList(channel_, value_));
        return true;
    }
};

#endif // STKSYNTH_H
