#include "synth_dubdub.h"
#include "ceammc_factory.h"
#include "synth_faust_with_freq.h"

using namespace ceammc;
using SynthDubDub = SynthWithFreq<faust_synth_dubdub_tilde>;

void setup_synth_dubdub_tilde()
{
    SoundExternalFactory<SynthDubDub> obj("synth.dubdub~", OBJECT_FACTORY_DEFAULT);
    obj.addMethod("note", &SynthDubDub::m_note);

    obj.setDescription("simple synth based on a sawtooth wave filtered by a resonant lowpass");
    obj.setCategory("synth");
    obj.setKeywords({"dubdub", "saw"});
}
