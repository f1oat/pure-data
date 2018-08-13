#include "preset_list.h"
#include "ceammc_factory.h"
#include "preset_iface.h"

PresetList::PresetList(const PdArgs& args)
    : PresetBase(args)
    , init_(0)
{
    // 1st positionalSymbolArgument is preset name - in base class
    // positional arguments from 2 we are using as init arguments
    init_ = new ListProperty("@init", positionalArguments().slice(1));
    current_value_ = init_->value();
    createProperty(init_);
}

void PresetList::onList(const AtomList& lst)
{
    current_value_ = lst;
}

void PresetList::loadFrom(size_t idx)
{
    current_value_ = loadList(idx, init_->value());
    listTo(0, current_value_);
}

void PresetList::storeAt(size_t idx)
{
    storeList(current_value_, idx);
}

void setup_preset_list()
{
    PresetIFaceFactory<PresetList> obj("preset.list");
}
