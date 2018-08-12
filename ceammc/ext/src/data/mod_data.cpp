#include "mod_data.h"
#include "data_dict.h"
#include "data_float.h"
#include "data_int.h"
#include "data_list.h"
#include "data_mlist.h"
#include "data_set.h"
#include "dict_contains.h"
#include "dict_keys.h"
#include "dict_size.h"
#include "dict_values.h"
#include "global_dict.h"
#include "global_float.h"
#include "global_int.h"
#include "global_list.h"
#include "global_mlist.h"
#include "global_set.h"
#include "local_dict.h"
#include "local_float.h"
#include "local_int.h"
#include "local_list.h"
#include "local_mlist.h"
#include "local_set.h"
#include "mlist_flatten.h"
#include "set_contains.h"
#include "set_difference.h"
#include "set_equal.h"
#include "set_intersection.h"
#include "set_size.h"
#include "set_symdiff.h"
#include "set_to_list.h"
#include "set_union.h"

extern "C" void setup_data0x2efifo();

void ceammc_data_setup()
{
    setup_data0x2efifo();

    setup_set0x2econtains();
    setup_set0x2edifference();
    setup_set0x2eequal();
    setup_set0x2eintersection();
    setup_set0x2elist();
    setup_set0x2esize();
    setup_set0x2eunion();
    setup_set0x2esymdiff();

    setup_data_dict();
    setup_data_float();
    setup_data_int();
    setup_data_list();
    setup_data_mlist();
    setup_data_set();

    setup_dict_contains();
    setup_dict_keys();
    setup_dict_size();
    setup_dict_values();

    setup_local_dict();
    setup_local_float();
    setup_local_int();
    setup_local_list();
    setup_local_mlist();
    setup_local_set();

    setup_global_dict();
    setup_global_float();
    setup_global_int();
    setup_global_list();
    setup_global_mlist();
    setup_global_set();

    setup_mlist_flatten();
}
