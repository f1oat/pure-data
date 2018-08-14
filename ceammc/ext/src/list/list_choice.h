#ifndef LIST_CHOICE_H
#define LIST_CHOICE_H

#include "ceammc_object.h"

using namespace ceammc;

class DataTypeMList;

class ListChoice : public BaseObject {
    BoolProperty* no_repeat_;
    size_t prev_idx_;

public:
    ListChoice(const PdArgs& a);
    void onList(const AtomList& l);
    void onDataT(const DataTypeMList& lst);
};

#endif // LIST_CHOICE_H
