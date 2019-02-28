#include "osc_sin.h"
#include "ceammc_factory.h"

using namespace ceammc;

class OscSin : public faust_osc_sin_tilde {
public:
    OscSin(const PdArgs& args)
        : faust_osc_sin_tilde(args)
    {
        createInlet();
        setInitSignalValue(positionalFloatArgument(0, 0));
    }

    void onInlet(size_t n, const AtomList&) override
    {
        dsp_->instanceClear();
    }
};

void setup_osc_sin_tilde()
{
    SoundExternalFactory<OscSin> obj("osc.sin~");
}
