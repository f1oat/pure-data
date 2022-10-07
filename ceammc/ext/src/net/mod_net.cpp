#include "mod_net.h"
#include "net_host.h"

extern void setup_net_osc_receive();
extern void setup_net_osc_send();
extern void setup_net_osc_server();

void ceammc_net_setup()
{
    setup_net_host();
    setup_net_osc_send();
    setup_net_osc_receive();
    setup_net_osc_server();
}
