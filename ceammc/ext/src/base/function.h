/*****************************************************************************
 * Copyright 2017 Serge Poltavsky. All rights reserved.
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
#ifndef FUNCTION_H
#define FUNCTION_H

#include "ceammc_object.h"

using namespace ceammc;

class Function : public BaseObject {
    t_symbol* name_;
    Message result_;

public:
    Function(const PdArgs& a);
    ~Function();
    void onBang() override;
    void onFloat(t_float f) override;
    void onSymbol(t_symbol* s) override;
    void onList(const AtomList& l) override;

    void onInlet(size_t n, const AtomList& l) override;
    Message& result();
    const Message& result() const;

public:
    static bool exists(t_symbol* name);
    static Function* function(t_symbol* name);
};

void function_setup();

#endif // FUNCTION_H
