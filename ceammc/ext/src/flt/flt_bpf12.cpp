#include "flt_bpf12.h"
#include "ceammc_factory.h"

using namespace ceammc;

class FltBpf12 : public faust_flt_bpf12_tilde {
public:
    FltBpf12(const PdArgs& args)
        : faust_flt_bpf12_tilde(args)
    {
        bindPositionalArgsToProps({ gensym("@freq"), gensym("@q") });
    }

    void m_reset(t_symbol*, const AtomList&)
    {
        dsp_->instanceClear();
    }
};

void setup_flt_bpf12_tilde()
{
    SoundExternalFactory<FltBpf12> obj("flt.bpf12~");
    obj.addMethod("reset", &FltBpf12::m_reset);
}
