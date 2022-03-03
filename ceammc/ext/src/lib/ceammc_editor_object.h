/*****************************************************************************
 * Copyright 2022 Serge Poltavsky. All rights reserved.
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
#ifndef CEAMMC_EDITOR_OBJECT_H
#define CEAMMC_EDITOR_OBJECT_H

#include "ceammc_object.h"

#include <boost/static_string.hpp>
#include <list>

#include "extra/boost_intrusive_pool.hpp"

namespace ceammc {

using EditorTitleString = boost::static_string<32>;

class EditorString : public memorypool::boost_intrusive_pool_item {
public:
    boost::static_string<MAXPDSTRING> str;

    EditorString() { }
    void destroy() final { str.clear(); }

    void append(t_float t);
    void append(const char* txt);
    void append(t_symbol* s) { append(s->s_name); }
    void append(const Atom& a);
    void append(const AtomList& lst, const char* delim = " ");
};

using EditorStringPtr = boost::intrusive_ptr<EditorString>;
using EditorLineList = std::vector<EditorStringPtr>;

class EditorStringPool {
    EditorStringPool() = delete;

public:
    using Pool = memorypool::boost_intrusive_pool<EditorString>;

public:
    static Pool& pool();
    static void dumpMemoryUsage();
};

class EditorObjectImpl {
    t_object* owner_;
    void* guiconnect_;

public:
    EditorObjectImpl(t_object* owner);
    ~EditorObjectImpl();

    /**
     * create and open editor TCL window
     * @param cnv - pointer to onwer canvas
     * @param data - editor content
     * @param nchars - line width in chars
     * @param nlines - number of lines
     * @param lineNumbers - show line numbers
     * @param highlightSyntax - highlight syntax
     * @return true on success, false on error
     */
    void open(t_canvas* cnv, const EditorLineList& data,
        const EditorTitleString& title,
        int x, int y, int nchars, int nlines,
        bool lineNumbers,
        bool highlightSyntax);

    /**
     * close TCL editor and stop GUI listening
     */
    void close();

    /**
     * Sync editor with specified data
     * @param lv
     */
    void sync(const EditorLineList& list);

private:
    unsigned long xowner() const { return reinterpret_cast<unsigned long>(owner_); }
};

template <typename BaseClass>
class EditorObject : public BaseClass {
    EditorObjectImpl impl_;
    bool line_nums_;
    bool highlight_;

public:
    EditorObject(const PdArgs& args)
        : BaseClass(args)
        , impl_(this->owner())
        , line_nums_(true)
        , highlight_(true)
    {
    }

    void onClick(t_floatarg xpos, t_floatarg ypos, t_floatarg shift, t_floatarg ctrl, t_floatarg alt) override
    {
        impl_.open(this->canvas(),
            this->getContentForEditor(),
            this->editorTitle(),
            (int)xpos, (int)ypos,
            this->calcEditorChars(),
            this->calcEditorLines(),
            line_nums_, highlight_);
    }

    virtual void editorClear() = 0;
    virtual void editorAddLine(t_symbol* sel, const AtomListView& lv) = 0;
    virtual EditorLineList getContentForEditor() const = 0;
    virtual EditorTitleString editorTitle() const { return "EDITOR"; }

    virtual int calcEditorLines() const { return 20; }
    virtual int calcEditorChars() const { return 80; }

    void m_editor_clear(t_symbol* s, const AtomListView& lv) { this->editorClear(); }
    void m_editor_close(t_symbol* s, const AtomListView& lv) { impl_.close(); }
    void m_editor_addline(t_symbol* s, const AtomListView& lv) { this->editorAddLine(s, lv); }
    void m_editor_sync(t_symbol*, const AtomListView&) { impl_.sync(this->getContentForEditor()); }

    /**
     * If show line numbers in editor
     */
    bool lineNumbers() const { return line_nums_; }

    /**
     * Set on/off line numbering in editor
     */
    void setLineNumbers(bool value) { line_nums_ = value; }

    /**
     * If highlight syntax in editor
     */
    bool highlight() const { return highlight_; }

    /**
     * Set on/off syntax highlighting in editor
     */
    void setHighlight(bool value) { highlight_ = value; }

public:
    using ThisType = EditorObject<BaseClass>;

    template <typename Factory>
    static void registerMethods(Factory& obj)
    {
        obj.useClick();
        obj.addMethod(".clear", &ThisType::m_editor_clear);
        obj.addMethod(".close", &ThisType::m_editor_close);
        obj.addMethod(".addline", &ThisType::m_editor_addline);
        obj.addMethod(".sync", &ThisType::m_editor_sync);
    }
};

}

#endif // CEAMMC_EDITOR_OBJECT_H
