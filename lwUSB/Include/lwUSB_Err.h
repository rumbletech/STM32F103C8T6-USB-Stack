/*
 * lwUSB_Err.h
 *
 *  Created on: Jun 28, 2023
 *      Author: Garmoosh
 */

#ifndef LWUSB_ERR_H_
#define LWUSB_ERR_H_

#include <stdint.h>
#include "lwUSB_Std.h"
/* Error Types */

#define ERR_NULL  2
#define ERR_FAULT 1
#define ERR_OK    0

typedef int32_t lw_err ;


enum lwUSB_InternalEvents_e {
	lwUSB_InternalEvents_e_Start = LWUSB_INTERNAL_EVENTS_S,
	lwUSB_InternalEvents_e_OVF ,
	lwUSB_InternalEvents_e_End = LWUSB_INTERNAL_EVENTS_S +  LWUSB_ENUM_TYPE_S + 1u ,
};

#endif /* LWUSB_ERR_H_ */
