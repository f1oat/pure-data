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
#include "ui_filter.h"
#include "ceammc_ui.h"
#include "ui_filter.tcl.h"

UIFilter::UIFilter()
{
    createOutlet();
}

void setup_ui_filter()
{
    ui_filter_tcl_output();

    UIObjectFactory<UIFilter> obj("ui.filter", EBOX_GROWINDI);
    ui::TFilter<UIObject>::setup(obj);
}
