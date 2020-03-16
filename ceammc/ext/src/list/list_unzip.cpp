//
//  list_unzip.cpp
//  pd_ext
//
//  Created by Alex Nadzharov on 23/12/16.
//
//

#include "list_unzip.h"
#include "ceammc_convert.h"
#include "ceammc_factory.h"
#include "ceammc_fn_list.h"
#include "ceammc_log.h"
#include "datatype_mlist.h"

using namespace ceammc;

constexpr size_t DEFAULT_OUTLET = 2;
constexpr size_t MIN_OUTLET = 2;
constexpr size_t MAX_OUTLET = 20;

static t_symbol* SYM_MIN;
static t_symbol* SYM_PAD;

ListUnzip::ListUnzip(const PdArgs& a)
    : ListBase(a)
    , n_(positionalConstant<DEFAULT_OUTLET, MIN_OUTLET, MAX_OUTLET>(0))
    , pad_(0.f)
    , method_(0)
{

    for (size_t i = 0; i < n_; i++) {
        out_lists_.push_back(AtomList());
        createOutlet();
    }

    method_ = new SymbolEnumProperty("@method", { SYM_MIN, SYM_PAD });
    addProperty(method_);
    addProperty(new SymbolEnumAlias("@min", method_, SYM_MIN));

    createCbAtomProperty(
        "@pad",
        [this]() -> Atom { return pad_; },
        [this](const Atom& a) -> bool {
        pad_ = a;
        method_->setValue(SYM_PAD);
        return true; });
}

void ListUnzip::onBang()
{
    for (size_t i = out_lists_.size(); i > 0; i--)
        listTo(i - 1, out_lists_[i - 1]);
}

void ListUnzip::onList(const AtomList& l)
{
    clearOutputList();

    if (method_->value() == SYM_MIN)
        list::deinterleaveMinLength(l, out_lists_);
    else if (method_->value() == SYM_PAD)
        list::deinterleavePadWith(l, pad_, out_lists_);

    onBang();
}

void ListUnzip::clearOutputList()
{
    for (size_t i = 0; i < out_lists_.size(); i++)
        out_lists_[i].clear();
}

void setup_list_unzip()
{
    SYM_MIN = gensym("min");
    SYM_PAD = gensym("pad");

    ObjectFactory<ListUnzip> obj("list.unzip");
    obj.addAlias("list.deinterleave");
    obj.processData<DataTypeMList>();
}
