/*****************************************************************************
 * Copyright 2022 Serge Poltavsky. All rights reserved.
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
#ifndef MIDI_SPLIT_H
#define MIDI_SPLIT_H

#include <string>

#include "ceammc_object.h"
using namespace ceammc;

class MidiSplit : public BaseObject {
    std::vector<t_float> split_points_;
    std::vector<std::string> split_tooltips_;

public:
    MidiSplit(const PdArgs& args);

    void onList(const AtomListView& lv) override;
    const char* annotateOutlet(size_t n) const override;
};

void setup_midi_split();

#endif // MIDI_SPLIT_H
