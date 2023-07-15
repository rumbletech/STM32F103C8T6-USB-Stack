/*
 * lwUSB.h
 *
 *  Created on: Dec 17, 2022
 *      Author: Garmoosh
 */

#ifndef LWUSB_H_
#define LWUSB_H_

#include "Include/lwUSB_SPECS.h"
#include "Include/lwUSB_Std.h"
#include <stddef.h>

/* Constants */
#define LWUSB_ISOCHRONOUS_MAX_PERIOD_MS 32768u
#define LWUSB_ENDPOINT_MAX_PERIOD_MS 255u
#define LWUSB_EP_TYPE_MAX 3u
#define LWUSB_EP_DIRECTION_MAX 1u
#define LWUSB_ENDPOINT_MAX_NUMBER 8u
#define LWUSB_STRINGD_ENCODE_RAW   0u
#define LWUSB_STRINGD_ENCODE_UTF16 1u

/* Macros*/
/* This Implementation Supports only :
 * Configurations with no class or vendor specific descriptors
 * it only implements configurations with defined interfaces .
 * it currently does not support double buffers.
 * it currently does not support alternate interfaces
 *
 */


enum lwUSB_BusEvent_e {
	lwUSB_BusEvent_e_Start = LWUSB_BUS_EVENTS_S,
	lwUSB_BusEvent_e_RESET,
	lwUSB_BusEvent_e_SUSPEND,
	lwUSB_BusEvent_e_RX,
	lwUSB_BusEvent_e_TX,
	lwUSB_BusEvent_e_SETUP,
	lwUSB_BusEvent_e_ERR,
	lwUSB_BusEvent_e_WAKEUP,
	lwUSB_BusEvent_e_End = LWUSB_BUS_EVENTS_S  + LWUSB_INTERNAL_EVENTS_S + 1u,
};

struct lwUSB_interface_s {


	struct lwUSB_interface_descriptor_s * d_interface ;
	struct lwUSB_string_s * string ;
	struct lwUSB_endpoint_s  * endpoint   ;
	struct lwUSB_interface_s * next ;

};
struct lwUSB_configuration_s {


	struct lwUSB_configuration_descriptor_s * d_configuration  ;
	struct lwUSB_interface_s * interface ;
	struct lwUSB_string_s * string ;
	struct lwUSB_configuration_s * next  ;

};

struct lwUSB_endpoint_s {

	struct lwUSB_endpoint_descriptor_s * d_endpoint ;
	struct lwUSB_endpoint_s * next ;


};

struct lwUSB_string_s {

	struct lwUSB_string_descriptor_s * d_string ;
	uint8_t sid ;
	struct lwUSB_string_s * next ;


};


typedef enum {

	e_lwUSB_EndPoint_Type_Control     = 0u  ,
	e_lwUSB_EndPoint_Type_Isochronous = 1u  ,
	e_lwUSB_EndPoint_Type_Bulk        = 2u  ,
	e_lwUSB_EndPoint_Type_Interrupt   = 3u

} e_lwUSB_EndPoint_Type ;


typedef enum {


	e_lwUSB_EndPoint_Direction_OUT     = 0u  ,
	e_lwUSB_EndPoint_Direction_IN      = 1u  ,
	e_lwUSB_EndPoint_Direction_INOUT   = 2u


} e_lwUSB_EndPoint_Direction ;


typedef enum {

	e_lwUSB_EndPoint_State_Disabled  = 0b00 ,
	e_lwUSB_EndPoint_State_Stall     = 0b01 ,
	e_lwUSB_EndPoint_State_Nak       = 0b10 ,
	e_lwUSB_EndPoint_State_Valid     = 0b11 ,

} e_lwUSB_EndPoint_State ;

typedef enum {

	e_lwUSB_transaction_state_null          = 0u ,
	e_lwUSB_transaction_state_setup_stage   = 1u ,
	e_lwUSB_transaction_state_status_stage =  2u ,
	e_lwUSB_transaction_state_data_in       = 3u ,
	e_lwUSB_transaction_state_data_out      = 4u ,

} e_lwUSB_transaction_state ;


struct lwUSB_transaction_s {
	/* Defines Transactions */
	uint8_t* payload_buffer                  ;						/* Pointer to the Buffer to Transmit From or Receive into */
	uint32_t payload_length                  ;						/* Total Transaction Length */
	uint32_t payload_ptr                     ;						/* Retains Completed Bytes Status */
	e_lwUSB_transaction_state current_state  ;						/* State of the Transaciton */
	e_lwUSB_transaction_state previous_state ;						/* Previous State of the Transaction , Useful for Control Eps */
	struct lwUSB_transaction_s * next        ;						/* Next Transaciton */

};

struct lwUSB_ep_ch_s {
	/* Low Level Communication Channel for EndPoints */
	e_lwUSB_EndPoint_State 	    EP_State ;							/* Current State */
	uint16_t         			EP_Size ;							/* Endpoint Buffer Size */
	uint8_t *       			EP_BufferHandle ;					/* Pointer to the Endpoint's Allocated buffer */
};

struct lwUSB_ep_s {
	/* EndPoint LOW Level Control Structure */
	uint32_t         			EP_isInitialized ;					/* Endpoint Initialized Check */
	e_lwUSB_EndPoint_Type   	EP_Type ;							/* Endpoint Type  Control , Bulk , Isochronous , Interrupt */
	e_lwUSB_EndPoint_Direction 	EP_Direction ;						/* In , out or INOUT */
	uint8_t         			EP_Number ;							/* Number of this Endpoint , this Refers to Endpoint Index in reference to the actual Number of EPS in the HW */
	uint8_t                     EP_Address ;						/* Address of the Endpoint , this Refers to the Actual Address the Endpoint is Addressed By the Host */
	/* Only one Channel is allowed for an Endpoint Except for BiDirectional Control Endpoints */
	struct lwUSB_ep_ch_s  		EP_TxChannel ;						/* Transmission Channel Defined Only for IN EndPoints and Control */
	struct lwUSB_ep_ch_s        EP_RxChannel ;						/* Reception Channel Defined Only for OUT EndPoints and Control   */

	struct lwUSB_transaction_s *EP_Transaction ;					    /*  the Current Running Transaction on the Endpoint */


};


/* Exposed User Functions */
#include "User_F.h"








#endif /* LWUSB_H_ */
