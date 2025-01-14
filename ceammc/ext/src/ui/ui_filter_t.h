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
#ifndef UI_FILTER_T_H
#define UI_FILTER_T_H

#include "ceammc_convert.h"
#include "ceammc_crc32.h"
#include "ceammc_filter.h"
#include "ceammc_preset.h"
#include "ceammc_ui.h"
#include "ceammc_ui_object.h"
#include "cicm/Sources/egraphics.h"

#include <array>

using namespace ceammc;

namespace ceammc {
namespace ui {

    CEAMMC_DEFINE_SYM_HASH(bpf);
    CEAMMC_DEFINE_SYM_HASH(bpfq);
    CEAMMC_DEFINE_SYM_HASH(highshelf);
    CEAMMC_DEFINE_SYM_HASH(hpf);
    CEAMMC_DEFINE_SYM_HASH(lin);
    CEAMMC_DEFINE_SYM_HASH(log);
    CEAMMC_DEFINE_SYM_HASH(lowshelf);
    CEAMMC_DEFINE_SYM_HASH(lpf);
    CEAMMC_DEFINE_SYM_HASH(notch);
    CEAMMC_DEFINE_SYM_HASH(peak);
    CEAMMC_DEFINE_SYM_HASH(rad);

    CEAMMC_DEFINE_SYM_HASH(freq);
    CEAMMC_DEFINE_SYM_HASH(q);
    CEAMMC_DEFINE_SYM_HASH(gain);
    CEAMMC_DEFINE_SYM_HASH(type);
    CEAMMC_DEFINE_SYM_HASH(scale);

    constexpr int ipow(int num, unsigned int pow)
    {
        return (pow == 0) ? 1 : num * ipow(num, pow - 1);
    }

    constexpr int MIN_LIN_FREQ = 0;
    constexpr int MAX_LIN_FREQ = 20000;
    constexpr float MIN_LOG10_FREQ = 10;
    constexpr float MAX_LOG10_FREQ = 20000;
    constexpr int MAX_Q_POW = 6;
    constexpr int MIN_Q_POW = -MAX_Q_POW;
    constexpr float MAX_Q = ipow(2, MAX_Q_POW);
    constexpr float MIN_Q = 1 / MAX_Q;
    constexpr int MIN_DB = -24;
    constexpr int MAX_DB = 24;
    constexpr int MIN_WIDTH = 100;
    constexpr int MIN_HEIGHT = 60;

    template <class TBase>
    class TFilter : public TBase {
        static_assert(std::is_same<TBase, UIObject>::value
                || std::is_same<TBase, UIDspObject>::value,
            "");

    protected:
        t_float b0_ { 1 }, b1_ { 0 }, b2_ { 0 }, a1_ { 0 }, a2_ { 0 };

    private:
        t_pt knob_pt_;
        t_symbol* prop_type { sym_lpf() };
        t_symbol* prop_scale { sym_lin() };
        t_rgba prop_color_grid;
        t_rgba prop_color_plot;
        t_rgba prop_color_knob;
        t_float prop_freq { 1000 };
        t_float prop_q { 1 };
        t_float prop_gain { 0 };

        const int outlet_n_;

    public:
        TFilter(int outn = 0)
            : outlet_n_(outn)
        {
            TBase::initPopupMenu(
                "main", {
                            { _("Low Pass"), [this](const t_pt&) {
                                 TBase::setProperty(sym_type(), AtomList(sym_lpf()));
                             } },
                            { _("High Pass"), [this](const t_pt&) {
                                 TBase::setProperty(sym_type(), AtomList(sym_hpf()));
                             } },
                            { _("Band Pass"), [this](const t_pt&) {
                                 TBase::setProperty(sym_type(), AtomList(sym_bpf()));
                             } },
                            { _("Peak Eq"), [this](const t_pt&) {
                                 TBase::setProperty(sym_type(), AtomList(sym_peak()));
                             } },
                            { _("Low Shelf"), [this](const t_pt&) {
                                 TBase::setProperty(sym_type(), AtomList(sym_lowshelf()));
                             } },
                            { _("High Shelf"), [this](const t_pt&) {
                                 TBase::setProperty(sym_type(), AtomList(sym_highshelf()));
                             } },
                            { _("Notch"), [this](const t_pt&) {
                                 TBase::setProperty(sym_type(), AtomList(sym_notch()));
                             } },
                            { "", { /* separator */ } },
                            { _("Log"), [this](const t_pt&) {
                                 TBase::setProperty(sym_scale(), AtomList(sym_log()));
                             } },
                            { _("Linear Hz"), [this](const t_pt&) {
                                 TBase::setProperty(sym_scale(), AtomList(sym_lin()));
                             } },
                            { _("Linear Rad"), [this](const t_pt&) {
                                 TBase::setProperty(sym_scale(), AtomList(sym_rad()));
                             } },
                        });
        }

        bool okSize(t_rect* newrect);
        void paint();

        void onMouseDown(t_object*, const t_pt& pt, const t_pt&, long);
        void onMouseUp(t_object* view, const t_pt& pt, long);
        void onMouseDrag(t_object* view, const t_pt& pt, long);
        void onMouseWheel(const t_pt& pt, long, float delta);

        void onPropChange(t_symbol* name);

        void loadPreset(size_t idx);
        void storePreset(size_t idx);

        void m_biquad(const AtomListView& lv);

    public:
        using Array = std::array<double, 6>;

    private:
        void updateCoeffs();
        float q() const;
        float calcDb() const;
        float calcBandwidth(float q, float w) const;

        void saveMousePoint(const t_pt& pt);
        void output();

        void setBA(const Array& ba);

        void knobUpdateFreq();
        void knobUpdateQ();
        void knobUpdateGain();

        void freqUpdateKnob();
        void qUpdateKnob();
        void gainUpdateKnob();

        double nyquistFreq() const { return 0.5 * sys_getsr(); }

    public:
        template <class Factory>
        static void setup(Factory& obj)
        {
            obj.hideLabelInner();

            obj.useAnnotations();
            obj.usePresets();
            obj.usePopup();
            obj.setDefaultSize(300, 100);
            obj.useMouseEvents(UI_MOUSE_DOWN | UI_MOUSE_UP | UI_MOUSE_DRAG | UI_MOUSE_WHEEL);
            obj.outputMouseEvents(MouseEventsOutput::DEFAULT_OFF);

            obj.addProperty("grid_color", _("Grid Color"), DEFAULT_BORDER_COLOR, &TFilter::prop_color_grid);
            obj.addProperty("plot_color", _("Plot Color"), "0 0 0 1", &TFilter::prop_color_plot);
            obj.addProperty("knob_color", _("Knob Color"), DEFAULT_ACTIVE_COLOR, &TFilter::prop_color_knob);
            obj.setPropertyDefaultValue("label_color", DEFAULT_BORDER_COLOR);

            obj.addFloatProperty(str_freq,
                _("Frequency"), 1000, &TFilter::prop_freq, _("Main"));
            obj.setPropertyRange(str_freq, 0, MAX_LIN_FREQ);
            obj.setPropertySave(str_freq, false);

            obj.addFloatProperty(str_q,
                _("Quality factor"), std::sqrt(0.5), &TFilter::prop_q, _("Main"));
            obj.setPropertySave(str_q, false);
            obj.setPropertyRange(str_q, MIN_Q, MAX_Q);

            obj.addFloatProperty(str_gain,
                _("Gain"), 0, &TFilter::prop_gain, _("Main"));
            obj.setPropertyRange(str_gain, MIN_DB, MAX_DB);
            obj.setPropertyUnits(str_gain, "db");
            obj.setPropertySave(str_gain, false);

            obj.addMenuProperty(str_type,
                _("Filter Type"),
                "lpf",
                &TFilter::prop_type,
                "lpf hpf bpf bpfq lowshelf highshelf peak notch",
                _("Main"));

            obj.addMenuProperty(str_scale,
                _("Scale"),
                "lin",
                &TFilter::prop_scale,
                "lin log rad",
                _("Main"));

            obj.addMethod("biquad", &TFilter::m_biquad);
        }
    };

    template <class TBase>
    void TFilter<TBase>::saveMousePoint(const t_pt& pt)
    {
        knob_pt_.x = clip01<t_float>(pt.x / TBase::width());
        knob_pt_.y = clip01<t_float>(pt.y / TBase::height());

        knobUpdateFreq();
        knobUpdateGain();

        if (crc32_hash(prop_type) != hash_peak)
            knobUpdateQ();
    }

    template <class TBase>
    void TFilter<TBase>::output()
    {
        Atom res[5] = { b0_, b1_, b2_, a1_, a2_ };
        TBase::listTo(outlet_n_, AtomListView(res, 5));
    }

    template <class TBase>
    void TFilter<TBase>::setBA(const Array& ba)
    {
        auto check_nan = [](double v) { return std::isnormal(v) ? v : 0; };

        b0_ = check_nan(ba[0]);
        b1_ = check_nan(ba[1]);
        b2_ = check_nan(ba[2]);
        a1_ = check_nan(ba[4]);
        a2_ = check_nan(ba[5]);
    }

    template <class TBase>
    void TFilter<TBase>::knobUpdateFreq()
    {
        using namespace convert;

        if (crc32_hash(prop_scale) == hash_lin) {
            prop_freq = lin2lin<float, 0, 1>(knob_pt_.x, MIN_LIN_FREQ, MAX_LIN_FREQ);
        } else if (crc32_hash(prop_scale) == hash_log) {
            static const float loga = std::log10(MIN_LOG10_FREQ);
            static const float logb = std::log10(MAX_LOG10_FREQ);

            const float fp = lin2lin_clip<float, 0, 1>(knob_pt_.x, loga, logb);
            prop_freq = std::pow(10, fp);
        } else if (crc32_hash(prop_scale) == hash_rad) {
            prop_freq = lin2lin_clip<float, 0, 1>(knob_pt_.x, 0, nyquistFreq());
        } else {
            UI_ERR << "unknown scale: " << prop_scale;
            prop_freq = 1;
        }
    }

    template <class TBase>
    void TFilter<TBase>::knobUpdateQ()
    {
        using namespace convert;

        prop_q = std::pow(2, lin2lin_clip<float>(knob_pt_.y, 0, 1, MAX_Q_POW, MIN_Q_POW));
    }

    template <class TBase>
    void TFilter<TBase>::knobUpdateGain()
    {
        using namespace convert;

        prop_gain = lin2lin_clip<float>(knob_pt_.y, 0, 1, MAX_DB, MIN_DB);
    }

    template <class TBase>
    void TFilter<TBase>::freqUpdateKnob()
    {
        using namespace convert;

        if (crc32_hash(prop_scale) == hash_lin) {
            knob_pt_.x = lin2lin<float, MIN_LIN_FREQ, MAX_LIN_FREQ>(prop_freq, 0, 1);
        } else if (crc32_hash(prop_scale) == hash_log) {
            static const float loga = std::log10(MIN_LOG10_FREQ);
            static const float logb = std::log10(MAX_LOG10_FREQ);

            const float fp = std::log10(prop_freq);
            const auto f = lin2lin_clip<float>(fp, loga, logb, 0, 1);
            knob_pt_.x = std::isnormal(f) ? f : 0;
        } else if (crc32_hash(prop_scale) == hash_rad) {
            knob_pt_.x = lin2lin_clip<float>(prop_freq, 0, nyquistFreq(), 0, 1);
        } else {
            UI_ERR << "unknown scale: " << prop_scale;
            knob_pt_.x = 0;
        }
    }

    template <class TBase>
    void TFilter<TBase>::qUpdateKnob()
    {
        using namespace convert;

        auto y = lin2lin_clip<float, MIN_Q_POW, MAX_Q_POW>(std::log2(prop_q), 1, 0);
        knob_pt_.y = std::isnormal(y) ? y : 0;
    }

    template <class TBase>
    void TFilter<TBase>::gainUpdateKnob()
    {
        using namespace convert;

        knob_pt_.y = lin2lin_clip<float, MIN_DB, MAX_DB>(prop_gain, 1, 0);
    }

    template <class TBase>
    bool TFilter<TBase>::okSize(t_rect* newrect)
    {
        newrect->w = pd_clip_min(newrect->w, MIN_WIDTH);
        newrect->h = pd_clip_min(newrect->h, MIN_HEIGHT);
        return true;
    }

    template <class TBase>
    void TFilter<TBase>::paint()
    {
        const auto Fs = sys_getsr();
        const auto w = flt::freq2ang<float>(prop_freq, Fs);

        const auto bw = calcBandwidth(q(), w);

        sys_vgui("ui::filter_update %s %lx %d %d %d "
                 "#%6.6x #%6.6x #%6.6x #%6.6x #%6.6x "
                 "%f %f %f %f %f "
                 "%.2f %.2f {%s} {%s} "
                 "%.1f %f\n",
            TBase::asEBox()->b_canvas_id->s_name, TBase::asEBox(),
            (int)TBase::width(), (int)TBase::height(), (int)TBase::zoom(),
            rgba_to_hex_int(TBase::prop_color_background),
            rgba_to_hex_int(prop_color_grid),
            rgba_to_hex_int(TBase::prop_color_label),
            rgba_to_hex_int(prop_color_plot),
            rgba_to_hex_int(prop_color_knob),
            b0_, b1_, b2_, a1_, a2_,
            knob_pt_.x * TBase::width(), knob_pt_.y * TBase::height(),
            prop_scale->s_name, prop_type->s_name,
            q(), bw);
    }

    template <class TBase>
    void TFilter<TBase>::onMouseDown(t_object*, const t_pt& pt, const t_pt&, long)
    {
        saveMousePoint(pt);
        updateCoeffs();
        TBase::redraw();
        output();
    }

    template <class TBase>
    void TFilter<TBase>::onMouseUp(t_object* view, const t_pt& pt, long)
    {
        saveMousePoint(pt);
        updateCoeffs();
        TBase::redraw();
        output();
    }

    template <class TBase>
    void TFilter<TBase>::onMouseDrag(t_object* view, const t_pt& pt, long)
    {
        saveMousePoint(pt);
        updateCoeffs();
        TBase::redraw();
        output();
    }

    template <class TBase>
    void TFilter<TBase>::onMouseWheel(const t_pt& pt, long, float delta)
    {
        if (crc32_hash(prop_type) == hash_peak) {
            prop_q = clip<t_float>(prop_q * (1 + delta * 0.05), MIN_Q, MAX_Q);
            updateCoeffs();
            TBase::redraw();
            output();
        }
    }

    template <class TBase>
    void TFilter<TBase>::onPropChange(t_symbol* name)
    {
        switch (crc32_hash(name)) {
        case hash_freq:
            freqUpdateKnob();
            updateCoeffs();
            break;
        case hash_q:
            qUpdateKnob();
            updateCoeffs();
            break;
        case hash_gain:
            gainUpdateKnob();
            updateCoeffs();
            break;
        case hash_type:
            updateCoeffs();
            break;
        case hash_scale:
            freqUpdateKnob();
            updateCoeffs();
            break;
        }

        TBase::redraw();
    }

    template <class TBase>
    void TFilter<TBase>::loadPreset(size_t idx)
    {
        auto lv = PresetStorage::instance().listValueAt(TBase::presetId(), idx);
        if (lv.size() == 3) {
            prop_freq = lv[0].asFloat(1000);
            prop_q = lv[1].asFloat(1);
            prop_gain = lv[2].asFloat(0);

            freqUpdateKnob();
            gainUpdateKnob();
            qUpdateKnob();

            updateCoeffs();
            TBase::redraw();
            output();
        }
    }

    template <class TBase>
    void TFilter<TBase>::storePreset(size_t idx)
    {
        AtomList data { prop_freq, prop_q, prop_gain };
        PresetStorage::instance().setListValueAt(TBase::presetId(), idx, data);
    }

    template <class TBase>
    void TFilter<TBase>::updateCoeffs()
    {
        auto Fs = sys_getsr();
        auto w = flt::freq2ang<float>(prop_freq, Fs);

#define CASE(type)    \
    case hash_##type: \
        return setBA(flt::calc_##type<double>(w, q()))

        switch (crc32_hash(prop_type)) {
            CASE(notch);
            CASE(lpf);
            CASE(hpf);
            CASE(bpfq);
            CASE(bpf);
        case hash_peak:
            return setBA(flt::calc_peak_eq<double>(w, q(), calcDb()));
        case hash_lowshelf:
            return setBA(flt::calc_lowshelf<double>(w, calcDb(), 1));
        case hash_highshelf:
            return setBA(flt::calc_highshelf<double>(w, calcDb(), 1));
        default:
            return;
        }
#undef CASE
    }

    template <class TBase>
    float TFilter<TBase>::q() const
    {
        switch (crc32_hash(prop_type)) {
        case hash_notch:
        case hash_bpfq:
        case hash_bpf:
        case hash_peak:
            return prop_q;
        case hash_lpf:
        case hash_hpf:
            return std::sqrt(0.5);
        default:
            return 0.1;
        }
    }

    template <class TBase>
    float TFilter<TBase>::calcDb() const
    {
        switch (crc32_hash(prop_type)) {

        case hash_peak:
        case hash_lowshelf:
        case hash_highshelf:
            return prop_gain;
        default:
            return 0;
        }
    }

    template <class TBase>
    float TFilter<TBase>::calcBandwidth(float q, float w) const
    {
        float bw = 0;
        if (crc32_hash(prop_scale) == hash_rad)
            bw = flt::q2bandwidth<float>(q, w);
        else
            bw = clip<t_float, MIN_LIN_FREQ, MAX_LIN_FREQ>(flt::q2bandwidth<float>(q, w) * prop_freq);

        return std::isnormal(bw) ? bw : 0;
    }

    template <class TBase>
    void TFilter<TBase>::m_biquad(const AtomListView& lv)
    {
        if (lv.size() != 5) {
            UI_ERR << "usage: biquad b0 b1 b2 a1 a2";
            return;
        }

        b0_ = lv[0].asFloat(1);
        b1_ = lv[1].asFloat(0);
        b2_ = lv[2].asFloat(0);
        a1_ = lv[3].asFloat(0);
        a2_ = lv[4].asFloat(0);
        TBase::redraw();
    }
}
}

#endif // UI_FILTER_T_H
