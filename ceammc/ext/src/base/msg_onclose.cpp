/*****************************************************************************
 * Copyright 2023 Serge Poltavski. All rights reserved.
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
#include "msg_onclose.h"
#include "ceammc_canvas.h"
#include "ceammc_factory.h"
#include "fmt/core.h"

constexpr const char* MENUCLOSE = "menuclose";
constexpr const char* ORIG_MENUCLOSE = "menuclose [msg.onclose]";

static void msg_onclose_menuclose(t_canvas* x, t_floatarg f)
{
    canvas_foreach(x, [](t_gobj* x, const t_class* c) {
        auto obj = ObjectFactory<MsgOnClose>::objectCast(x);
        if (obj)
            obj->onCloseBang();
    });

    /* pass the menuclose message on to the real canvasmethod*/
    pd::message_to((t_pd*)x, gensym(ORIG_MENUCLOSE), Atom(f));
}

MsgOnClose::MsgOnClose(const PdArgs& args)
    : BaseObject(args)
{
    createOutlet();

    bindReceive(gensym(MENUCLOSE));

    AtomList raw_args;
    binbufArgs().restorePrimitives(raw_args);
    raw_args.expandDollarArgs(canvas());
    raw_args.view().split(Atom::comma(),
        [this](const AtomListView& lv) {
            msg_.push_back(Message::makeTyped(lv));
        });
}

void MsgOnClose::onCloseBang()
{
    for (auto& m : msg_)
        messageTo(0, m);
}

void setup_msg_onclose()
{
    constexpr const char* OBJ_NAME = "msg.onclose";
    ObjectFactory<MsgOnClose> obj(OBJ_NAME);
    obj.useLoadBang();

    auto orgfun = zgetfn(&canvas_get_class(), gensym(MENUCLOSE));
    if ((t_gotfn)msg_onclose_menuclose != orgfun) {
        LIB_LOG << fmt::format("[{}] replace '{}' method for canvas:", OBJ_NAME, MENUCLOSE);

        class_addmethod(canvas_get_class(), (t_method)msg_onclose_menuclose, gensym(MENUCLOSE), A_DEFFLOAT, 0);
        class_addmethod(canvas_get_class(), (t_method)orgfun, gensym(ORIG_MENUCLOSE), A_DEFFLOAT, 0);
    }
}
