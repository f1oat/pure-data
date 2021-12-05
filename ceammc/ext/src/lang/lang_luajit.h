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
#ifndef LANG_LUAJIT_H
#define LANG_LUAJIT_H

#include "lua_cmd.h"
#include "lua_interp.h"
#include "poll_dispatcher.h"
#include "pollthread_object.h"

#include "readerwriterqueue.h"

using namespace ceammc;

using LuaCommandQueue = moodycamel::ReaderWriterQueue<lua::LuaCmd>;

class LangLuaJit : public ceammc::PollThreadQueueObject<lua::LuaCmd> {
    lua::LuaInterp interp_;
    LuaCommandQueue lua_cmd_queue_;

public:
    LangLuaJit(const PdArgs& args);

    void onBang() override;
    void onFloat(t_float f) override;
    void onList(const AtomList& lst) override;

    Future createTask() override;
    void processMessage(const lua::LuaCmd& msg) override;

    void m_load(t_symbol* s, const AtomListView& lv);
    void m_eval(t_symbol* s, const AtomListView& lv);

public:
    LuaCommandQueue& inPipe() { return lua_cmd_queue_; }
    LuaCommandQueue& outPipe() { return result_; }
    lua::LuaInterp& interp() { return interp_; }
};

void setup_lang_luajit();

#endif // LANG_LUAJIT_H
