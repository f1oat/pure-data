#include "lfo_saw_pos.h"
#include "ceammc_factory.h"
using namespace ceammc;

class LfoSawPos : public faust_lfo_saw_pos_tilde {
public:
    LfoSawPos(const PdArgs& args)
        : faust_lfo_saw_pos_tilde(args)
    {
        createInlet();
        setInitSignalValue(parsedPosArgs().floatAt(0, 0));
        bindPositionalArgToProperty(1, gensym("@invert"));
    }

    void onInlet(size_t n, const AtomListView&) override
    {
        dsp_->instanceClear();
    }
};

void setup_lfo_saw_pos_tilde()
{
    SoundExternalFactory<LfoSawPos> obj("lfo.+saw~");
    obj.addMethod("reset", &LfoSawPos::m_reset);
    obj.setXletsInfo({ "freq in hz", "bang: reset phase" }, { "saw in \\[0,+1) range" });
}
