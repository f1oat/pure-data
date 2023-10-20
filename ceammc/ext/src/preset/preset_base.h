#ifndef PRESET_BASE_H
#define PRESET_BASE_H

#include "ceammc_object.h"
#include "ceammc_table_editor.h"

using namespace ceammc;

class PresetBase : public TableObject<BaseObject> {
    FlagProperty* global_;
    FlagProperty* subpatch_;

    t_symbol* name_;
    t_symbol* path_;
    t_symbol* preset_path_;

public:
    PresetBase(const PdArgs& args);
    ~PresetBase() override;

    void initDone() override;

    t_symbol* makePath() const;
    t_symbol* makePresetPath() const;

    t_symbol* name() const;
    t_symbol* path() const { return path_; }
    t_symbol* presetPath() const { return preset_path_; }

    virtual void loadFrom(t_float idx);
    virtual void storeAt(size_t idx);

    t_float loadFloat(t_float idx, t_float def = 0.f);
    t_symbol* loadSymbol(size_t idx, t_symbol* def);
    AtomListView loadList(size_t idx, const AtomListView& def = {});
    AtomList loadAny(size_t idx, const AtomList& def = AtomList());

    void storeFloat(t_float f, size_t idx);
    void storeSymbol(t_symbol* s, size_t idx);
    void storeList(const AtomList& l, size_t idx);
    void storeAny(t_symbol* sel, const AtomList& l, size_t idx);

    // editor overloads
    void tableEditorClear() final;
    TableLineList getTableContentForEditor() const final;
    void tableEditorAddLine(t_symbol* sel, const AtomListView& lv) final;
    EditorTitleString editorTitle() const final;

    virtual bool setEditorPreset(size_t idx, const AtomListView& lv);
    virtual AtomList editorPresetValue(size_t idx) const;

public:
    void m_load(t_symbol*, const AtomListView& index);
    void m_interp(t_symbol*, const AtomListView& index);
    void m_store(t_symbol*, const AtomListView& index);
    void m_update(t_symbol*, const AtomListView&);
    void m_clear(t_symbol*, const AtomListView& index);

private:
    void bind();
    void unbind();
};

#endif // PRESET_BASE_H
