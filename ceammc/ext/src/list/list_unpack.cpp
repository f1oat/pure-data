#include "list_unpack.h"
#include "ceammc_convert.h"
#include "ceammc_factory.h"
#include "datatype_mlist.h"

#include <algorithm>

constexpr size_t MIN_OUTLETS = 1;
constexpr size_t MAX_OUTLETS = 32;
constexpr size_t DEFAULT_OUTLETS = 1;

ListUnpack::ListUnpack(const PdArgs& a)
    : BaseObject(a)
    , n_(positionalConstant<DEFAULT_OUTLETS, MIN_OUTLETS, MAX_OUTLETS>(0))
{
    for (size_t i = 0; i < n_; i++)
        createOutlet();
}

void ListUnpack::onList(const AtomList& l)
{
    const size_t N = std::min<size_t>(l.size(), n_);

    for (size_t i = N; i > 0; i--)
        atomTo(i - 1, l[i - 1]);
}

void ListUnpack::onDataT(const DataTPtr<DataTypeMList>& dptr)
{
    const size_t N = std::min<size_t>(dptr->size(), n_);
    for (size_t i = N; i > 0; i--)
        atomTo(i - 1, (*dptr)[i - 1].asAtom());
}

void setup_list_unpack()
{
    ObjectFactory<ListUnpack> obj("list.unpack");
    obj.processData<DataTypeMList>();
}
