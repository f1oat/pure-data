#include "list_unique.h"
#include "ceammc_factory.h"
#include "ceammc_fn_list.h"
#include "datatype_mlist.h"

ListUnique::ListUnique(const ceammc::PdArgs& a)
    : BaseObject(a)
    , stable_(nullptr)
{
    stable_ = new BoolProperty("@stable", true);
    addProperty(stable_);

    createOutlet();
}

void ListUnique::onList(const AtomListView& lv)
{
    if (stable_->value())
        listTo(0, list::uniqueStable(lv));
    else
        listTo(0, list::uniqueSorted(lv));
}

void ListUnique::onDataT(const MListAtom& ml)
{
    if (stable_->value())
        atomTo(0, MListAtom(list::uniqueStable(ml->data())));
    else
        atomTo(0, MListAtom(list::uniqueSorted(ml->data())));
}

void setup_list_unique()
{
    ObjectFactory<ListUnique> obj("list.unique");
    obj.processData<DataTypeMList>();

    obj.setDescription("removes duplicates from input list");
    obj.setCategory("list");
    obj.setKeywords({"list", "unique", "sort"});
}
