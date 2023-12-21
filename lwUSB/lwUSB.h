/*
 * lwUSB.h
 *
 *  Created on: Dec 17, 2022
 *      Author: Garmoosh
 */

#ifndef LWUSB_H_
#define LWUSB_H_

#include "Include/lwUSB_Std.h"

/* CONSTANTS Exposed to the USER */

#define LWUSB_ISOCHRONOUS_MAX_PERIOD_MS 32768u
#define LWUSB_ENDPOINT_MAX_PERIOD_MS 255u
#define LWUSB_EP_TYPE_MAX 3u
#define LWUSB_EP_DIRECTION_MAX 1u
#define LWUSB_ENDPOINT_MAX_NUMBER 8u
#define LWUSB_STRINGD_ENCODE_RAW   0u
#define LWUSB_STRINGD_ENCODE_UTF16 1u


/* TYPES Exposed to the USER */


/* OTHERS to USER */

#include "HAL/Hal.h"



#endif /* LWUSB_H_ */
