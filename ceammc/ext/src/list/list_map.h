#ifndef LIST_MAP_H
#define LIST_MAP_H

#include "ceammc_data.h"
#include "ceammc_object.h"
#include "datatype_property.h"
using namespace ceammc;

class ListMap : public BaseObject {
    DictAtom dict_;

public:
    ListMap(const PdArgs& args);

    void onFloat(t_float f) override;
    void onSymbol(t_symbol* s) override;
    void onList(const AtomList& lst) override;
    void onInlet(size_t n, const AtomListView& lv) override;
};

void setup_list_map();

#endif // LIST_MAP_H
