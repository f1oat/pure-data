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
#include "canvas_path.h"
#include "ceammc_canvas.h"
#include "ceammc_factory.h"

CanvasPath::CanvasPath(const PdArgs& args)
    : BaseObject(args)
    , path_(nullptr)
{
    createOutlet();
}

void CanvasPath::onBang()
{
    if (!path_) {
        std::string path = canvas_info_dir(canvas())->s_name;
        if (!path.empty())
            path += '/';

        path += canvas_info_name(canvas())->s_name;
        path_ = gensym(path.c_str());
    }

    symbolTo(0, path_);

}

void setup_base_canvas_path()
{
    ObjectFactory<CanvasPath> obj("canvas.path");
}
