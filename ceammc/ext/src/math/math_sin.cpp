/// this file is autogenerated, do not edit!
#include <cmath>

#include "ceammc_containers.h"
#include "ceammc_factory.h"
#include "ceammc_object.h"
#include "ceammc_sound_external.h"

using namespace ceammc;
        
class MathSin : public BaseObject {
    using FloatUnaryFn = t_float (*)(t_float);

public:
    MathSin(const PdArgs& a)
        : BaseObject(a)
    {
        createOutlet();
    }

    void onFloat(t_float f) final
    {
        floatTo(0, std::sin(f));
    }

    void onList(const AtomListView& lv) final
    {
        auto fn = static_cast<FloatUnaryFn>(std::sin);
        SmallAtomList lst;
        lst.reserve(lv.size());
        for (auto& a : lv)
            lst.push_back(fn(a.asFloat()));

        listTo(0, lst.view());
    }
};

class MathSinTilde : public SoundExternal {
public:
    MathSinTilde(const PdArgs& a)
        : SoundExternal(a)
    {
        createSignalOutlet();
    }

    void processBlock(const t_sample** in, t_sample** out) final
    {
        const auto BS = blockSize();
        for (size_t i = 0; i < BS; i += 8) {
            out[0][i + 0] = std::sin(in[0][i + 0]);
            out[0][i + 1] = std::sin(in[0][i + 1]);
            out[0][i + 2] = std::sin(in[0][i + 2]);
            out[0][i + 3] = std::sin(in[0][i + 3]);
            out[0][i + 4] = std::sin(in[0][i + 4]);
            out[0][i + 5] = std::sin(in[0][i + 5]);
            out[0][i + 6] = std::sin(in[0][i + 6]);
            out[0][i + 7] = std::sin(in[0][i + 7]);
        }
    }
};


void setup_math_sin()
{
    ObjectFactory<MathSin> obj0("math.sin");
    SoundExternalFactory<MathSinTilde> obj1("math.sin~");
}
        
