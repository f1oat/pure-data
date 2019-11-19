#ifndef UI_KNOB_H
#define UI_KNOB_H

#include "ui_single_value.h"

using namespace ceammc;

class UIKnob : public UISingleValue {
    UIFont txt_font;
    UITextLayout txt_min;
    UITextLayout txt_max;

    int show_range_;
    int draw_active_scale_;
    t_pt click_pos_;

public:
    t_rgba prop_knob_color;
    t_rgba prop_scale_color;

    static void setup();

public:
    UIKnob();

    void paint();
    void okSize(t_rect* newrect);
    void onMouseUp(t_object* view, const t_pt& pt, long modifiers);
    void onMouseDrag(t_object*, const t_pt& pt, long modifiers);
    void onMouseDown(t_object*, const t_pt& pt, const t_pt& abs_pt, long modifiers);
    void onMouseWheel(const t_pt& pt, long modifiers, double delta);
    void onPopup(t_symbol* menu_name, long item_idx, const t_pt &pt);
    void showPopup(const t_pt& pt, const t_pt& abs_pt);
};

void setup_ui_knob();

#endif // UI_KNOB_H
