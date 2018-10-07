/*****************************************************************************
 * Copyright 2018 Serge Poltavsky. All rights reserved.
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
#ifndef PROP_DECLARE_H
#define PROP_DECLARE_H

#include "ceammc_object.h"
#include "datatype_property.h"

using namespace ceammc;

class PropDeclare : public BaseObject {
    t_symbol* sym_name_;
    t_symbol* sym_full_name_;
    std::string full_name_;
    SymbolEnumProperty* type_;
    FloatProperty* min_;
    FloatProperty* max_;
    ListProperty* enum_;
    ListProperty* default_;
    DataTypeProperty* pprop_;

public:
    PropDeclare(const PdArgs& args);
    ~PropDeclare();

    void parseProperties() override;
    t_symbol* name() const;
    t_symbol* fullName() const;

    bool isFloat() const;
    bool isInt() const;
    bool isBool() const;
    bool isSymbol() const;
    bool isList() const;

public:
    static t_symbol* className;

private:
    void initName();
};

void setup_prop_declare();

#endif // PROP_DECLARE_H
