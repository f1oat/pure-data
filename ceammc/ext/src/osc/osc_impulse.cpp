#include "osc_impulse.h"
#include "ceammc_factory.h"

using namespace ceammc;

class OscImpulse : public faust_osc_impulse_tilde {
public:
    OscImpulse(const PdArgs& args)
        : faust_osc_impulse_tilde(args)
    {
        createInlet();
        setInitSignalValue(positionalFloatArgumentT(0, 0));
    }

    void onInlet(size_t n, const AtomList&) override
    {
        dsp_->instanceClear();
    }
};

void setup_osc_impulse_tilde()
{
    SoundExternalFactory<OscImpulse> obj("osc.impulse~");
}
