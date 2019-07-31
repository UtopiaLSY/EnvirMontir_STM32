#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__
#include "lwip/err.h"
#include "lwip/netif.h"

#define IFNAME0     'L'
#define IFNAME1     'Y'
 

err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);


#endif
