#include "list_pass.h"
#include "ceammc_containers.h"
#include "ceammc_factory.h"
#include "ceammc_fn_list.h"
#include "datatype_mlist.h"

constexpr size_t BIN_SEARCH_SIZE = 16;

ListPass::ListPass(const PdArgs& args)
    : BaseObject(args)
    , args_(nullptr)
{
    createInlet();
    createOutlet();

    args_ = new ListProperty("@args");
    args_->setArgIndex(0);
    args_->setSuccessFn([this](Property* p) {
        const auto old_size = args_->value().size();
        args_->setValue(list::uniqueSorted(args_->value()));
        if (old_size != args_->value().size())
            OBJ_ERR << "duplicates in pass list, fixed";
    });
    addProperty(args_);
}

void ListPass::onFloat(t_float f)
{
    if (contains(Atom(f)))
        floatTo(0, f);
}

void ListPass::onSymbol(t_symbol* s)
{
    if (contains(Atom(s)))
        symbolTo(0, s);
}

void ListPass::onList(const AtomListView& lv)
{
    SmallAtomList res;
    res.reserve(lv.size());

    for (auto& a : lv) {
        if (contains(a))
            res.push_back(a);
    }

    listTo(0, res.view());
}

void ListPass::onInlet(size_t n, const AtomListView& lv)
{
    args_->set(lv);
}

void ListPass::onDataT(const MListAtom& ml)
{
    MListAtom res;
    res->reserve(ml->size());

    for (auto& a : *ml) {
        if (contains(a))
            res->append(a);
    }

    atomTo(0, res);
}

bool ListPass::contains(const Atom& a) const
{
    auto& l = args_->value();

    if (l.size() >= BIN_SEARCH_SIZE)
        return std::binary_search(l.begin(), l.end(), a);
    else
        return std::find(l.begin(), l.end(), a) != l.end();
}

void setup_list_pass()
{
    ObjectFactory<ListPass> obj("list.pass");
    obj.processData<DataTypeMList>();

    obj.setXletsInfo({ "float, symbol, list or Mlist", "list: set passed values" }, { "list: with passed values" });

    obj.setDescription("leave in list only specified elements");
    obj.setCategory("list");
    obj.setKeywords({"list", "pass", "filter"});
}
