#include "fx_flanger.h"
#include "ceammc_factory.h"

using namespace ceammc;

class FxFlanger : public faust_fx_flanger_tilde {
public:
    FxFlanger(const PdArgs& args)
        : faust_fx_flanger_tilde(args)
    {
    }
};

void setup_fx_flanger_tilde()
{
    SoundExternalFactory<FxFlanger> obj("fx.flanger~");
}
