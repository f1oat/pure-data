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
#ifndef METRO_WCHOICE_H
#define METRO_WCHOICE_H

#include "ceammc_clock.h"
#include "ceammc_object.h"
#include "ceammc_random.h"

#include <random>

using namespace ceammc;

class MetroWChoice : public BaseObject {
    ClockLambdaFunction clock_;
    random::RandomGen gen_;
    ListProperty* timeset_;
    ListProperty* weights_;
    std::discrete_distribution<int> dist_;

public:
    MetroWChoice(const PdArgs& args);
    void onFloat(t_float f) final;
    void onInlet(size_t n, const AtomListView& lv) final;

private:
    bool updateDistribution(const AtomListView& lv);
};

void setup_metro_wchoice();

#endif // METRO_WCHOICE_H
