/*
 * lwUSB.h
 *
 *  Created on: Dec 17, 2022
 *      Author: Garmoosh
 */

#ifndef SRC_LWUSB_H_
#define SRC_LWUSB_H_

#include "lwCommon.h"
#include "lwUSB_SPECS.h"



//todo remove
#define USB_STATE_IDLE 0u
#define USB_STATE_RESET 1u
#define USB_STATE_SUSP 2u
#define USB_STATE_SOF 3u
#define USB_STATE_ESOF 4u
#define USB_STATE_ERR 5u
#define USB_STATE_WKUP 6u
#define USB_STATE_CTR 7u



/* Constants */



#define LWUSB_EP_TYPE_RANGE 3u
#define LWUSB_EP_KIND_RANGE 1u
#define LWUSB_SAVE_EP_TOGG (~(USB_EP0R_DTOG_RX_Msk|USB_EP0R_DTOG_TX_Msk|USB_EP0R_STAT_RX_Msk|USB_EP0R_STAT_TX_Msk))
#define LWUSB_ISOCHRONOUS_MAX_PERIOD_MS 32768u
#define LWUSB_ENDPOINT_MAX_PERIOD_MS 255u
#define LWUSB_EP_TYPE_MAX 3u
#define LWUSB_EP_DIRECTION_MAX 1u
#define LWUSB_ENDPOINT_MAX_NUMBER 8u


/* Macros*/

#define LWUSB_WRITE_DTOG_RX(EPXR,DTOG) *(EPXR) = (( ((*EPXR)&LWUSB_SAVE_EP_TOGG)^(DTOG << USB_EP0R_DTOG_RX_Pos) ) ) ;
#define LWUSB_WRITE_STAT_RX(EPXR,STAT)  *(EPXR) = (( (*EPXR)^(STAT << USB_EP0R_STAT_RX_Pos) )&((LWUSB_SAVE_EP_TOGG|USB_EP0R_STAT_RX_Msk)) );
#define LWUSB_WRITE_STAT_TX(EPXR,STAT)  *(EPXR) = (( (*EPXR)^(STAT << USB_EP0R_STAT_TX_Pos) )&((LWUSB_SAVE_EP_TOGG|USB_EP0R_STAT_TX_Msk)) );
#define LWUSB_WRITE_DTOG_TX(EPXR,DTOG) *(EPXR) = (( ((*EPXR)&LWUSB_SAVE_EP_TOGG)^(DTOG << USB_EP0R_DTOG_TX_Pos) ) ) ;
#define LWUSB_CLEAR_CTR_RX(EPXR)       *(EPXR) = (((*EPXR)&(~USB_EP0R_CTR_RX_Msk))&LWUSB_SAVE_EP_TOGG)
#define LWUSB_CLEAR_CTR_TX(EPXR)       *(EPXR) = (((*EPXR)&(~USB_EP0R_CTR_TX_Msk))&LWUSB_SAVE_EP_TOGG)



/* This Implementation Supports only :
 * Configurations with no class or vendor specific descriptors
 * it only implements configurations with defined interfaces .
 * it currently does not support double buffers.
 * it currently does not support alternate interfaces
 *
 */

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
	uint8_t* payload_buffer                  ;						/* Pointer to the Buffer to Transmit From */
	uint32_t payload_length                  ;						/* Total Transaction Length */
	uint32_t payload_ptr                     ;						/* Retains Completed Bytes Status */
	e_lwUSB_transaction_state current_state  ;						/* State of the Transaciton */
	e_lwUSB_transaction_state previous_state ;						/* Previous State of the Transaction , Useful for Control Eps */
	struct lwUSB_transaction_s * next        ;						/* Next Transaciton */

};

struct lwUSB_ep_ch_s {
	/* Low Level Communication Channel for EndPoints */
	e_lwUSB_ep_state 			EP_State ;							/* Current State */
	uint16_t         			EP_Size ;							/* Endpoint Buffer Size */
	uint32_t *       			EP_BufferHandle ;					/* Pointer to the Endpoint's Allocated buffer */
	struct lwUSB_transaction_s* EP_Transaction ;					/* Pointer to the Current Running Transaction on the Endpoint */
};

struct lwUSB_ep_s {
	/* EndPoint LOW Level Control Structure */
	uint32_t         			EP_isInitialized ;					/* Endpoint Initialized Check */
	e_lwUSB_EndPoint_Type   	EP_Type ;							/* Endpoint Type  Control , Bulk , Isochronous , Interrupt */
	e_lwUSB_EndPoint_Direction 	EP_Direction ;						/* In , out or INOUT */
	uint8_t         			EP_Number ;							/* Number of this Endpoint , this Refers to Endpoint Index in reference to the actual Number of EPS in the HW */
	uint8_t                     EP_Address ;						/* Address of the Endpoint , this Refers to the Actual Address the Endpoint is Addressed By the Host */
	/* Only one Channel is allowed for an Endpoint Except for BiDirectional Control Endpoints */
	struct lwUSB_ep_ch_s * 		EP_TxChannel ;						/* Transmission Channel Defined Only for IN EndPoints and Control */
	struct lwUSB_ep_ch_s *      EP_RxChannel ;						/* Reception Channel Defined Only for OUT EndPoints and Control   */

};




/* These Functions Need To be Implemented Based on the Hardware */

/* This Function Should Perform Initial Setup of USB Registers and Setup the Hardware/Transceivers*/
int32_t lwUSB_hwInit( void );

/* This Function Should Disable all USB Communication */
int32_t lwUSB_hwDisable( void );

/* This Function Should Enable all USB Communication */
int32_t lwUSB_hwEnable( void );

/* This Function Should Perform a Register Reset ,
 * - it should Disable all Endpoints.
 * it is called if a reset Request is received */
int32_t lwUSB_hwRegisterReset( void );


/* This Function Should Perform a Packet Memory Reset ,
 * - it should Re
 * it is called if a reset Request is received */
int32_t lwUSB_hwMemoryReset( void );


/* This Function Initializes an EndPoint
 * -it should Create Valid Packet Buffer Handles for the EndPoint */
int32_t lwUSB_hwInitializeEP( lwUSB_btable_ep_entry_s * lwEP );


/* This Function Writes Data to an EndPoint Channel through its already Initialized Packet Memory Handle
 */
int32_t lwUSB_hwWriteData( lwUSB_btable_ep_entry_s * lwEP , uint8_t * lwData , uint16_t lwLen );


/* This Function Reads Data from an EndPoint Channel through its already Initialized Packet Memory Handle
 */
int32_t lwUSB_hwReadData(  lwUSB_btable_ep_entry_s * lwEP , uint8_t * lwData , uint16_t lwLen );

/* This Function Sets the Address of the Device */
int32_t lwUSB_hwSetAddress( uint8_t Address );

/* This Function Sets Status return on the next IN Transaction */

int32_t  lwUSB_hwSetTXResponse ( e_lwUSB_EndPoint_State Resp );


/* This Function Sets Status return on the next OUT Transaction */

int32_t  lwUSB_hwSetRXResponse ( e_lwUSB_EndPoint_State Resp );


/* This Function Sets Data Toggle in IN Data Stages */

int32_t  lwUSB_hwSetTXDataToggle ( void  );

/* This Function Clears Data Toggle in IN Data Stages */

int32_t  lwUSB_hwClearTXDataToggle ( void  );


/* This Function Sets Data Toggle in OUT Data Stages */

int32_t  lwUSB_hwSetRXDataToggle ( void  );

/* This Function Clears Data Toggle in OUT Data Stages */

int32_t  lwUSB_hwClearRXDataToggle ( void  );


/* This Function Clears Transaction TX Complete Flag */

int32_t  lwUSB_hwClearTXTransactionComplete ( void  );

/* This Function Clears Transaction RX Complete Flag */

int32_t  lwUSB_hwClearRXTransactionComplete ( void  );


/* This Function returns True if the HW Detected a PID Token with Setup */

int32_t  lwUSB_hwIsSetupFlag ( void  );

/* This Function returns True if a Reset Condition is Detected  */

int32_t  lwUSB_hwIsResetFlag ( void  );

/* This Function Clears Reset Condition Flags  */

int32_t  lwUSB_hwClearResetFlag ( void  );
/* This Function returns True if a Suspend Condition is Detected  */

int32_t  lwUSB_hwIsSuspendFlag ( void  );

/* This Function Clears Suspend Condition Flags  */

int32_t  lwUSB_hwClearSuspendFlag ( void  );

/* This Function returns True if a Suspend Condition is Detected  */

int32_t  lwUSB_hwIsTransactionComplete ( void  );

/* This Function Clears Transaction Complete Condition Flags  */

int32_t  lwUSB_hwClearTransactionCompleteFlag ( void  );




#endif /* SRC_LWUSB_H_ */
