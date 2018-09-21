#include "osc_saw.h"
#include "ceammc_factory.h"

using namespace ceammc;

class OscSaw : public faust_saw_tilde {
public:
    OscSaw(const PdArgs& args)
        : faust_saw_tilde(args)
    {
        createInlet();
        setInitSignalValue(positionalFloatArgument(0, 0));
    }

    void onInlet(size_t n, const AtomList&) override
    {
        dsp_->instanceClear();
    }
};

void setup_osc_saw_tilde()
{
    SoundExternalFactory<OscSaw> obj("osc.saw~");
}
