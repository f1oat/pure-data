/*****************************************************************************
 * Copyright 2024 Serge Poltavski. All rights reserved.
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
#ifndef HW_PRINTER_H
#define HW_PRINTER_H

#include <boost/variant.hpp>

#include "ceammc_pollthread_spsc.h"
#include "hw_rust.hpp"

namespace ceammc {
namespace printer {

    struct PrinterInfo {
        std::string name;
        std::string system_name;
        std::string driver_name;
        std::string uri;
        std::string location;
        bool is_default;
        bool is_shared;
        ceammc_hw_printer_state state;
    };

    using PrinterList = std::vector<PrinterInfo>;

    namespace req {
        struct ListPrinters { };
        struct PrintFile {
            std::string printer_name;
            std::string path;
        };
    }

    namespace reply {
    }

    using Request = boost::variant<req::ListPrinters, req::PrintFile>;
    using Reply = boost::variant<PrinterList>;
}
}

using namespace ceammc;

using HwPrinterBase = FixedSPSCObject<printer::Request, printer::Reply>;

class HwPrinter : public HwPrinterBase {
public:
    HwPrinter(const PdArgs& args);

    void processRequest(const printer::Request& req, ResultCallback cb) final;
    void processResult(const printer::Reply& res) final;

    void m_devices(t_symbol* s, const AtomListView& lv);
    void m_print(t_symbol* s, const AtomListView& lv);
};

void setup_hw_printer();

#endif // HW_PRINTER_H
