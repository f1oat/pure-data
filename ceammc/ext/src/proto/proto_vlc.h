/*****************************************************************************
 * Copyright 2021 Serge Poltavsky. All rights reserved.
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
#ifndef PROTO_VLC_H
#define PROTO_VLC_H

#include "ceammc_object.h"
#include "ceammc_property_enum.h"
using namespace ceammc;

class ProtoVlc : public BaseObject {
    SymbolProperty* host_;
    IntProperty* port_;

public:
    ProtoVlc(const PdArgs& args);

    void m_play(t_symbol* s, const AtomListView& lv);
    void m_stop(t_symbol* s, const AtomListView& lv);
    void m_next(t_symbol* s, const AtomListView& lv);
    void m_prev(t_symbol* s, const AtomListView& lv);
    void m_pause(t_symbol* s, const AtomListView& lv);
    void m_seek(t_symbol* s, const AtomListView& lv);
    void m_loop(t_symbol* s, const AtomListView& lv);
    void m_frame(t_symbol* s, const AtomListView& lv);
    void m_repeat(t_symbol* s, const AtomListView& lv);
    void m_fullscreen(t_symbol* s, const AtomListView& lv);

    void m_add(t_symbol* s, const AtomListView& lv);
    void m_clear(t_symbol* s, const AtomListView& lv);
    void m_delete(t_symbol* s, const AtomListView& lv);
    void m_shutdown(t_symbol* s, const AtomListView& lv);

private:
    void writeCmd(const char* cmd);
};

void setup_proto_vlc();

#endif // PROTO_VLC_H
