/*****************************************************************************
 * Copyright 2017 Serge Poltavsky. All rights reserved.
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
#ifndef MSG_H
#define MSG_H

#include "ceammc_factory.h"
#include "ceammc_log.h"
#include "ceammc_object.h"

using namespace ceammc;

class Msg : public BaseObject {
    AtomList prefix_;
    AtomList data_;

public:
    Msg(const PdArgs& a);
    void onBang() final;
    void onFloat(t_float v) final;
    void onSymbol(t_symbol* s) final;
    void onList(const AtomList& l) final;
    void onAny(t_symbol* sel, const AtomListView& l) final;
    void onInlet(size_t n, const AtomList& l) final;
    bool processAnyProps(t_symbol*, const AtomListView&) final;
    void parseProperties() final;

private:
    void setMethod(const AtomList& l);
    void output();
};

void setup_base_msg();

#endif // MSG_H
