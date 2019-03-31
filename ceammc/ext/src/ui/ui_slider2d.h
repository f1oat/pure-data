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
#ifndef UI_SLIDER2D_H
#define UI_SLIDER2D_H

#include "ceammc_ui_object.h"

using namespace ceammc;

class UISlider2D : public UIObject {
    float prop_x_min;
    float prop_x_max;
    float prop_y_min;
    float prop_y_max;
    int prop_show_range;
    int prop_show_grid;

private:
    UIFont txt_font;
    UITextLayout txt_xrange_;
    UITextLayout txt_yrange_;
    UILayer knob_layer_;
    float x_pos_;
    float y_pos_;
    bool mouse_down_;

public:
    UISlider2D();

    void okSize(t_rect* newrect);
    void paint(t_object* view);
    t_pd_err notify(t_symbol* attr_name, t_symbol* msg);
    void paintBackground();
    void paintKnob();

    void onBang();
    void onList(const AtomList& lst);
    void onMouseDown(t_object* view, const t_pt& pt, const t_pt& abs_pt, long modifiers);
    void onMouseDrag(t_object* view, const t_pt& pt, long modifiers);
    void onMouseUp(t_object* view, const t_pt& pt, long modifiers);

    void m_set(const AtomList& lst);
    void m_move(const AtomList& lst);
    void loadPreset(size_t idx);
    void storePreset(size_t idx);

    bool setRealValue(const AtomList& lst);
    AtomList realValue() const;
    t_float realXValue() const;
    t_float realYValue() const;

    t_float xRange() const { return prop_x_max - prop_x_min; }
    t_float yRange() const { return prop_y_max - prop_y_min; }

    void output();

    AtomList propXRange() const;
    AtomList propYRange() const;

    AtomList propXValue() const;
    AtomList propYValue() const;
    void propSetXValue(const AtomList& lst);
    void propSetYValue(const AtomList& lst);

public:
    static void setup();

    void setMouse(float x, float y);
    void updateLabels();

private:
    void redrawKnob();
    void redrawAll();
};

void setup_ui_slider2d();

#endif // UI_SLIDER2D_H
