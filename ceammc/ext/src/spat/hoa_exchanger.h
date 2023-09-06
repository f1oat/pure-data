/*****************************************************************************
 * Copyright 2023 Serge Poltavsky. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#ifndef HOA_EXCHANGER_H
#define HOA_EXCHANGER_H

#include <array>
#include <memory>

#include "ceammc_property_enum.h"
#include "hoa_common.h"
using namespace ceammc;

class HoaExchanger : public HoaBase {
    Buffer in_buf_;
    Buffer out_buf_;
    std::unique_ptr<Exchanger2d> hoa_;
    SymbolEnumProperty* num_ { nullptr };
    SymbolEnumProperty* norm_ { nullptr };

public:
    HoaExchanger(const PdArgs& args);
    void initDone() override;

    void processBlock(const t_sample** in, t_sample** out) override;
    void blockSizeChanged(size_t bs) override;

    const char* annotateInlet(size_t n) const override;
    const char* annotateOutlet(size_t n) const override;

public:
    static void initAnnotations();

private:
    constexpr static size_t ANNOT_LEN = 32;
    static std::array<char[ANNOT_LEN], HOA_MAX_ORDER> xlet_annotations_;
};

void setup_spat_hoa_exchanger();

#endif // HOA_EXCHANGER_H
