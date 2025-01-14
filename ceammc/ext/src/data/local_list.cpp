#include "local_list.h"
#include "ceammc_factory.h"
#include "data_protocol.h"
#include "datatype_mlist.h"

LocalList::LocalList(const PdArgs& a)
    : LocalListBase(a)
{
    createCbListProperty(
        "@value",
        [this]() -> AtomList { return list(); },
        [this](const AtomListView& lv) -> bool { list() = lv; return true; })
        ->setArgIndex(1);

    createOutlet();
}

EditorTitleString LocalList::editorTitle() const
{
    char buf[EditorTitleMaxLength];
    snprintf(buf, sizeof(buf) - 1, "LOCAL.LIST (%s)", this->id()->s_name);
    return buf;
}

void setup_local_list()
{
    ListIFaceFactory<LocalList> obj("local.list");
    obj.processData<DataTypeMList>();

    LocalList::factoryEditorObjectInit(obj);

    obj.setDescription("local named list object");
    obj.setCategory("local");
    obj.setKeywords({ "list", "global" });
}
