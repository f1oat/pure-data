#ifndef UI_RADIO_H
#define UI_RADIO_H

#include "ceammc_ui_object.h"

#include <bitset>

using namespace ceammc;

typedef std::bitset<128> BitSet;

class UIRadio : public UIObject {
    BitSet items_;
    int idx_;

    int prop_nitems_;
    int prop_checklist_mode_;
    t_rgba prop_color_active;
    UILayer items_layer_;

public:
    UIRadio();

    void init(t_symbol* name, const AtomList& args, bool usePresets);

    int singleValue() const;
    void setSingleValue(int idx);
    AtomList listValue() const;
    void setListValue(const AtomList& lst);

    void output();
    bool isVertical() const { return width() < height(); }

    void paint();
    void drawBackground();
    void drawItems();
    void okSize(t_rect* newrect);
    void redrawAll();
    void redrawItems();
    void onPropChange(t_symbol* prop_name);

    void onBang();
    void onFloat(t_float f);
    void onList(const AtomList& lst);
    void onMouseDown(t_object*, const t_pt& pt, const t_pt& abs_pt, long mod);
    void onDblClick(t_object* view, const t_pt& pt, long modifiers);
    void loadPreset(size_t idx);
    void storePreset(size_t idx);

    void onPopup(t_symbol* menu_name, long item_idx, const t_pt& pt);
    void showPopup(const t_pt& pt, const t_pt& abs_pt);

    // props
    float p_numItems() const;
    void p_setNumItems(float n);
    float p_mode() const;
    void p_setMode(float p_mode);
    AtomList p_value() const;
    void p_setValue(const AtomList& lst);

    // methods
    void m_flip();
    void m_reset();
    void m_next();
    void m_prev();
    void m_random();
    void m_plus(t_float f);
    void m_minus(t_float f);

public:
    static void setup();
};

void setup_ui_radio();

#endif // UI_RADIO_H
