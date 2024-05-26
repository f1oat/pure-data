/*****************************************************************************
 * Copyright 2023 Serge Poltavski. All rights reserved.
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
#ifndef RHYTHM_HEXBEAT_H
#define RHYTHM_HEXBEAT_H

#include "ceammc_object.h"
using namespace ceammc;

class RhythmHexBeat : public BaseObject {
    SymbolProperty* hex_;
    AtomList pattern_;

public:
    RhythmHexBeat(const PdArgs& args);
    void onBang() override;
    void onSymbol(t_symbol* s) override;
    void onList(const AtomListView& lv) override;
    void onAny(t_symbol* s, const AtomListView& lv) override;

    void onInlet(size_t n, const AtomListView& lv) override;

private:
    void updatePattern();
};

void setup_rhythm_hexbeat();

#endif // RHYTHM_HEXBEAT_H