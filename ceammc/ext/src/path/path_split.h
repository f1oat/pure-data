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
#ifndef PATH_SPLIT_H
#define PATH_SPLIT_H

#include "ceammc_data.h"
#include "ceammc_object.h"
#include "datatype_path.h"
using namespace ceammc;

class PathSplit : public BaseObject {
public:
    PathSplit(const PdArgs& arsgs);
    void onSymbol(t_symbol* s) override;
    void onDataT(const StringAtom& s);
    void onDataT(const PathAtom& p);
};

void setup_path_split();

#endif // PATH_SPLIT_H
