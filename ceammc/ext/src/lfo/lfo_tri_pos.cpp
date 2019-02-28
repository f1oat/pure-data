#include "lfo_tri_pos.h"
#include "ceammc_factory.h"

using namespace ceammc;

class LfoTriPos : public faust_lfo_tri_pos_tilde {
public:
    LfoTriPos(const PdArgs& args)
        : faust_lfo_tri_pos_tilde(args)
    {
        createInlet();
        setInitSignalValue(positionalFloatArgument(0, 0));
    }

    void onInlet(size_t n, const AtomList&) override
    {
        dsp_->instanceClear();
    }
};

void setup_lfo_tri_pos_tilde()
{
    SoundExternalFactory<LfoTriPos> obj("lfo.+tri~");
}
