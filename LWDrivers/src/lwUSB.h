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

#define LWUSB_EP_NUM 8u
#define LWUSB_BUFF_MEM_SIZE 512u
#define LWUSB_BUFF_PER_EP 2u

/* Constants */
#define LWUSB_CONFIG_VALUE_START 0x40
#define LWUSB_STRING_INDEX_START 0x20
#define LWUSB_USB_TYPEDEF USB
#define LWUSB_PMA_START_ADDR USB_PMAADDR
#define LWUSB_BTABLE_ALIGNMENT 8u
#define LWUSB_PMA_BUFF_ALIGNMENT 2u
#define LWUSB_PMA_BTABLE_ADDRESS (LWUSB_PMA_START_ADDR/2u)
#define LWUSB_BTABLE_ENTRY_SIZE 8u
#define LWUSB_BTABLE_SIZE (LWUSB_EP_NUM*LWUSB_BTABLE_ENTRY_SIZE/LWUSB_PMA_BUFF_ALIGNMENT)
#define LWUSB_BUFF_SPACE_SIZE (LWUSB_BUFF_MEM_SIZE/LWUSB_PMA_BUFF_ALIGNMENT - LWUSB_BTABLE_SIZE)
#define LWUSB_FILL_VALUE 0XA5
#define LWUSB_EP_TYPE_RANGE 3u
#define LWUSB_EP_KIND_RANGE 1u
#define LWUSB_SAVE_EP_TOGG (~(USB_EP0R_DTOG_RX_Msk|USB_EP0R_DTOG_TX_Msk|USB_EP0R_STAT_RX_Msk|USB_EP0R_STAT_TX_Msk))


/* Control EP Configuration */
#define LWUSB_CONTROL_EP_TX_B_SIZE 64u
#define LWUSB_CONTROL_EP_RX_B_SIZE 32u

/* Macros*/
#define LWUSB_PMA_ADDR(X) ((uint16_t)((UINT32T_CAST(X)-UINT32T_CAST(LWUSB_PMA_START_ADDR))/LWUSB_PMA_BUFF_ALIGNMENT))
#define LWUSB_SYS_ADDR(X) ((uint32_t*)((uint32_t)LWUSB_PMA_START_ADDR + X*LWUSB_PMA_BUFF_ALIGNMENT))
#define LWUSB_GET_BTABLE_ADDR() LWUSB_SYS_ADDR(USB->BTABLE)
#define LWUSB_GET_EPR(EP_NUM) (((uint32_t*)LWUSB_USB_TYPEDEF)+EP_NUM)
#define LWUSB_WRITE_DTOG_RX(EPXR,DTOG) *(EPXR) = (( ((*EPXR)&LWUSB_SAVE_EP_TOGG)^(DTOG << USB_EP0R_DTOG_RX_Pos) ) ) ;
#define LWUSB_WRITE_STAT_RX(EPXR,STAT)  *(EPXR) = (( (*EPXR)^(STAT << USB_EP0R_STAT_RX_Pos) )&((LWUSB_SAVE_EP_TOGG|USB_EP0R_STAT_RX_Msk)) );
#define LWUSB_WRITE_STAT_TX(EPXR,STAT)  *(EPXR) = (( (*EPXR)^(STAT << USB_EP0R_STAT_TX_Pos) )&((LWUSB_SAVE_EP_TOGG|USB_EP0R_STAT_TX_Msk)) );
#define LWUSB_WRITE_DTOG_TX(EPXR,DTOG) *(EPXR) = (( ((*EPXR)&LWUSB_SAVE_EP_TOGG)^(DTOG << USB_EP0R_DTOG_TX_Pos) ) ) ;
#define LWUSB_CLEAR_CTR_RX(EPXR)       *(EPXR) = (((*EPXR)&(~USB_EP0R_CTR_RX_Msk))&LWUSB_SAVE_EP_TOGG)
#define LWUSB_CLEAR_CTR_TX(EPXR)       *(EPXR) = (((*EPXR)&(~USB_EP0R_CTR_TX_Msk))&LWUSB_SAVE_EP_TOGG)
#define LWUSB_GET_CONFIG_V_NUM(VALUE) (VALUE-LWUSB_CONFIG_VALUE_START)


/* This Implementation Supports only :
 * Configurations with no class or vendor specific descriptors
 * it only implements configurations with defined interfaces .
 * it currently does not support double buffers.
 * it currently does not support alternate interfaces
 *
 */

struct lwUSB_btable_buff_entry_s {

	uint32_t addr ;    /* Address of Buffer */
	uint32_t count ;   /* Size */

};

struct lwUSB_btable_ep_entry_s {
	union {
		struct lwUSB_btable_buff_entry_s tx_buff ;		/* Single Buffered TX */
		struct lwUSB_btable_buff_entry_s rx_buff_d ;	/* Double Buffered RX */
	};
	union {
		struct lwUSB_btable_buff_entry_s rx_buff ;	  /* Single Buffered RX */
		struct lwUSB_btable_buff_entry_s tx_buff_d ;  /* Double Buffered TX */
	};

};

struct lwUSB_pma_s {

	struct lwUSB_btable_ep_entry_s btable[LWUSB_EP_NUM] ;
	uint32_t buff_mem[LWUSB_BUFF_SPACE_SIZE];



};





typedef enum  {

	e_lwUSB_ep_type_bulk      = 0b00 ,
	e_lwUSB_ep_type_control   = 0b01 ,
	e_lwUSB_ep_type_iso       = 0b10 ,
	e_lwUSB_ep_type_interrupt = 0b11
} e_lWUSB_ep_type ;


typedef enum  {

	e_lwUSB_ep_kind_single_buffered  = 0b0 ,
	e_lwUSB_ep_kind_double_buffered  = 0b1 ,
} e_lwUSB_ep_kind ;


typedef enum {

	e_lwUSB_ep_state_disabled  = 0b00 ,
	e_lwUSB_ep_state_stall     = 0b01 ,
	e_lwUSB_ep_state_nak       = 0b10 ,
	e_lwUSB_ep_state_valid     = 0b11 ,


} e_lwUSB_ep_state ;

typedef enum {

	e_lwUSB_ep_dir_inout   = 0u ,
	e_lwUSB_ep_dir_s_in    = 1u ,
	e_lwUSB_ep_dir_s_out   = 2u ,
	e_lwUSB_ep_dir_d_in    = 3u ,
	e_lwUSB_ep_dir_d_out   = 4u

} e_lwUSB_ep_dir ;


typedef enum {

	e_lwUSB_controller_state_Default = 0u ,
	e_lwUSB_controller_state_address = 1u ,
	e_lwUSB_controller_state_configured = 2u ,
	e_lwUSB_controller_state_suspended = 3u ,
	e_lwUSB_controller_state_attached = 4u ,                    //attached but not powered todo


} e_lwUSB_controller_state ;


typedef enum {

	e_lwUSB_transaction_state_null          = 0u ,
	e_lwUSB_transaction_state_setup_stage   = 1u ,
	e_lwUSB_transaction_state_status_stage =  2u ,
	e_lwUSB_transaction_state_data_in       = 3u ,
	e_lwUSB_transaction_state_data_out      = 4u ,

} e_lwUSB_transaction_state ;

struct lwUSB_transaction_s {


	uint8_t* payload_buffer                  ;
	uint32_t payload_length                  ;
	uint32_t payload_ptr                     ;
	e_lwUSB_transaction_state current_state  ;
	e_lwUSB_transaction_state previous_state ;
	struct lwUSB_transaction_s * next        ;



};

struct lwUSB_interface_s {


	struct lwUSB_interface_descriptor_s * d_interface ;
	struct lwUSB_endpoint_s  * endpoint   ;
	struct lwUSB_interface_s * next ;

};
struct lwUSB_configuration_s {


	struct lwUSB_configuration_descriptor_s * d_configuration  ;
	struct lwUSB_interface_s * interface ;
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



struct lwUSB_ep_s {

	uint32_t isInitialized ;

	e_lWUSB_ep_type type ;
	e_lwUSB_ep_kind kind ;
	uint32_t ep_num ;
	uint8_t ep_addr ;
	e_lwUSB_ep_dir dir ;

	/* Transactions */
	union {
		struct lwUSB_transaction_s* t_in ;
		struct lwUSB_transaction_s* t_out_d ;

	};
	union {
		struct lwUSB_transaction_s* t_out ;
		struct lwUSB_transaction_s* t_in_d ;

	};


	union {
	e_lwUSB_ep_state tx_state ;
	e_lwUSB_ep_state rxd_state ;
	};
	union {
	e_lwUSB_ep_state rx_state ;
	e_lwUSB_ep_state txd_state ;
	};

	union {
	uint32_t txb_size ;
	uint32_t rxdb_size ;
	};
	union {
	uint32_t rxb_size  ;
	uint32_t txdb_size ;
	};

	union {
		uint32_t * txb_addr ;
		uint32_t * rxdb_addr ;
	};

	union {
		uint32_t * rxb_addr ;
		uint32_t * txdb_addr ;
	};
};



err_t lwUSB_HardwareReset( void );
err_t lwUSB_Init( void );
err_t lwUSB_Initialize_Endpoint ( struct lwUSB_ep_s * lwusb_ep );
err_t lwUSB_Reset( void );


void* lwUSB_Allocate( uint32_t ep_num , uint32_t size  );
err_t lwUSB_pmaInit( void );
err_t lwUSB_pmaWriteEntry( uint32_t ep_num , struct lwUSB_btable_ep_entry_s * entry );
err_t lwUSB_pmaRead( uint8_t * ptr , uint32_t len , uint32_t* addr  );
err_t lwUSB_pmaWrite( uint8_t * ptr , uint32_t len , uint32_t* addr  );
struct lwUSB_btable_ep_entry_s * lwUSB_pmaGetEntry ( uint32_t ep_num );

#endif /* SRC_LWUSB_H_ */
