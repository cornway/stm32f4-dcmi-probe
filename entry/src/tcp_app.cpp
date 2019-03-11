#include "VM.h"
#include "vmapi.h"
#include "fgpu.h"
#include "gui.h"
#include "app_def.h"

#include <lwip/inet.h>
#include <lwip/tcp.h>
#include <lwip/netif.h>
#include <lwip/init.h>
#include <lwip/stats.h>
#include <lwip/timers.h>
#include <netif/etharp.h>

#include "mchdrv.h"
#include "enc28j60.h"

#include "enc28j60_interface.h"

static void mch_net_init(void);
static void mch_net_poll(void);

ip_addr mch_myip_addr   = {0x0200a8c0UL}; /* 192.168.0.2 */
ip_addr gw_addr         = {0x0100a8c0UL}, netmask = {0x000000ffUL}; /* 192.168.0.1 */

static struct netif mchdrv_netif;
static enc_device_t mchdrv_hw;

INT_T tcp_app (void *argv)
{
    enc28j60_ll_init();
    mch_net_init();
    for (;;) {
        mch_net_poll();
        sys_check_timeouts();
    }
}


static void mch_net_init(void)
{
    // Initialize LWIP
    lwip_init();

    mchdrv_netif.hwaddr_len = 6;
    /* demo mac address */
    mchdrv_netif.hwaddr[0] = 0;
    mchdrv_netif.hwaddr[1] = 1;
    mchdrv_netif.hwaddr[2] = 2;
    mchdrv_netif.hwaddr[3] = 3;
    mchdrv_netif.hwaddr[4] = 4;
    mchdrv_netif.hwaddr[5] = 5;

    // Add our netif to LWIP (netif_add calls our driver initialization function)
    if (netif_add(&mchdrv_netif, &mch_myip_addr, &netmask, &gw_addr, &mchdrv_hw,
                mchdrv_init, ethernet_input) == NULL) {
        LWIP_ASSERT("mch_net_init: netif_add (mchdrv_init) failed\n", 0);
    }

    netif_set_default(&mchdrv_netif);
    netif_set_up(&mchdrv_netif);
}

static void mch_net_poll(void)
{
    mchdrv_poll(&mchdrv_netif);
}


