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

enum lwUSB_BusEvent_e {
	lwUSB_BusEvent_e_Start = LWUSB_BUS_EVENTS_S,
	lwUSB_BusEvent_e_RESET,
	lwUSB_BusEvent_e_SUSPEND,
	lwUSB_BusEvent_e_RX,
	lwUSB_BusEvent_e_TX,
	lwUSB_BusEvent_e_SetAddress,
	lwUSB_BusEvent_e_SETUP,
	lwUSB_BusEvent_e_ERR,
	lwUSB_BusEvent_e_WAKEUP,
	lwUSB_BusEvent_e_End = LWUSB_BUS_EVENTS_S  + LWUSB_ENUM_TYPE_S + 1u,
};

/* lwUSB shall Support UTF16 and ASCII Strings ,
 * if lwUSB_String_Encoding_e_ASCII is used , then lwUSB Shall Make Conversion to UTF16 On Sending the String and Descriptors.
 * if lwUSB_String_Encoding_e_UTF16 is used , then lwUSB Shall assume the character array to be already encoded and shall transmit as is.
 */
enum lwUSB_String_Encoding_e {
	lwUSB_String_Encoding_e_Start = LWUSB_ENUM_STRING_ENCODING_S,
	lwUSB_String_Encoding_e_ASCII,
	lwUSB_String_Encoding_e_UTF16,
	lwUSB_String_Encoding_e_End = LWUSB_ENUM_STRING_ENCODING_S  + LWUSB_ENUM_TYPE_S + 1u,
};

/* OTHERS to USER */

#include "HAL/Hal.h"



#endif /* LWUSB_H_ */
