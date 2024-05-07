/*****************************************************************************
 * Copyright 2019 Serge Poltavsky. All rights reserved.
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
#ifndef HW_CPU_TEMP_H
#define HW_CPU_TEMP_H

#include "ceammc_object.h"
#include "ceammc_poll_dispatcher.h"
#include "hw_rust.hpp"

using namespace ceammc;

class HwCpuTemp : public DispatchedObject<BaseObject> {
    std::unique_ptr<ceammc_hw_cputemp, void (*)(ceammc_hw_cputemp*)> cpu_temp_;

public:
    HwCpuTemp(const PdArgs& args);
    void onBang() override;
    bool notify(int) final;
};

void setup_hw_cpu_temp();

#endif // HW_CPU_TEMP_H
