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
#include "string_format.h"
#include "ceammc_format.h"
#include "data_string_factory.h"

#include <sstream>
#include <stdexcept>

#define TINYFORMAT_ERROR(msg)          \
    {                                  \
        throw std::runtime_error(msg); \
    }

#include "tinyformat.h"
#include "vformatlist.h"

StringFormat::StringFormat(const PdArgs& a)
    : BaseObject(a)
    , fmt_result_(new String(new DataString("")))
{
    createOutlet();

    propSetFormat(positionalArguments());
    createCbProperty("@format", &StringFormat::propGetFormat, &StringFormat::propSetFormat);
}

void StringFormat::onBang()
{
    if (!fmt_result_)
        return;

    atomTo(0, fmt_result_->toAtom());
}

void StringFormat::onData(const DataString& d)
{
    try {
        fmt_result_->data()->str() = tfm::format(fmt_str_.c_str(), d.str());
    } catch (std::exception& e) {
        OBJ_ERR << e.what();
        return;
    }

    onBang();
}

void StringFormat::onFloat(float v)
{
    try {
        fmt_result_->data()->str() = tfm::format(fmt_str_.c_str(), v);
    } catch (std::exception& e) {
        OBJ_ERR << e.what();
        return;
    }

    onBang();
}

void StringFormat::onSymbol(t_symbol* s)
{
    try {
        fmt_result_->data()->str() = tfm::format(fmt_str_.c_str(), s->s_name);
    } catch (std::exception& e) {
        OBJ_ERR << e.what();
        return;
    }

    onBang();
}

void StringFormat::onList(const AtomList& lst)
{
    VFormatList args;
    for (size_t i = 0; i < lst.size(); i++) {
        switch (lst[i].type()) {
        case Atom::FLOAT:
            args.add(lst[i].asFloat());
            break;
        case Atom::SYMBOL:
        case Atom::PROPERTY:
            args.add(std::string(lst[i].asSymbol()->s_name));
            break;
        }
    }

    try {
        std::ostringstream buf;
        tfm::vformat(buf, fmt_str_.c_str(), args);
        fmt_result_->data()->str() = buf.str();
    } catch (std::exception& e) {
        OBJ_ERR << e.what();
        return;
    }

    onBang();
}

AtomList StringFormat::propGetFormat() const
{
    return fmt_atoms_;
}

void StringFormat::propSetFormat(const AtomList& lst)
{
    fmt_atoms_ = lst;
    fmt_str_ = to_string(lst, " ");
}

extern "C" void setup_string0x2eformat()
{
    DataStringFactory<StringFormat, DataString> obj("string.format");
    obj.addAlias("str.format");
}
