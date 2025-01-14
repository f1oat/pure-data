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
#ifndef CONV_COLOR2RGBA_H
#define CONV_COLOR2RGBA_H

#include "ceammc_object.h"
#include "ceammc_property_enum.h"
#include "datatype_color.h"
#include "lex/parser_color.h"
using namespace ceammc;

class ConvColor2RGBA : public BaseObject {
    parser::ColorFullMatch parser_;
    SymbolEnumProperty* mode_ { nullptr };
    FlagProperty* pack_ { nullptr };

public:
    ConvColor2RGBA(const PdArgs& args);

    void initDone() override;

    void onSymbol(t_symbol* s) override;
    void onAny(t_symbol* s, const AtomListView& lv) override;

    void onDataT(const DataAtom<DataTypeColor>& data);

    const char* annotateOutlet(size_t n) const final;
};

void setup_conv_color2rgba();

#endif // CONV_COLOR2RGBA_H
