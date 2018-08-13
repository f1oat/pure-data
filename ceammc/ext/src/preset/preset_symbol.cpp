#include "preset_symbol.h"
#include "ceammc_factory.h"
#include "preset_iface.h"

PresetSymbol::PresetSymbol(const PdArgs& args)
    : PresetBase(args)
{
    // 1st positionalSymbolArgument is preset name - in base class
    // 2nd positionalSymbolArgument weare using as init value
    init_ = new SymbolProperty("@init", positionalSymbolArgument(1, &s_), true);
    current_value_ = init_->value();
    createProperty(init_);
}

void PresetSymbol::onSymbol(t_symbol* s)
{
    current_value_ = s;
}

void PresetSymbol::loadFrom(size_t idx)
{
    current_value_ = loadSymbol(idx, init_->value());
    symbolTo(0, current_value_);
}

void PresetSymbol::storeAt(size_t idx)
{
    storeSymbol(current_value_, idx);
}

void setup_preset_symbol()
{
    PresetIFaceFactory<PresetSymbol> obj("preset.symbol");
}
