/*****************************************************************************
 * Copyright 2024 Serge Poltavski. All rights reserved.
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
#ifndef HW_GAMEPAD_H
#define HW_GAMEPAD_H

#include "ceammc_object.h"
#include "ceammc_poll_dispatcher.h"
#include "hw_rust.hpp"

using namespace ceammc;

namespace ceammc {
namespace gp {

    struct GamepadImpl {
        ceammc_hw_gamepad* gp_ { nullptr };
        std::function<void(const char*)> cb_err;
        std::function<void(const ceammc_hw_gamepad_event&)> cb_event;
        std::function<void(const ceammc_gamepad_dev_info&)> cb_devlist;

        GamepadImpl(SubscriberId id, size_t poll_ms = 20);
        ~GamepadImpl();

        void processEvents();
        void listDevices();

        static void on_error(void* user, const char* msg);
        static void on_event(void* user, const ceammc_hw_gamepad_event* ev);
        static void on_devlist(void* user, const ceammc_gamepad_dev_info* info);
    };
}
}

using HwGamepadBase = DispatchedObject<BaseObject>;

class HwGamepad : public HwGamepadBase {
    std::unique_ptr<gp::GamepadImpl> gp_;

public:
    HwGamepad(const PdArgs& args);

    bool notify(int code) final;
    void m_devices(t_symbol* s, const AtomListView& lv);
};

void setup_hw_gamepad();

#endif // HW_GAMEPAD_H
