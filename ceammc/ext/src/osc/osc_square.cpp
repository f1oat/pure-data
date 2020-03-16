#include "osc_square.h"
#include "ceammc_factory.h"

using namespace ceammc;

class OscSquare : public faust_osc_square_tilde {
public:
    OscSquare(const PdArgs& args)
        : faust_osc_square_tilde(args)
    {
        createInlet();
        setInitSignalValue(positionalFloatArgumentT(0, 0));
    }

    void onInlet(size_t n, const AtomList&) override
    {
        dsp_->instanceClear();
    }
};

void setup_osc_square_tilde()
{
    SoundExternalFactory<OscSquare> obj("osc.square~");
}
