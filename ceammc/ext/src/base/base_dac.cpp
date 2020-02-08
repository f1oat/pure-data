/*****************************************************************************
 * Copyright 2019 Serge Poltavsky. All rights reserved.
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
#include "base_dac.h"
#include "ceammc_factory.h"

#include <algorithm>
#include <regex>

extern "C" {
#include "s_stuff.h"
}

#define OBJ_NAME "xdac~"
#define DEFDACBLKSIZE 64

static std::vector<t_int> parseRange(const std::string& str)
{
    std::vector<t_int> res;
    const std::regex re("([0-9]+):([0-9]+)");
    std::smatch match;
    try {
        if (std::regex_match(str, match, re)) {
            if (match.size() == 3) {
                int v0 = std::stoi(match.str(1));
                int v1 = std::stoi(match.str(2));
                auto p = std::minmax(v0, v1);

                if (v0 == v1)
                    return res;

                for (int i = p.first; i <= p.second; i++)
                    res.push_back(i);
            }
        } else {
            return res;
        }
    } catch (std::exception& e) {
        LIB_ERR << e.what();
    } catch (...) {
        LIB_ERR << "unknown exception";
    }

    return res;
}

BaseDac::BaseDac(const PdArgs& args)
    : SoundExternal(args)
{
    auto& pos_args = positionalArguments();
    if (pos_args.empty()) {
        vec_.assign({ 1, 2 });
    } else {
        for (const Atom& a : pos_args) {
            if (a.isFloat()) {
                vec_.push_back(a.asInt());
                continue;
            } else if (a.isSymbol()) {
                auto rng = parseRange(a.asSymbol()->s_name);
                if (rng.empty()) {
                    OBJ_ERR << "invalid range: " << a;
                    continue;
                }

                std::copy(rng.begin(), rng.end(), std::back_inserter(vec_));
            } else {
                OBJ_ERR << "invalid argument: " << a;
            }
        }
    }

    for (size_t i = 1; i < vec_.size(); i++)
        createSignalInlet();
}

void BaseDac::processBlock(const t_sample** in, t_sample** out)
{
    // should be empty
}

void BaseDac::setupDSP(t_signal** sp)
{
    const int NOUTS = sys_get_outchannels();

    for (size_t i = 0; i < vec_.size(); i++) {
        t_signal* sig = sp[i];
        if (sig->s_n != DEFDACBLKSIZE) {
            error(OBJ_NAME ": bad vector size");
            break;
        }

        int ch = vec_[i] - 1;
        if (ch >= 0 && ch < NOUTS) {
            auto in = sig->s_vec;
            auto out = get_sys_soundout() + (DEFDACBLKSIZE * ch);

            dsp_add_plus(in, out, out, DEFDACBLKSIZE);
        } else
            OBJ_LOG << "invalid channel number: " << (ch + 1);
    }
}

void BaseDac::onClick(t_floatarg xpos, t_floatarg ypos, t_floatarg shift, t_floatarg ctrl, t_floatarg alt)
{
    sys_gui("pdsend \"pd audio-properties\"\n");
}

void setup_base_dac()
{
    SoundExternalFactory<BaseDac> obj(OBJ_NAME);
    obj.useClick();
}
