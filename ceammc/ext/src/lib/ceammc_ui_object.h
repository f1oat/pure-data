#ifndef CEAMMC_UI_OBJECT_H
#define CEAMMC_UI_OBJECT_H

#include "ceammc_atomlist.h"
#include "ceammc_cicm.h"
#include "ceammc_data.h"
#include "ceammc_property_info.h"

#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ceammc {

class UIObject;

class UIError : public std::ostringstream {
    const UIObject* obj_;

public:
    UIError(const UIObject* obj = NULL);
    ~UIError();
    UIError& stream() { return *this; }
};

class UIDebug : public std::ostringstream {
    const UIObject* obj_;

public:
    UIDebug(const UIObject* obj = NULL);
    ~UIDebug();
    UIDebug& stream() { return *this; }
};

#define UI_ERR UIError(this).stream()
#define UI_DBG UIDebug(this).stream()

class UIObject : t_ebox {
    AtomList args_;
    std::vector<t_outlet*> outlets_;
    std::unordered_set<t_symbol*> binded_signals_;
    std::vector<UILayer*> layer_stack_;
    t_symbol* name_;
    t_symbol* old_preset_id_;
    bool use_presets_;

protected:
    UILayer bg_layer_;
    void appendToLayerList(UILayer* l);
    void prependToLayerList(UILayer* l);
    void invalidateLayer(UILayer* l);
    void invalidateBox();

public:
    t_rgba prop_color_background;
    t_rgba prop_color_border;
    t_rgba prop_color_label;
    int prop_mouse_events;

public:
    UIObject();
    ~UIObject();

    // CICM and Pd
    t_ebox* asEBox() const;
    t_eobj* asEObj() const;
    t_object* asPdObject() const;
    t_gobj* asGObj() const;
    t_pd* asPd() const;
    t_canvas* canvas() const;
    bool isPatchLoading() const;
    bool isPatchEdited() const;
    bool isVisible() const;
    const AtomList& args() const { return args_; }
    AtomList& args() { return args_; }

    void init(t_symbol* name, const AtomList& args, bool usePresets);
    t_symbol* name() const;
    t_symbol* presetId();

    void paint();
    void create();
    void erase();

    void redraw();
    void redrawInnerArea();
    void redrawBGLayer();
    void redrawLayer(UILayer& l);

    void updateSize();
    void resize(int w, int h);

    void onMouseMove(t_object* view, const t_pt& pt, long modifiers);
    void onMouseUp(t_object* view, const t_pt& pt, long modifiers);
    void onMouseDown(t_object* view, const t_pt& pt, const t_pt& abs_pt, long modifiers);
    void onMouseDrag(t_object* view, const t_pt& pt, long modifiers);
    void onMouseLeave(t_object* view, const t_pt& pt, long modifiers);
    void onMouseEnter(t_object* view, const t_pt& pt, long modifiers);
    void onMouseWheel(t_object* view, const t_pt& pt, long modifiers, double delta);
    void onDblClick(t_object* view, const t_pt& pt, long modifiers);
    void onPopup(t_symbol* menu_name, long item_idx);
    bool outputMouseEvents() const;

    void okSize(t_rect* newrect);
    void setDrawParams(t_edrawparams* params);
    void onZoom(t_float z);
    void onPropChange(t_symbol* name);
    void write(const std::string& fname);
    void read(const std::string& fname);

    void m_custom(t_symbol* sel, const AtomList& lst);

    void onBang();
    void onFloat(t_float f);
    void onSymbol(t_symbol* s);
    void onList(const AtomList& lst);
    void onAny(t_symbol* s, const AtomList& lst);
    void onKey(int k, long modifiers);
    void onKeyFilter(int k, long modifiers);
    void onData(const DataPtr& ptr);
    void onProperty(t_symbol* s, const AtomList& lst);

    void loadPreset(size_t idx);
    void storePreset(size_t idx);
    void clearPreset(size_t idx);

    void bangTo(size_t n);
    void floatTo(size_t n, t_float f);
    void symbolTo(size_t n, t_symbol* s);
    void listTo(size_t n, const AtomList& lst);
    void anyTo(size_t n, t_symbol* s, const AtomList& args);
    void anyTo(size_t n, const AtomList& msg);
    void dataTo(size_t n, const DataPtr& ptr);

    void sendBang();
    void send(t_float f);
    void send(t_symbol* s);
    void send(const AtomList& lst);
    void send(t_symbol* s, const AtomList& lst);

    t_rect rect() const;
    float x() const;
    float y() const;
    float width() const;
    float height() const;

    float zoom() const;
    void setCursor(t_cursor c);

    // presets
    void presetInit();
    void bindPreset(t_symbol* name);
    void unbindPreset(t_symbol* name);
    void rebindPreset(t_symbol* from, t_symbol* to);
    void handlePresetNameChange();

    // xlets
    size_t numInlets() const;
    size_t numOutlets() const;
    const std::vector<t_outlet*>& outlets() const { return outlets_; }
    t_outlet* createOutlet();

    bool hasProperty(t_symbol* name) const;
    bool getProperty(t_symbol* name, t_float& f) const;
    bool getProperty(t_symbol* name, AtomList& lst) const;
    void setProperty(t_symbol* name, const AtomList& lst);
    std::vector<PropertyInfo> propsInfo() const;

    // bind to global dispatcher
    void bindTo(t_symbol* s);
    void unbindFrom(t_symbol* s);
    void unbindAll();

    // fonts
    float fontSize() const;
    float fontSizeZoomed() const;

public:
    static const char* BG_LAYER;
    static const char* FONT_FAMILY;
    static const char* FONT_STYLE;
    static const char* FONT_WEIGHT;
    static const char* COLOR_ACTIVE;
    static const int FONT_SIZE;
    static const int FONT_SIZE_SMALL;

    static t_symbol* genPresetName(t_symbol* prefix);
    static void acquirePresetName(t_symbol* s);
    static void releasePresetName(t_symbol* s);

private:
    typedef std::unordered_map<t_symbol*, int> PresetNameMap;
    static PresetNameMap presets_;
};
}

#endif // CEAMMC_UI_OBJECT_H
