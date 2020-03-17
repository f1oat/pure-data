/*****************************************************************************
 * Copyright 2018 Serge Poltavsky. All rights reserved.
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

#include "../mod_init.h"
#include "ceammc_pd.h"
#include "ceammc_platform.h"
#include "datatype_tree_imp.h"
#include "m_pd.h"
#include "stk/stk/include/Stk.h"
#include "json/json.hpp"

using json = nlohmann::json;

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

static t_symbol* any = &s_anything;
extern "C" void pd_init();
t_class* ceammc_class = nullptr;

using namespace std;
using namespace ceammc;

int main(int argc, char* argv[])
{
    pd_init();
    ceammc_init();

    if (argc < 2) {
        cerr << "usage: " << platform::basename(argv[0]) << " OBJECT_NAME" << endl;
        return 1;
    }

    CanvasPtr cnv = PureData::instance().createTopCanvas("/test_canvas");

    if (!cnv) {
        cerr << "can't create root canvas" << endl;
        return 2;
    }

    AtomList args;
    if (argc > 2) {
        t_binbuf* b = binbuf_new();

        std::string str;
        for (int i = 2; i < argc; i++) {
            str += argv[i];
            str += " ";
        }

        binbuf_text(b, str.c_str(), str.size());

        int n = binbuf_getnatom(b);

        for (int i = 0; i < n; i++)
            args.append(binbuf_getvec(b)[i]);

        binbuf_free(b);
    }

    // stk rawwaves path
    if (getenv("RAWWAVES") != nullptr) {
        stk::Stk::setRawwavePath(getenv("RAWWAVES"));
    }

    pd::External ext(argv[1], args);
    if (!ext.object()) {
        cerr << "can't create object: " << argv[1] << endl;
        return 3;
    }

    json j;
    json jobj;

    jobj["name"] = pd::object_name(ext.object())->s_name;
    jobj["dir"] = pd::object_dir(ext.object())->s_name;
    jobj["inlets"] = ext.numInlets();
    jobj["outlets"] = ext.numOutlets();
    for (auto m : ext.methods()) {
        const char* name = m->s_name;
        if (name[0] == '@' || name[0] == '.')
            continue;

        jobj["methods"].push_back(name);
    }

    for (PropertyInfo i : ext.properties()) {
        DataTypeTree info = i.info();
        jobj["properties"].push_back(info.internalPimpl()->internalJson());
    }

    j["object"] = jobj;
    cout << j.dump(2);

    return 0;
}
