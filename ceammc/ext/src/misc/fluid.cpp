/*****************************************************************************
 * Copyright 2018 Serge Poltavsky. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#include "fluid.h"
#include "ceammc_factory.h"
#include "ceammc_platform.h"

#include "fluidsynth.h"
#include "src/sfloader/fluid_sfont.h"

Fluid::Fluid(const PdArgs& args)
    : SoundExternal(args)
    , synth_(nullptr)
    , sound_font_(&s_)
{
    createSignalOutlet();
    createSignalOutlet();

    fluid_settings_t* settings = new_fluid_settings();
    if (settings == nullptr) {
        OBJ_ERR << "couldn't create synth settings";
        return;
    }

    // settings:
    fluid_settings_setnum(settings, "synth.midi-channels", 16);
    fluid_settings_setnum(settings, "synth.polyphony", 256);
    fluid_settings_setnum(settings, "synth.gain", 0.6);
    fluid_settings_setnum(settings, "synth.sample-rate", 44100);
    fluid_settings_setstr(settings, "synth.chorus.active", "no");
    fluid_settings_setstr(settings, "synth.reverb.active", "no");
    fluid_settings_setstr(settings, "synth.ladspa.active", "no");

    // Create fluidsynth instance:
    synth_ = new_fluid_synth(settings);

    if (synth_ == nullptr)
        OBJ_ERR << "couldn't create synth";

    createCbProperty("@sf", &Fluid::propSoundFont, &Fluid::propSetSoundFont);
    createCbProperty("@version", &Fluid::propVersion);
    createCbProperty("@soundfonts", &Fluid::propSoundFonts);
}

Fluid::~Fluid()
{
    if (synth_) {
        fluid_settings_t* settings = fluid_synth_get_settings(synth_);
        delete_fluid_settings(settings);
    }

    delete_fluid_synth(synth_);
}

void Fluid::onList(const AtomList& lst)
{
    m_note(&s_, lst);
}

void Fluid::setupDSP(t_signal** sp)
{
    SoundExternal::setupDSP(sp);

    if (synth_) {
        fluid_settings_t* s = fluid_synth_get_settings(synth_);
        if (fluid_settings_setnum(s, "synth.sample-rate", samplerate()) != FLUID_OK) {
            OBJ_ERR << "can't set samplerate: " << samplerate();
        }
    }
}

AtomList Fluid::propSoundFont() const
{
    return Atom(sound_font_);
}

void Fluid::propSetSoundFont(const AtomList& lst)
{
    if (!synth_) {
        OBJ_ERR << "NULL synth";
        return;
    }

    if (!checkArgs(lst, ARG_SYMBOL)) {
        OBJ_ERR << "path to soundfont expected: " << lst;
        return;
    }

    const char* fn = lst.symbolAt(0, &s_)->s_name;
    std::string filename = findInStdPaths(fn);

    if (filename.empty()) {
        filename = platform::find_in_exernal_dir(owner(), fn);

        if (filename.empty()) {
            OBJ_ERR << "sound font is not found: " << lst;
            return;
        }
    }

    if (fluid_synth_sfload(synth_, filename.c_str(), 0) >= 0) {

        OBJ_DBG << "loaded soundfont: " << filename;
        fluid_synth_program_reset(synth_);

        sound_font_ = lst.symbolAt(0, &s_);
    } else {
        OBJ_ERR << "can't load soundfont: " << lst;
    }
}

AtomList Fluid::propVersion() const
{
    return Atom(gensym(FLUIDSYNTH_VERSION));
}

AtomList Fluid::propSoundFonts() const
{
    AtomList res;
    const int N = fluid_synth_sfcount(synth_);

    for (int i = 0; i < N; i++) {
        fluid_sfont_t* sf = fluid_synth_get_sfont(synth_, i);
        const char* name = fluid_sfont_get_name(sf);
        res.append(Atom(gensym(name)));
    }

    return res;
}

void Fluid::m_note(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    if (lst.size() == 3 && lst[0].isFloat() && lst[1].isFloat() && lst[2].isFloat()) {
        int chan = lst[0].asInt();
        int key = lst[1].asInt();
        int vel = lst[2].asInt();

        fluid_synth_noteon(synth_, chan - 1, key, vel);
    } else if (lst.size() == 2 && lst[0].isFloat() && lst[1].isFloat()) {
        int key = lst[0].asInt();
        int vel = lst[1].asInt();
        fluid_synth_noteon(synth_, 0, key, vel);
    } else {
        METHOD_ERR(s) << "CHAN KEY VEL or KEY VEL expected: " << lst;
    }
}

void Fluid::m_cc(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    if (lst.size() == 3 && lst[0].isFloat() && lst[1].isFloat() && lst[2].isFloat()) {
        int chan = lst[0].asInt();
        int key = lst[1].asInt();
        int vel = lst[2].asInt();

        fluid_synth_cc(synth_, chan - 1, key, vel);
    } else if (lst.size() == 2 && lst[0].isFloat() && lst[1].isFloat()) {
        int key = lst[0].asInt();
        int vel = lst[1].asInt();
        fluid_synth_cc(synth_, 0, key, vel);
    } else {
        METHOD_ERR(s) << "CHAN KEY VAL or KEY VAL expected: " << lst;
    }
}

void Fluid::m_prog(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    if (lst.size() == 2 && lst[0].isFloat() && lst[1].isFloat()) {
        int chan = lst[0].asInt();
        int program = lst[1].asInt();
        fluid_synth_program_change(synth_, chan - 1, program);
    } else {
        METHOD_ERR(s) << "CHAN PROGRAM expected: " << lst;
    }
}

void Fluid::m_bank(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    if (lst.size() == 2 && lst[0].isFloat() && lst[1].isFloat()) {
        int chan = lst[0].asInt();
        int bank = lst[1].asInt();
        fluid_synth_bank_select(synth_, chan - 1, bank);

        unsigned int sf_id;
        unsigned int bank_num;
        unsigned int prog_num;

        fluid_synth_get_program(synth_, chan - 1, &sf_id, &bank_num, &prog_num);
        fluid_synth_program_change(synth_, chan - 1, prog_num);
    } else {
        METHOD_ERR(s) << "CHAN BANK expected: " << lst;
    }
}

void Fluid::m_bend(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    if (lst.size() == 2 && lst[0].isFloat() && lst[1].isFloat()) {
        int chan = lst[0].asInt();
        int val = lst[1].asInt();
        fluid_synth_pitch_bend(synth_, chan - 1, val);
    } else {
        METHOD_ERR(s) << "CHAN VAL expected: " << lst;
    }
}

void Fluid::m_gen(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    if (lst.size() == 3 && lst[0].isFloat() && lst[1].isFloat() && lst[2].isFloat()) {
        int chan = lst[0].asInt();
        int param = lst[1].asInt();
        int value = lst[2].asInt();

        fluid_synth_set_gen(synth_, chan - 1, param, value);
    } else if (lst.size() == 2 && lst[0].isFloat() && lst[1].isFloat()) {
        int param = lst[0].asInt();
        int value = lst[1].asInt();
        fluid_synth_set_gen(synth_, 0, param, value);
    } else {
        METHOD_ERR(s) << "CHAN PARAM VAL or PARAM VAL expected: " << lst;
    }
}

void Fluid::m_panic(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    fluid_synth_system_reset(synth_);
}

void Fluid::m_reset(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    int n = fluid_synth_count_midi_channels(synth_);

    for (int i = 0; i < n; i++)
        fluid_synth_reset_basic_channel(synth_, i);
}

void Fluid::m_notesOff(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    int chan = lst.floatAt(0, 0);
    fluid_synth_all_notes_off(synth_, chan - 1);
}

void Fluid::m_soundsOff(t_symbol* s, const AtomList& lst)
{
    if (synth_ == nullptr)
        return;

    int chan = lst.floatAt(0, 0);
    fluid_synth_all_sounds_off(synth_, chan - 1);
}

void Fluid::dump() const
{
    SoundExternal::dump();

    const int N = fluid_synth_sfcount(synth_);

    OBJ_DBG << " presets:";
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 128; j++) {
            fluid_preset_t* preset = NULL;
            fluid_sfont_t* sf = NULL;

            for (int k = 0; k < N; k++) {
                sf = fluid_synth_get_sfont(synth_, k);
                preset = fluid_sfont_get_preset(sf, i, j);

                if (preset != NULL)
                    break;
            }

            if (preset != NULL) {
                const char* sf_name = fluid_sfont_get_name(sf);
                const char* preset_name = fluid_preset_get_name(preset);

                post("  '%s': soundfont '%s', bank %d, program %d",
                    preset_name, sf_name, i, j);
            }
        }
    }

    int NMIDI = fluid_synth_count_midi_channels(synth_);
    OBJ_DBG << " channels:";

    for (int i = 0; i < NMIDI; i++) {
        fluid_preset_t* preset = fluid_synth_get_channel_preset(synth_, i);

        if (preset != NULL) {
            const char* preset_name = fluid_preset_get_name(preset);
            unsigned int sf_id;
            unsigned int bank_num;
            unsigned int prog_num;
            fluid_sfont_t* sf;

            fluid_synth_get_program(synth_, i, &sf_id, &bank_num, &prog_num);
            sf = fluid_synth_get_sfont_by_id(synth_, sf_id);

            post("  %d: soundfont '%s', bank %d, program %d: '%s'",
                i + 1, fluid_sfont_get_name(sf), bank_num, prog_num, preset_name);
        } else
            post("  channel %d: no preset", i + 1);
    }

    OBJ_DBG << "gain: " << fluid_synth_get_gain(synth_);
}

void Fluid::processBlock(const t_sample** in, t_sample** out)
{
    if (synth_ == nullptr)
        return;

    float* left = out[0];
    float* right = out[1];
    fluid_synth_write_float(synth_, blockSize(), left, 0, 1, right, 0, 1);
}

void setup_misc_fluid()
{
    SoundExternalFactory<Fluid> obj("fluid~", OBJECT_FACTORY_DEFAULT);

    obj.addMethod("note", &Fluid::m_note);
    obj.addMethod("cc", &Fluid::m_cc);
    obj.addMethod("prog", &Fluid::m_prog);
    obj.addMethod("bank", &Fluid::m_bank);
    obj.addMethod("bend", &Fluid::m_bend);
    obj.addMethod("gen", &Fluid::m_gen);
    obj.addMethod("panic", &Fluid::m_panic);
    obj.addMethod("reset", &Fluid::m_reset);
    obj.addMethod("notes_off", &Fluid::m_notesOff);
    obj.addMethod("sounds_off", &Fluid::m_soundsOff);
}
