#include "lfo_pulse_pos.h"
#include "ceammc_factory.h"
using namespace ceammc;

class LfoPulsePos : public faust_lfo_pulse_pos_tilde {
public:
    LfoPulsePos(const PdArgs& args)
        : faust_lfo_pulse_pos_tilde(args)
    {
        createInlet();
        setInitSignalValue(parsedPosArgs().floatAt(0, 0));
        bindPositionalArgToProperty(1, gensym("@duty"));
    }

    void onInlet(size_t n, const AtomListView&) override
    {
        dsp_->instanceClear();
    }
};

void setup_lfo_pulse_pos_tilde()
{
    SoundExternalFactory<LfoPulsePos> obj("lfo.+pulse~");
    obj.addMethod("reset", &LfoPulsePos::m_reset);
    obj.setXletsInfo({ "freq in hz", "bang: reset phase" }, { "pulse wave in \\[0,+1\\] range" });
}
