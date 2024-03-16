#include "mod_net.h"
#include "net_host.h"

extern void setup_net_artnet_send();
extern void setup_net_http_send();
extern void setup_net_mqtt_client();
extern void setup_net_osc_receive();
extern void setup_net_osc_send();
extern void setup_net_osc_server();
extern void setup_net_zeroconf();

void ceammc_net_setup()
{
    setup_net_artnet_send();
    setup_net_host();
    setup_net_http_send();
    setup_net_mqtt_client();
    setup_net_osc_receive();
    setup_net_osc_send();
    setup_net_osc_server();
    setup_net_zeroconf();
}
