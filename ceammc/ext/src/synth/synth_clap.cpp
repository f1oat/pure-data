#include "synth_clap.h"
#include "ceammc_clock.h"
#include "ceammc_convert.h"
#include "ceammc_factory.h"

using namespace ceammc;

constexpr t_float MIN_DUR = 10;
constexpr t_float MAX_DUR = 1000;
constexpr t_float DEFAULT_DUR = 200;

class SynthClap : public faust_synth_clap_tilde {
    UIProperty* gate_;
    UIProperty* attack_;
    UIProperty* release_;
    BoolProperty* env_wait_;
    ClockLambdaFunction gate_timer_;
    ClockLambdaFunction release_timer_;

public:
    SynthClap(const PdArgs& args)
        : faust_synth_clap_tilde(args)
        , gate_(findProperty("@gate"))
        , attack_(findProperty("@attack"))
        , release_(findProperty("@release"))
        , env_wait_(nullptr)
        , gate_timer_([this]() {
            gate_->setValue(0);
            // go to release phase
            release_timer_.delay(release_->value());
        })
        , release_timer_([this]() { bangTo(1); })
    {
        createOutlet();

        env_wait_ = new BoolProperty("@envwait", false);
        addProperty(env_wait_);

        gate_->setSuccessFn([this](Property*) {
            if (gate_->value() == 0) {
                // go to release phase
                release_timer_.delay(release_->value());
            } else if (release_timer_.isActive())
                release_timer_.unset();
        });
    }

    void onBang() override
    {
        if (isBusy())
            return;

        play(1);
    }

    void onFloat(t_float gain) override
    {
        if (isBusy())
            return;

        play(gain);
    }

    void onList(const AtomListView& lv) override
    {
        const auto N = lv.size();
        const bool ok = (N >= 1 && N <= 2) && lv[0].isFloat();
        if (!ok) {
            OBJ_ERR << "usage: GAIN[0..1] DUR[ms]?";
            return;
        }

        if (isBusy())
            return;

        const auto gain = lv[0].asT<t_float>();

        if (N == 2) {
            const auto rel = clip<t_float>(lv[1].asFloat(DEFAULT_DUR) - attackPhaseMs(), MIN_DUR, MAX_DUR);
            release_->setValue(rel);
        }

        play(gain);
    }

private:
    UIProperty* findProperty(const char* name)
    {
        return static_cast<UIProperty*>(property(gensym(name)));
    }

    t_float attackPhaseMs() const
    {
        return std::ceil(attack_->value() + 4);
    }

    bool isBusy() const
    {
        return env_wait_->value() && (gate_timer_.isActive() || release_timer_.isActive());
    }

    void play(t_float gain)
    {
        if (gain <= 0)
            return;

        gate_->setValue(gain);
        gate_timer_.delay(attackPhaseMs());
        if (release_timer_.isActive())
            release_timer_.unset();
    }
};

void setup_synth_clap_tilde()
{
    SoundExternalFactory<SynthClap> obj("synth.clap~", OBJECT_FACTORY_DEFAULT);
    obj.addMethod("reset", &SynthClap::m_reset);

    obj.setXletsInfo({ "bang: play clap\n"
                       "float: play with GAIN\n"
                       "list: play with GAIN DUR" },
        { "audio: output", "bang: on finish" });

    obj.setDescription("clap synth from faust libraries");
    obj.setCategory("synth");
    obj.setKeywords({"synth", "kick"});
}
