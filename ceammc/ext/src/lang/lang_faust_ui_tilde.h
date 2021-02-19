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
#ifndef LANG_FAUST_UI_TILDE_H
#define LANG_FAUST_UI_TILDE_H

#include "ceammc_sound_external.h"
#include "nui/nui.h"
#include "nui/property.h"
#include "nui/rect.h"
#include "nui/tk_view_impl.h"
#include "nui/view.h"
#include "nui/widget.h"

using namespace ceammc;
using namespace ceammc::ui;

class FaustMasterView {
    FrameModel model_;
    FrameView view_;

    using SliderModelPtr = std::unique_ptr<SliderModel>;
    using LabelModelPtr = std::unique_ptr<LabelModel>;
    using PropSliderView = FloatPropertyObserver<SliderData, SliderData::VALUE>;
    using PropSliderViewPtr = std::unique_ptr<PropSliderView>;
    std::vector<SliderModelPtr> sliders_;
    std::vector<LabelModelPtr> labels_;
    std::vector<PropSliderViewPtr> slider_props_;

public:
    FaustMasterView();
    ~FaustMasterView();

    Size build(const std::vector<Property*>& props);

    void addProperty(Property* p);

    void create(WinId win, WidgetId id, const Size& sz, int zoom);
    void erase(WinId win, WidgetId id) { }
    void update(WinId win, WidgetId id);

    void setSize(const Size& sz);
    void select(bool state);
};

class LangFaustUiTilde : public ui::Widget<SoundExternal> {
    FaustMasterView vc_;

public:
    LangFaustUiTilde(const PdArgs& args);

    void processBlock(const t_sample** in, t_sample** out) final;
    void buildUI() override;

    void onWidgetShow() override;
    void onWidgetResize(const Size& sz) override;
    void onWidgetSelect(bool state) override;
};

void setup_lang_faust_ui_tilde();

#endif // LANG_FAUST_UI_TILDE_H