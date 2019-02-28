#include "fx_chorus.h"
#include "ceammc_factory.h"

using namespace ceammc;

class FxChorus : public faust_fx_chorus_tilde {
public:
    FxChorus(const PdArgs& args)
        : faust_fx_chorus_tilde(args)
    {
    }

    void m_reset(t_symbol*, const AtomList&)
    {
        dsp_->instanceClear();
    }
};

void setup_fx_chorus_tilde()
{
    SoundExternalFactory<FxChorus> obj("fx.chorus~");
    obj.addMethod("reset", &FxChorus::m_reset);
}
