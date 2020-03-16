#include "mod_data.h"
#include "data_dict.h"
#include "data_fifo.h"
#include "data_float.h"
#include "data_int.h"
#include "data_list.h"
#include "data_mlist.h"
#include "data_set.h"
#include "data_tree.h"
#include "dict_contains.h"
#include "dict_each.h"
#include "dict_from_list.h"
#include "dict_get.h"
#include "dict_is.h"
#include "dict_keys.h"
#include "dict_pass.h"
#include "dict_reject.h"
#include "dict_size.h"
#include "dict_to_list.h"
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
#include "tree_at.h"
#include "tree_float.h"
#include "tree_keys.h"
#include "tree_size.h"
#include "tree_symbol.h"

void ceammc_data_setup()
{

    setup_set0x2econtains();
    setup_set0x2edifference();
    setup_set0x2eequal();
    setup_set0x2eintersection();
    setup_set0x2elist();
    setup_set0x2esize();
    setup_set0x2eunion();
    setup_set0x2esymdiff();

    setup_data_dict();
    setup_data_fifo();
    setup_data_float();
    setup_data_tree();
    setup_data_int();
    setup_data_list();
    setup_data_mlist();
    setup_data_set();

    setup_dict_contains();
    setup_dict_each();
    setup_dict_from_list();
    setup_dict_get();
    setup_dict_keys();
    setup_dict_pass();
    setup_dict_reject();
    setup_dict_size();
    setup_dict_to_list();
    setup_dict_values();
    setup_is_dict();

    setup_tree_at();
    setup_tree_float();
    setup_tree_keys();
    setup_tree_size();
    setup_tree_symbol();

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
