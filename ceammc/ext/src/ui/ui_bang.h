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
#ifndef UI_BANG_H
#define UI_BANG_H

#include "ceammc_clock.h"
#include "ceammc_ui_object.h"

using namespace ceammc;

class UIBang : public UIObject {
    ClockMemberFunction<UIBang> clock_;
    bool active_;

private:
    t_rgba prop_color_active;

public:
    UIBang();

    void okSize(t_rect* newrect);
    void paint();

    void onMouseDown(t_object* view, const t_pt& pt, const t_pt& abs_pt, long modifiers);
    void onMouseUp(t_object* view, const t_pt& pt, long modifiers);
    void onAny(t_symbol* s, const AtomList& lst);

private:
    void activate();
    void deactivate();

public:
    static void setup();
};

void setup_ui_bang();

#endif // UI_BANG_H
