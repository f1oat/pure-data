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
#include "ui_preset.h"
#include "ceammc_format.h"
#include "ceammc_preset.h"
#include "ceammc_ui.h"

UIPreset::UIPreset()
    : prop_color_text(rgba_black)
    , prop_color_empty(rgba_grey)
    , prop_color_stored(rgba_greydark)
    , prop_color_active(rgba_blue)
    , button_size_(10)
    , selected_index_(-1)
    , mouse_over_index_(-1)
{
}

void UIPreset::init(t_symbol* name, const AtomList& args, bool usePresets)
{
    UIObject::init(name, args, usePresets);
    bindTo(gensym(PresetStorage::SYM_PRESET_UPDATE_INDEX_ADDR));
    updateIndexes();
}

void UIPreset::okSize(t_rect* newrect)
{
    newrect->width = pd_clip_min(newrect->width, 15.);
    newrect->height = pd_clip_min(newrect->height, 15.);
}

void UIPreset::paint(t_object* view)
{
#ifdef __APPLE__
    const static int FNT_SZ = FONT_SIZE_SMALL;
#elif _WINDOWS
    const static int FNT_SZ = FONT_SIZE_SMALL + 2;
#else
    const static int FNT_SZ = FONT_SIZE_SMALL + 2;
#endif

    // NO multithreading!!!
    static std::vector<UITextLayout*> numbers_;
    static UIFont font(gensym(FONT_FAMILY), FNT_SZ);

    if (numbers_.empty()) {
        for (size_t i = 0; i < 256; i++)
            numbers_.push_back(new UITextLayout(font.font(), ColorRGBA::black(),
                ETEXT_CENTER, ETEXT_JCENTER, ETEXT_NOWRAP));
    }

    const t_rect& r = rect();
    UIPainter p = bg_layer_.painter(r);

    if (!p)
        return;

    button_size_ = FNT_SZ * zoom() * 2.5;
    int btn_x = button_size_ / 2;
    int btn_y = button_size_ / 2;

    char number[256];
    t_rgba color;

    for (int i = 0; i < presets_.size(); i++) {
        if (btn_y + button_size_ * 0.25 >= r.height)
            break;

        if (presets_[i]) {
            if (selected_index_ == i)
                color = rgba_addContrast(prop_color_active, 0.1f);
            else
                color = rgba_addContrast(prop_color_stored, -0.1f);
        } else
            color = rgba_addContrast(prop_color_empty, 0.1f);

        // on mouse over
        if (mouse_over_index_ == i)
            color = rgba_addContrast(color, 0.08f);

        // draw button
        p.setColor(color);
        p.drawCircle(btn_x, btn_y, button_size_ * 0.4);
        p.fill();

        // draw button text
        sprintf(number, "%i", i + 1);
        numbers_[i]->setColor(prop_color_text);
        numbers_[i]->set(number, btn_x, btn_y, 40, 0);
        p.drawText(*numbers_[i]);

        btn_x += button_size_;
        if (btn_x + button_size_ * 0.25 > r.width) {
            // start next row
            btn_x = button_size_ / 2;
            btn_y += button_size_;
        }
    }
}

void UIPreset::onMouseDown(t_object* view, const t_pt& pt, long modifiers)
{
    int index = buttonIndexAt(pt.x, pt.y);

    if (index < 0 || index >= presets_.size())
        return;

    if (modifiers == EMOD_ALT)
        clearIndex(index);
    else if (modifiers == EMOD_SHIFT)
        storeIndex(index);
    else
        loadIndex(index);
}

void UIPreset::onMouseMove(t_object* view, const t_pt& pt, long modifiers)
{
    int index = buttonIndexAt(pt.x, pt.y);

    if (mouse_over_index_ != index) {
        mouse_over_index_ = index;
        redrawBGLayer();
    }
}

void UIPreset::onMouseLeave(t_object* view, const t_pt& pt, long modifiers)
{
    mouse_over_index_ = -1;
    redrawBGLayer();
}

int UIPreset::buttonIndexAt(float x, float y) const
{
    float n;
    const float rem = modff(width() / button_size_, &n);
    const int num_cols = n + (rem >= 0.75);
    const int col = std::min<int>(x / button_size_, num_cols - 1);
    const int row = int(y / button_size_);
    return row * num_cols + col;
}

void UIPreset::m_read(const AtomList& lst)
{
    if (PresetStorage::instance().read(canvas(), to_string(lst))) {
        selected_index_ = -1;
        redrawBGLayer();
    }
}

void UIPreset::m_write(const AtomList& lst)
{
    PresetStorage::instance().write(canvas(), to_string(lst));
}

void UIPreset::m_load(const AtomList& lst)
{
    loadIndex(lst.floatAt(0, 0));
}

void UIPreset::m_store(const AtomList& lst)
{
    storeIndex(lst.floatAt(0, 0));
}

void UIPreset::m_clear(const AtomList& lst)
{
    clearIndex(lst.floatAt(0, 0));
}

AtomList UIPreset::propCurrent() const
{
    return Atom(selected_index_);
}

void UIPreset::setup()
{
    UIObjectFactory<UIPreset> obj("ui.preset");

#ifdef __WIN32
    obj.setDefaultSize(102, 42);
#else
    obj.setDefaultSize(102, 42);
#endif

    obj.hideProperty("send");

    obj.addProperty("text_color", _("Text Color"), "0. 0. 0. 1.", &UIPreset::prop_color_text);
    obj.addProperty("empty_color", _("Empty Button Color"), "0.86 0.86 0.86 1.", &UIPreset::prop_color_empty);
    obj.addProperty("stored_color", _("Stored Button Color"), "0.5 0.5 0.5 1.", &UIPreset::prop_color_stored);
    obj.addProperty(PROP_ACTIVE_COLOR, _("Active Color"), DEFAULT_ACTIVE_COLOR, &UIPreset::prop_color_active);
    obj.addProperty("current", &UIPreset::propCurrent, 0);

    obj.useMouseEvents(UI_MOUSE_DOWN | UI_MOUSE_MOVE | UI_MOUSE_LEAVE);
    obj.addMethod(PresetStorage::SYM_PRESET_INDEX_ADD, &UIPreset::indexAdd);
    obj.addMethod(PresetStorage::SYM_PRESET_INDEX_REMOVE, &UIPreset::indexRemove);

    obj.addMethod("read", &UIPreset::m_read);
    obj.addMethod("write", &UIPreset::m_write);
    obj.addMethod("clear", &UIPreset::m_clear);
    obj.addMethod("load", &UIPreset::m_load);
    obj.addMethod("store", &UIPreset::m_store);
}

void UIPreset::indexAdd(const AtomList& lst)
{
    float f;
    if (lst.size() == 1 && lst[0].getFloat(&f)) {
        size_t idx = f;
        if (idx >= presets_.size()) {
            UI_ERR << "invalid preset index: " << idx;
            return;
        }

        presets_.set(idx, true);
        redrawBGLayer();
    }
}

void UIPreset::indexRemove(const AtomList& lst)
{
    float f;
    if (lst.size() == 1 && lst[0].getFloat(&f)) {
        size_t idx = f;
        if (idx >= presets_.size()) {
            UI_ERR << "invalid preset index: " << idx;
            return;
        }

        presets_.set(idx, false);
        redrawBGLayer();
    }
}

void UIPreset::updateIndexes()
{
    PresetStorage& s = PresetStorage::instance();
    for (size_t i = 0; i < presets_.size(); i++)
        presets_.set(i, s.hasIndex(i));
}

void UIPreset::loadIndex(size_t idx)
{
    if (idx >= presets_.size()) {
        UI_ERR << "Invalid preset index: " << idx;
        return;
    }

    if (presets_.test(idx)) {
        selected_index_ = idx;
        PresetStorage::instance().loadAll(idx);
        redrawBGLayer();
    }
}

void UIPreset::storeIndex(size_t idx)
{
    if (idx >= presets_.size()) {
        UI_ERR << "Invalid preset index: " << idx;
        return;
    }

    PresetStorage::instance().storeAll(idx);
    presets_.set(idx, true);
    selected_index_ = idx;
}

void UIPreset::clearIndex(size_t idx)
{
    if (idx >= presets_.size()) {
        UI_ERR << "Invalid preset index: " << idx;
        return;
    }

    if (presets_.test(idx)) {
        if (selected_index_ == idx)
            selected_index_ = -1;

        PresetStorage::instance().clearAll(idx);
        presets_.set(idx, false);
        redrawBGLayer();
    }
}

void setup_ui_preset()
{
    UIPreset::setup();
}
