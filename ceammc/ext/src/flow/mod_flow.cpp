#include "mod_flow.h"

#include "flow_append.h"
#include "flow_change.h"
#include "flow_count.h"
#include "flow_demultiplex.h"
#include "flow_gate.h"
#include "flow_group.h"
#include "flow_less.h"
#include "flow_less_eq.h"
#include "flow_multiplex.h"
#include "flow_pass.h"
#include "flow_pass_if.h"
#include "flow_reject.h"
#include "flow_reject_if.h"
#include "flow_speedlim.h"
#include "flow_split.h"
#include "flow_sync.h"
#include "flow_tee_tilde.h"

void ceammc_flow_setup()
{
    setup_flow_append();
    setup_flow0x2echange();
    setup_flow_count();
    setup_flow_demultiplex();
    setup_flow0x2egroup();
    setup_flow_less();
    setup_flow_less_eq();
    setup_flow0x2epass();
    setup_flow0x2epass_if();
    setup_flow0x2ereject();
    setup_flow0x2ereject_if();
    setup_flow_speedlim();
    setup_flow0x2esplit();
    setup_flow0x2esync();
    setup_flow0x2etee_tilde();
    setup_flow_multiplex();
    setup_flow_gate();
}
