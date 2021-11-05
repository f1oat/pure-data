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
#include "proto_vlc.h"
#include "ceammc_factory.h"
#include "ceammc_format.h"
#include "fmt/format.h"

ProtoVlc::ProtoVlc(const PdArgs& args)
    : BaseObject(args)
    , host_(nullptr)
    , port_(nullptr)
{
    port_ = new IntProperty("@port", 8080);
    port_->setArgIndex(0);
    addProperty(port_);

    host_ = new SymbolProperty("@host", gensym("localhost"));
    host_->setArgIndex(1);
    addProperty(host_);

    createOutlet();
}

void ProtoVlc::m_play(t_symbol* /*s*/, const AtomListView& /*lv*/)
{
    writeCmd("pl_play\n");
}

void ProtoVlc::m_stop(t_symbol* /*s*/, const AtomListView& /*lv*/)
{
    writeCmd("pl_stop\n");
}

void ProtoVlc::m_next(t_symbol* /*s*/, const AtomListView& /*lv*/)
{
    writeCmd("pl_next\n");
}

void ProtoVlc::m_prev(t_symbol* /*s*/, const AtomListView& /*lv*/)
{
    writeCmd("pl_prev\n");
}

void ProtoVlc::m_pause(t_symbol* /*s*/, const AtomListView& lv)
{
    if (lv.empty()) {
        writeCmd("pause\n");
    } else {
        char buf[32];
        sprintf(buf, "pause %s\n", lv.boolAt(0, false) ? "off" : "on");
        writeCmd(buf);
    }
}

void ProtoVlc::m_seek(t_symbol* /*s*/, const AtomListView& lv)
{
    char buf[64];
    sprintf(buf, "seek %d\n", lv.intAt(0, 0));
    writeCmd(buf);
}

void ProtoVlc::m_loop(t_symbol* /*s*/, const AtomListView& lv)
{
    char buf[32];
    sprintf(buf, "loop %s\n", lv.boolAt(0, false) ? "on" : "off");
    writeCmd(buf);
}

void ProtoVlc::m_frame(t_symbol* s, const AtomListView& lv)
{
    writeCmd("frame\n");
}

void ProtoVlc::m_repeat(t_symbol* /*s*/, const AtomListView& lv)
{
    char buf[32];
    sprintf(buf, "repeat %s\n", lv.boolAt(0, false) ? "on" : "off");
    writeCmd(buf);
}

void ProtoVlc::m_fullscreen(t_symbol* /*s*/, const AtomListView& lv)
{
    if (lv.empty()) {
        writeCmd("fullscreen\n");
    } else {
        char buf[32];
        sprintf(buf, "fullscreen %s\n", lv.boolAt(0, false) ? "on" : "off");
        writeCmd(buf);
    }
}

void ProtoVlc::m_add(t_symbol* s, const AtomListView& lv)
{
    if (lv.empty()) {
        METHOD_ERR(s) << "file name required";
        return;
    }

    auto path = findInStdPaths(to_string(lv).c_str());
    if (path.empty()) {
        METHOD_ERR(s) << "file not found: " << lv;
        return;
    }

    std::string cmd = "add " + path + "\n";
    writeCmd(cmd.c_str());
}

void ProtoVlc::m_clear(t_symbol* /*s*/, const AtomListView& /*lv*/)
{
    writeCmd("clear\n");
}

void ProtoVlc::m_delete(t_symbol* /*s*/, const AtomListView& lv)
{
    char buf[64];
    sprintf(buf, "delete %d\n", lv.intAt(0, 0));
    writeCmd(buf);
}

void ProtoVlc::m_shutdown(t_symbol* /*s*/, const AtomListView& /*lv*/)
{
    writeCmd("shutdown\n");
}

void ProtoVlc::writeCmd(const char* cmd)
{
    const auto req = fmt::format("GET /requests/status.xml HTTP/1.1\n"
                                 "Host: {}:{}\n"
                                 "User-Agent: pd\n"
                                 "Accept: */*\n"
                                 "\n",
        cmd, host_->value()->s_name, port_->value());

    OBJ_DBG << req;

    AtomList l;
    l.reserve(req.size());
    for (auto c : req)
        l.append(uint8_t(c));

    anyTo(0, gensym("send"), l.view());
}

void setup_proto_vlc()
{
    ObjectFactory<ProtoVlc> obj("proto.vlc");

    obj.addMethod("play", &ProtoVlc::m_play);
    obj.addMethod("stop", &ProtoVlc::m_stop);
    obj.addMethod("next", &ProtoVlc::m_next);
    obj.addMethod("prev", &ProtoVlc::m_prev);
    obj.addMethod("pause", &ProtoVlc::m_pause);
    obj.addMethod("loop", &ProtoVlc::m_loop);
    obj.addMethod("repeat", &ProtoVlc::m_repeat);
    obj.addMethod("seek", &ProtoVlc::m_seek);
    obj.addMethod("add", &ProtoVlc::m_add);
    obj.addMethod("clear", &ProtoVlc::m_clear);
    obj.addMethod("delete", &ProtoVlc::m_delete);
    obj.addMethod("shutdown", &ProtoVlc::m_shutdown);
    obj.addMethod("frame", &ProtoVlc::m_frame);
    obj.addMethod("fs", &ProtoVlc::m_fullscreen);
}
