#include "ui_radio.h"
#include "ceammc_convert.h"
#include "ceammc_preset.h"
#include "ceammc_ui.h"

#include <cassert>
#include <chrono>
#include <random>

static const int MAX_ITEMS = 128;
static const char* MENU_NAME_CHECKLIST = "checklist-menu";
static t_symbol* SYM_PROP_NITEMS;

void setup_ui_radio()
{
    UIRadio::setup();
}

UIRadio::UIRadio()
    : items_()
    , idx_(0)
    , prop_nitems_(8)
    , prop_checklist_mode_(0)
    , prop_color_active(hex_to_rgba(DEFAULT_ACTIVE_COLOR))
    , items_layer_(asEBox(), gensym("items_layer"))
{
    createOutlet();
}

void UIRadio::init(t_symbol* name, const AtomList& args, bool usePresets)
{
    UIObject::init(name, args, usePresets);

    if (name == gensym("ui.vrd"))
        std::swap(asEBox()->b_rect.width, asEBox()->b_rect.height);

    // has positional arguments
    int n = args.intAt(0, -1);
    if (n > 0) {
        prop_nitems_ = clip<int>(n, 2, MAX_ITEMS);
        const int dim1 = 15;
        const int dim2 = ((dim1 + 1) * prop_nitems_) - 1;
        if (isVertical()) {
            asEBox()->b_rect.width = dim1;
            asEBox()->b_rect.height = dim2;
        } else {
            asEBox()->b_rect.width = dim2;
            asEBox()->b_rect.height = dim1;
        }
    } else {
        const size_t N = args.size();
        for (size_t i = 0; i < N; i++) {
            const auto& a = args[i];
            if (!a.isProperty())
                continue;

            if (a.asSymbol() != SYM_PROP_NITEMS)
                continue;

            size_t inext = i + 1;
            if (inext >= N)
                break;

            prop_nitems_ = clip<int>(args[inext].asInt(0), 2, MAX_ITEMS);
            int h = 15;
            int w = ((h + 1) * prop_nitems_) - 1;
            if (isVertical())
                std::swap(h, w);

            asEBox()->b_rect.width = w;
            asEBox()->b_rect.height = h;
            break;
        }
    }
}

int UIRadio::singleValue() const
{
    return idx_;
}

void UIRadio::setSingleValue(int idx)
{
    idx_ = clip<int>(idx, 0, prop_nitems_ - 1);
}

void UIRadio::onBang()
{
    output();
}

void UIRadio::onFloat(t_float f)
{
    if (prop_checklist_mode_) {
        UI_ERR << "no floats expected on checklist mode: " << f;
        return;
    }

    setSingleValue(f);
    output();
    redrawItems();
}

void UIRadio::onList(const AtomList& lst)
{
    if (!prop_checklist_mode_) {
        UI_ERR << "no lists expected in single mode: " << lst;
        return;
    }

    setListValue(lst);
    output();
    redrawItems();
}

void UIRadio::onMouseDown(t_object*, const t_pt& pt, const t_pt& abs_pt, long mod)
{
    if (mod == EMOD_RIGHT) {
        if (prop_checklist_mode_) {
            UIPopupMenu menu(asEObj(), MENU_NAME_CHECKLIST, abs_pt);
            menu.addItem(_("reset"));
            menu.addItem(_("flip"));
            menu.addItem(_("random"));
            return;
        }
    }

    t_rect r = rect();
    const int idx = isVertical() ? (pt.y / r.height * prop_nitems_) : (pt.x / r.width * prop_nitems_);

    if (idx >= 0 && idx < prop_nitems_) {
        if (prop_checklist_mode_) {
            items_.flip(idx);
        } else {
            setSingleValue(idx);
            items_.reset();
            items_.set(idx, true);
        }

        output();
        redrawItems();
    }
}

void UIRadio::onDblClick(t_object* view, const t_pt& pt, long modifiers)
{
    t_canvas* c = reinterpret_cast<t_canvas*>(view);
    if (c->gl_edit)
        resize(height() / zoom(), width() / zoom());
    else
        onMouseDown(view, pt, pt, modifiers);
}

float UIRadio::p_numItems() const
{
    return prop_nitems_;
}

void UIRadio::p_setNumItems(float n)
{
    int num = clip<int>(n, 2, MAX_ITEMS);
    if (num != prop_nitems_) {
        prop_nitems_ = num;
        items_.reset();
        updateSize();
    }
}

float UIRadio::p_mode() const
{
    return prop_checklist_mode_;
}

void UIRadio::p_setMode(float f)
{
    int mode = f ? 1 : 0;

    if (prop_checklist_mode_ != mode) {
        prop_checklist_mode_ = mode;
        idx_ = 0;
        items_.reset();
        redrawItems();
    }
}

AtomList UIRadio::p_value() const
{
    return prop_checklist_mode_ ? listValue() : Atom(singleValue());
}

void UIRadio::p_setValue(const AtomList& lst)
{
    if (prop_checklist_mode_)
        setListValue(lst);
    else {
        if (lst.empty()) {
            UI_ERR << "radio index expected";
            return;
        }

        setSingleValue(lst[0].asFloat());
    }

    redrawItems();
}

void UIRadio::m_flip()
{
    if (!prop_checklist_mode_) {
        UI_ERR << "not in check list mode";
        return;
    }

    items_.flip();
    output();
    redrawItems();
}

void UIRadio::m_reset()
{
    if (!prop_checklist_mode_) {
        UI_ERR << "not in check list mode";
        return;
    }

    items_.reset();
    output();
    redrawItems();
}

void UIRadio::m_next()
{
    if (prop_checklist_mode_) {
        UI_ERR << "single check mode expected";
        return;
    }

    if (idx_ + 1 < prop_nitems_) {
        idx_++;
        output();
        redrawItems();
    }
}

void UIRadio::m_random()
{
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine gen(seed);

    if (prop_checklist_mode_) {
        std::uniform_int_distribution<int> dist(0, 1);

        for (int i = 0; i < prop_nitems_; i++)
            items_.set(i, dist(gen));

        output();
        redrawItems();
    } else {
        std::uniform_int_distribution<int> dist(0, prop_nitems_ - 1);
        onFloat(dist(gen));
    }
}

void UIRadio::m_prev()
{
    if (prop_checklist_mode_) {
        UI_ERR << "single check mode expected";
        return;
    }

    if (idx_ > 0) {
        idx_--;
        output();
        redrawItems();
    }
}

void UIRadio::m_plus(t_float f)
{
    if (prop_checklist_mode_) {
        UI_ERR << "single check mode expected";
        return;
    }

    int v = idx_ + int(f);
    idx_ = (v >= 0) ? v % prop_nitems_ : prop_nitems_ - (abs(v) % prop_nitems_);
    output();
    redrawItems();
}

void UIRadio::m_minus(t_float f)
{
    if (prop_checklist_mode_) {
        UI_ERR << "single check mode expected";
        return;
    }

    int v = idx_ - int(f);
    idx_ = (v >= 0) ? v % prop_nitems_ : prop_nitems_ - (abs(v) % prop_nitems_);
    output();
    redrawItems();
}

void UIRadio::loadPreset(size_t idx)
{
    if (prop_checklist_mode_) {
        AtomList lst = PresetStorage::instance().listValueAt(presetId(), idx);
        onList(lst);
    } else {
        t_float f = PresetStorage::instance().floatValueAt(presetId(), idx);
        onFloat(f);
    }
}

void UIRadio::storePreset(size_t idx)
{
    if (prop_checklist_mode_)
        PresetStorage::instance().setListValueAt(presetId(), idx, listValue());
    else
        PresetStorage::instance().setFloatValueAt(presetId(), idx, singleValue());
}

void UIRadio::onPopup(t_symbol* menu_name, long item_idx)
{
    if (menu_name == gensym(MENU_NAME_CHECKLIST) && prop_checklist_mode_) {
        switch (item_idx) {
        case 0:
            m_reset();
            break;
        case 1:
            m_flip();
            break;
        case 2:
            m_random();
            break;
        default:
            UI_ERR << "unknown menu item: " << item_idx;
            break;
        }
    }
}

AtomList UIRadio::listValue() const
{
    AtomList lst;

    lst.reserve(prop_nitems_);
    for (int i = 0; i < prop_nitems_; i++)
        lst.append(Atom(items_[i]));

    return lst;
}

void UIRadio::setListValue(const AtomList& lst)
{
    size_t total = std::min<size_t>(lst.size(), prop_nitems_);
    for (size_t i = 0; i < total; i++)
        items_.set(i, lst[i].asFloat());
}

void UIRadio::output()
{
    if (!prop_checklist_mode_) {
        floatTo(0, idx_);
        send(idx_);
    } else {
        AtomList v = listValue();
        listTo(0, v);
        send(v);
    }
}

void UIRadio::paint()
{
    drawBackground();
    drawItems();
}

void UIRadio::drawBackground()
{
    const t_rect r = rect();
    UIPainter p = bg_layer_.painter(r);

    if (!p)
        return;

    p.setLineWidth(1);
    p.setColor(prop_color_border);

    if (isVertical()) {
        const int cell_size = r.width + zoom();
        for (int i = 1; i < prop_nitems_; i++) {
            int y = i * cell_size - zoom();
            p.drawLine(-1, y, r.width, y);
        }
    } else {
        const int cell_size = r.height + zoom();
        for (int i = 1; i < prop_nitems_; i++) {
            int x = i * cell_size - zoom();
            p.drawLine(x, -1, x, r.height);
        }
    }
}

void UIRadio::drawItems()
{
    const t_rect r = rect();
    UIPainter p = items_layer_.painter(r);

    if (!p)
        return;

    p.setColor(prop_color_active);
    const int cell_size = isVertical() ? r.width : r.height;

    // draw crosses
    if (prop_checklist_mode_) {
        p.setLineWidth(2);

        if (isVertical()) {
            for (int i = 0; i < prop_nitems_; i++) {
                if (items_[i]) {
                    const int offset = i * (cell_size + zoom());

                    const int x0 = 1;
                    const int y0 = offset + 1;
                    const int x1 = cell_size - 1;
                    const int y1 = offset + cell_size - 1;

                    // draw cross
                    p.drawLine(x0, y0, x1, y1);
                    p.drawLine(x0, y1, x1, y0);
                }
            }
        } else {
            for (int i = 0; i < prop_nitems_; i++) {
                if (items_[i]) {
                    const int offset = i * (cell_size + zoom());

                    const int x0 = offset + 1;
                    const int y0 = 1;
                    const int x1 = offset + cell_size - 1;
                    const int y1 = cell_size - 1;

                    // draw cross
                    p.drawLine(x0, y0, x1, y1);
                    p.drawLine(x0, y1, x1, y0);
                }
            }
        }
    } else {
        // draw knobs
        const int knob_offset = std::max((static_cast<int>(roundf(cell_size * 0.16f)) / 2) * 2, 2);
        const int knob_size = cell_size - knob_offset * 2 - 1;
        const float cell_offset = (cell_size - knob_size) / 2;

        if (isVertical()) {
            for (int i = 0; i < prop_nitems_; i++) {
                if (i == idx_) {
                    float y = i * (cell_size + zoom()) + cell_offset;
                    p.drawRect(cell_offset, y, knob_size, knob_size);
                    p.fill();
                }
            }

        } else {
            for (int i = 0; i < prop_nitems_; i++) {
                if (i == idx_) {
                    float x = i * (cell_size + zoom()) + cell_offset;
                    p.drawRect(x, cell_offset, knob_size, knob_size);
                    p.fill();
                }
            }
        }
    }
}

void UIRadio::okSize(t_rect* newrect)
{
    assert(prop_nitems_ > 0);

    //    if (isPatchLoading()) {
    //        newrect->height = pd_clip_min(newrect->height, 8);
    //        newrect->width = pd_clip_min(newrect->width, 8);
    //    } else {
    if (isVertical()) {
        const float box_size = pd_clip_min(static_cast<int>(newrect->height / prop_nitems_), 8);
        newrect->height = prop_nitems_ * (box_size + 1) - 1;
        newrect->width = box_size;
    } else {
        const float box_size = pd_clip_min(static_cast<int>(newrect->width / prop_nitems_), 8);
        newrect->width = prop_nitems_ * (box_size + 1) - 1;
        newrect->height = box_size;
    }
    //    }
}

void UIRadio::redrawAll()
{
    bg_layer_.invalidate();
    items_layer_.invalidate();
    redrawInnerArea();
}

void UIRadio::redrawItems()
{
    items_layer_.invalidate();
    redrawInnerArea();
}

void UIRadio::onPropChange(t_symbol* prop_name)
{
    redrawAll();
}

void UIRadio::setup()
{
    SYM_PROP_NITEMS = gensym("@nitems");

    UIObjectFactory<UIRadio> obj("ui.radio");
    obj.addAlias("ui.hrd");
    obj.addAlias("ui.vrd");

    obj.useBang();
    obj.useFloat();
    obj.useList();
    obj.usePresets();
    obj.useMouseEvents(UI_MOUSE_DOWN | UI_MOUSE_DBL_CLICK);
    obj.setDefaultSize(127, 15);
    obj.hideLabelInner();

    obj.addProperty("active_color", _("Active Color"), DEFAULT_ACTIVE_COLOR, &UIRadio::prop_color_active);
    obj.addProperty("nitems", _("Number of Items"), 8, &UIRadio::prop_nitems_, _("Main"));
    obj.setPropertyRange("nitems", 2, MAX_ITEMS);
    obj.setPropertyAccessor("nitems", &UIRadio::p_numItems, &UIRadio::p_setNumItems);
    obj.addProperty("mode", _("Check List Mode"), false, &UIRadio::prop_checklist_mode_, _("Main"));
    obj.setPropertyAccessor("mode", &UIRadio::p_mode, &UIRadio::p_setMode);
    obj.addProperty("value", &UIRadio::p_value, &UIRadio::p_setValue);

    obj.addMethod("+", &UIRadio::m_plus);
    obj.addMethod("-", &UIRadio::m_minus);
    obj.addMethod("flip", &UIRadio::m_flip);
    obj.addMethod("next", &UIRadio::m_next);
    obj.addMethod("prev", &UIRadio::m_prev);
    obj.addMethod("random", &UIRadio::m_random);
    obj.addMethod("reset", &UIRadio::m_reset);
    obj.addMethod("set", &UIRadio::p_setValue);
}
