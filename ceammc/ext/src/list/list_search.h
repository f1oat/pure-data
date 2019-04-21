#ifndef LIST_SEARCH_H
#define LIST_SEARCH_H

#include "ceammc_dataatomlist.h"
#include "list_base.h"

using namespace ceammc;

class ListSearch : public ListBase {
    DataAtomList subj_;

public:
    ListSearch(const PdArgs& args);
    void parseProperties() override;

    void onList(const AtomList& lst) override;
    void onInlet(size_t n, const AtomList& lst) override;
};

void setup_list_search();

#endif // LIST_SEARCH_H
