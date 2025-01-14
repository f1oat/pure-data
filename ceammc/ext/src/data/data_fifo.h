/*****************************************************************************
 * Copyright 2020 Serge Poltavsky. All rights reserved.
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
#ifndef DATA_FIFO_H
#define DATA_FIFO_H

#include "ceammc_editor_object.h"
#include "ceammc_message.h"
#include "ceammc_object.h"

#include <list>

using namespace ceammc;

using MessageFifo = std::list<Message>;
using DataFifoBase = EditorObject<BaseObject, EditorSyntax::SELECTOR>;

class DataFifo : public DataFifoBase {
    MessageFifo fifo_;
    size_t size_;

public:
    DataFifo(const PdArgs& args);

    void onBang() override;
    void onFloat(t_float v) override;
    void onSymbol(t_symbol* s) override;
    void onList(const AtomListView& lv) override;
    void onAny(t_symbol* s, const AtomListView& lv) override;

    void m_flush(t_symbol*, const AtomListView&);
    void m_clear(t_symbol*, const AtomListView&);
    void m_pop(t_symbol*, const AtomListView&);
    void m_resize(t_symbol*, const AtomListView& l);

    void add(const Message& msg);

    // remove all, no output
    void clear();

    // output all, remove all
    void flush();

    // output and remove first in queue
    void pop();

    void resize(size_t sz);
    void dump() const override;

    // editor methods
    void editorAddLine(t_symbol* sel, const AtomListView& lv) final;
    void editorClear() final;
    EditorLineList getContentForEditor() const final;
    int calcEditorLines() const final;
    int calcEditorChars() const final;
    EditorTitleString editorTitle() const final { return "DATA.FIFO"; }
};

void setup_data_fifo();

#endif // DATA_FIFO_H
