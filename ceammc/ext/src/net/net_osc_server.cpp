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
#include "net_osc_server.h"
#include "ceammc_factory.h"
#include "ceammc_format.h"
#include "fmt/core.h"

#include <algorithm>
#include <unordered_set>

namespace ceammc {
namespace net {

    NetOscServer::NetOscServer(const PdArgs& args)
        : BaseObject(args)
        , name_(nullptr)
        , url_(nullptr)
        , auto_start_(nullptr)
        , dump_(nullptr)
    {
        createOutlet();

        name_ = new SymbolProperty("@name", gensym("default"), PropValueAccess::INITONLY);
        name_->setArgIndex(0);
        addProperty(name_);

        url_ = new OscUrlProperty("@url", Atom(gensym("udp:9000")), PropValueAccess::INITONLY);
        url_->setArgIndex(1);
        url_->registerProps(this);
        addProperty(url_);

        dump_ = new BoolProperty("@dump", false);
        dump_->setSuccessFn([this](Property*) {
            if (server_ && server_->isValid())
                server_->setDumpAll(dump_->value());
        });
        addProperty(dump_);

        auto_start_ = new BoolProperty("@auto_start", true);
        addProperty(auto_start_);
    }

    NetOscServer::~NetOscServer()
    {
        osc::OscServerList::instance().unregisterServer(name_->value()->s_name);
    }

    void NetOscServer::dump() const
    {
        osc::OscServerList::instance().dump();
        BaseObject::dump();
    }

    void NetOscServer::initDone()
    {
        auto name = name_->value()->s_name;
        auto url = url_->value();

        auto& srv_list = osc::OscServerList::instance();

        auto osc = srv_list.findByName(name);
        if (osc.expired()) {
            t_symbol* str_url = &s_;
            if (url_->isProtoPortAddr())
                server_.reset(new osc::OscServer(name, 19090, osc::OSC_PROTO_UDP));
            else if (url_->isUrlAddr() && url_->value().getSymbol(&str_url))
                server_.reset(new osc::OscServer(name, str_url->s_name));
        } else
            server_ = osc.lock();

        if (!server_ || !server_->isValid()) {
            OBJ_ERR << fmt::format("can't create server '{}': {}", name, to_string(url));
        } else {
            server_->setDumpAll(dump_->value());
            srv_list.registerServer(name, server_);

            if (auto_start_->value())
                server_->start(true);
        }
    }

    void NetOscServer::m_start(t_symbol* s, const AtomListView& lv)
    {
        bool value = false;

        if (lv.empty())
            value = true;
        else if (lv.isBool())
            value = lv[0].asT<bool>();
        else {
            METHOD_ERR(s) << "bool value expected, got: " << lv;
            return;
        }

        bool ok = false;
        if (server_ && server_->isValid())
            ok = server_->start(value);

        if (!ok)
            METHOD_ERR(s) << "can't start osc server";
    }

    void NetOscServer::m_stop(t_symbol* s, const AtomListView& lv)
    {
        bool value = false;

        if (lv.empty())
            value = true;
        else if (lv.isBool())
            value = lv[0].asT<bool>();
        else {
            METHOD_ERR(s) << "bool value expected, got: " << lv;
            return;
        }

        bool ok = false;
        if (server_ && server_->isValid())
            ok = server_->start(!value);

        if (!ok)
            METHOD_ERR(s) << "can't stop osc server";
    }
}
}

void setup_net_osc_server()
{
    using namespace ceammc;

    ObjectFactory<net::NetOscServer> obj("net.osc.server");
    obj.addAlias("net.osc");

    obj.parseArgsMode(PdArgs::PARSE_UNQUOTE);
    obj.parsePropsMode(PdArgs::PARSE_UNQUOTE);

    obj.addMethod("start", &net::NetOscServer::m_start);
    obj.addMethod("stop", &net::NetOscServer::m_stop);
}
