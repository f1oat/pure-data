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
#include "proto_http.h"
#include "ceammc_factory.h"

extern "C" {
#include "s_net.h"
}

ProtoHttp::ProtoHttp(const PdArgs& args)
    : BaseObject(args)
    , host_(nullptr)
    , port_(nullptr)
{
    host_ = new SymbolProperty("@host", gensym("127.0.0.1"));
    host_->setArgIndex(0);
    addProperty(host_);

    port_ = new IntProperty("@port", 0);
    port_->setArgIndex(1);
    addProperty(port_);

    createOutlet();
}

void ProtoHttp::onSymbol(t_symbol* s)
{
    int sockfd, status;
    struct addrinfo *ailist = NULL, *ai;
    struct sockaddr_storage ss = { 0 }; // IPv4 or IPv6 addr

    // generate addrinfo list
    status = addrinfo_get_list(&ailist, host_->value()->s_name, port_->value(), SOCK_STREAM);
    if (status != 0) {
        OBJ_ERR << "bad host or port? " << gai_strerror(status) << " (" << status << ")";
        return;
    }

    // try each addr until we find one that works
    for (ai = ailist; ai != NULL; ai = ai->ai_next) {
        sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (sockfd < 0)
            continue; // go to the next addr

        // perform socket setsockopt(), bind(), connect(), etc as required,
        // cleanup and continue to next addr on failure
        /* try to connect */
        if (socket_connect(sockfd, ai->ai_addr, ai->ai_addrlen, 10.f) < 0) {
            socket_close(sockfd);
            sockfd = -1;
            continue;
        }
        /* this addr worked */

        // save addr that worked and exit loop
        memcpy(&ss, ai->ai_addr, ai->ai_addrlen);
        break;
    }

    freeaddrinfo(ailist); // cleanup

    // confirm that socket setup succeeded
    if (sockfd == -1) {
        int err = socket_errno();
        OBJ_ERR << "socket setup failed: " << strerror(errno) << " (" << errno << ")";
        return;
    }

    std::string buf = s->s_name;
    buf += '\n';
    write(sockfd, buf.data(), buf.length());

    OBJ_DBG << "write ok: " << buf;
}

void setup_proto_http()
{
    ObjectFactory<ProtoHttp> obj("proto.http");
}
