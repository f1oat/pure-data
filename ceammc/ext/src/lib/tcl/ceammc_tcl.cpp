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
#include "ceammc_tcl.h"
#include "ceammc_texteditor.tcl.h"
#include "ceammc_tooltips.tcl.h"

namespace ceammc {

void ceammc_tcl_init()
{
    ceammc_texteditor_tcl_output();
}

void ceammc_tcl_init_tooltips()
{
    ceammc_tooltips_tcl_output();
}

}
