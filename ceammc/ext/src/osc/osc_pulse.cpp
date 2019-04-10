#include "osc_pulse.h"
#include "ceammc_factory.h"

using namespace ceammc;

class OscPulse : public faust_osc_pulse_tilde {
public:
    OscPulse(const PdArgs& args)
        : faust_osc_pulse_tilde(args)
    {
        createInlet();
        setInitSignalValue(positionalFloatArgument(0, 0));
        bindPositionalArgToProperty(1, gensym("@duty"));
    }

    void onInlet(size_t n, const AtomList&) override
    {
        dsp_->instanceClear();
    }
};

void setup_osc_pulse_tilde()
{
    SoundExternalFactory<OscPulse> obj("osc.pulse~");
}
