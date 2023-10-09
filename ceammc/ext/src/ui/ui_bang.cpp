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
#include "ui_bang.h"
#include "ceammc_ui.h"
#include "ui_bang.tcl.h"

constexpr int FLASH_TIMEOUT = 100;

namespace ceammc {

UIBang::UIBang()
    : UIBindObject<1>({ [this]() { bang(); } })
    , clock_([this]() { if(active_) deactivate(); })
    , active_(false)
    , prop_color_active(rgba_blue)
{
    createOutlet();
}

void UIBang::okSize(t_rect* newrect)
{
    newrect->w = pd_clip_min(newrect->w, 8);
    newrect->h = pd_clip_min(newrect->h, 8);
}

void UIBang::paint()
{
    sys_vgui("::ui::bang_update %s %lx %d %d %d #%6.6x %d\n",
        asEBox()->b_canvas_id->s_name, asEBox(),
        (int)width(), (int)height(), (int)zoom(),
        active_ ? rgba_to_hex_int(prop_color_active)
                : rgba_to_hex_int(prop_color_border),
        (int)active_);
}

void UIBang::onMouseDown(t_object* view, const t_pt& pt, const t_pt& abs_pt, long modifiers)
{
    bang();
}

void UIBang::onMouseUp(t_object* view, const t_pt& pt, long modifiers)
{
    if (active_)
        deactivate();
}

void UIBang::onAny(t_symbol* s, const AtomListView&)
{
    bang();
}

void UIBang::activate()
{
    active_ = true;
    bg_layer_.invalidate();
    redraw();
    bangTo(0);
    sendBang();
}

void UIBang::deactivate()
{
    active_ = false;
    bg_layer_.invalidate();
    redraw();
}

void UIBang::bang()
{
    activate();
    clock_.delay(FLASH_TIMEOUT);
}

void UIBang::setup()
{
    ui_bang_tcl_output();

    UIObjectFactory<UIBang> obj("ui.bang", EBOX_GROWLINK);
    obj.addAlias("ui.b");

    obj.setDefaultSize(15, 15);
    obj.addProperty(PROP_ACTIVE_COLOR, _("Active Color"), DEFAULT_ACTIVE_COLOR, &UIBang::prop_color_active);

    obj.useMouseEvents(UI_MOUSE_DOWN | UI_MOUSE_UP);
    obj.useAny();

    obj.addVirtualProperty("bind", _("Bind"), "", &UIBang::getBind<0>, &UIBang::setBind<0>);
    obj.setPropertyCategory("bind", "Main");
}

}

void setup_ui_bang()
{
    ceammc::UIBang::setup();
}
