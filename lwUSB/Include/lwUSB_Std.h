/*
 * lwUSB_Std.h
 *
 *  Created on: Jun 28, 2023
 *      Author: Garmoosh
 */

#ifndef LWUSB_STD_H_
#define LWUSB_STD_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/* MAXIMUM Value Allowed By Stack */
#define LWUSB_MAX_EP_N     8u
#define LWUSB_MAX_CONFIG_N 1u
#define LWUSB_MAX_INTF_N   1u

/* MINIMUM Value Allowed By Stack */
#define LWUSB_MIN_EP_N     1u
#define LWUSB_MIN_CONFIG_N 1u
#define LWUSB_MIN_INTF_N   1u

/* Default Values */
#define LWUSB_DEFAULT_EP_TX_BS 256u
#define LWUSB_DEFAULT_EP_RX_BS 256u

#define LWUSB_ENUM_TYPE_S 32u
#define LWUSB_BUS_EVENTS_S 0u
#define LWUSB_INTERNAL_EVENTS_S 33u


#define WEAK __attribute__((weak))

#endif /* LWUSB_STD_H_ */
