/*
 * lwUSB.h
 *
 *  Created on: Dec 17, 2022
 *      Author: Garmoosh
 */

#ifndef LWUSB_H_
#define LWUSB_H_

#include "Include/lwUSB_Std.h"
#include "tree.h"


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

lwUSB_Err lwUSB_Init(void);
lwUSB_Err lwUSB_Main(void);
Handle lwUSB_CreatePhysicalEndPoint( uint8_t epNum , uint32_t BufferSz , boolean isDouble , uint8_t * out_b , uint8_t * in_b , uint32_t b_size );
Handle lwUSB_CreateEndpoint( Handle phyHandle , uint8_t epAddress ,  e_lwUSB_EndPoint_Type epType ,  e_lwUSB_EndPoint_Direction epDir , uint16_t maxPacketSize , uint16_t pollTms );
Handle lwUSB_CreateString( uint8_t * stringContent , uint32_t stringLength  , uint8_t stringID , enum lwUSB_String_Encoding_e enc );
Handle lwUSB_CreateInterface ( uint8_t itfNumber , uint8_t itfClass , uint8_t  itfSubClass ,  uint8_t itfProtocol , uint8_t itfNumSettings );
Handle lwUSB_CreateConfiguration ( uint8_t cfgNumber , uint8_t cfgMaxPower , boolean cfgSelfPowered , boolean cfgRemoteWakeup  );
lwUSB_Err lwUSB_RegisterEndPoint ( Handle h_EndPoint , Handle h_Interface );
lwUSB_Err lwUSB_RegisterInterface ( Handle h_Interface , Handle h_Configuration );
lwUSB_Err lwUSB_RegisterConfiguration ( Handle h_Configuration );


#endif /* LWUSB_H_ */
