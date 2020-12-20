/// this file is autogenerated, do not edit!
#include <cmath>

#include "ceammc_factory.h"
#include "ceammc_object.h"
#include "ceammc_sound_external.h"

using namespace ceammc;
        

class MathAsinh : public BaseObject
{
    using FloatUnaryFn = t_float(*)(t_float);
public:
    MathAsinh(const PdArgs& a) : BaseObject(a)
    {
        createOutlet();
    }

    void onFloat(t_float f) final
    {
        floatTo(0, std::asinh(f));
    }

    void onList(const AtomList& l) final
    {
        listTo(0, l.mapFloat(static_cast<FloatUnaryFn>(std::asinh)));
    }

};


class MathAsinhTilde : public SoundExternal
{
public:
    MathAsinhTilde(const PdArgs& a) : SoundExternal(a)
    {
        createSignalOutlet();
    }

    void processBlock(const t_sample** in, t_sample** out) final
    {
        const auto BS = blockSize();
        for(size_t i = 0; i < BS; i += 8) {
            out[0][i+0] = std::asinh(in[0][i+0]);
            out[0][i+1] = std::asinh(in[0][i+1]);
            out[0][i+2] = std::asinh(in[0][i+2]);
            out[0][i+3] = std::asinh(in[0][i+3]);
            out[0][i+4] = std::asinh(in[0][i+4]);
            out[0][i+5] = std::asinh(in[0][i+5]);
            out[0][i+6] = std::asinh(in[0][i+6]);
            out[0][i+7] = std::asinh(in[0][i+7]);
        }
    }
};


void setup_math_asinh() {
    ObjectFactory<MathAsinh> obj0("math.asinh");
    SoundExternalFactory<MathAsinhTilde> obj1("math.asinh~");
}
        
