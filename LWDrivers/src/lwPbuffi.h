/*
 * lwPbuffi.h
 *
 *  Created on: Jan 14, 2023
 *      Author: mrashada
 */

#ifndef LWPBUFFI_H_
#define LWPBUFFI_H_

#include "lwUSB_Opts.h"


/* Configuration Parameters for Microcontroller Specific PMA Buffer */
#define LWUSB_PMA_EP_NUM LWUSB_OPTS_NUM_HW_EPS                      /* Number of Hardware EPS , To Calculate Size of Btable */
#define LWUSB_PMA_MEM_SIZE 512u									    /* Total Supported Memory Size */
#define LWUSB_PMA_START_ADDR USB_PMAADDR							/* PMA Start Address */
#define LWUSB_PMA_BTABLE_ENTRY_SIZE 8u								/* PMA Entry Size Per EP */
#define LWUSB_FILL_VALUE 0XA5										/* PMA Buffer Initialization Fill Value */

/* Macros */
#define LWUSB_PMA_ADDR(X) ((uint16_t)((UINT32T_CAST(X)-UINT32T_CAST(LWUSB_PMA_START_ADDR))/LWUSB_PMA_BUFF_ALIGNMENT)) //Convert from SYS Addr to PMA
#define LWUSB_SYS_ADDR(X) ((uint32_t*)((uint32_t)LWUSB_PMA_START_ADDR + X*LWUSB_PMA_BUFF_ALIGNMENT))				  //Convert from PMA Addr to SYS
#define LWUSB_GET_EPR(EP_NUM) (((uint32_t*)LWUSB_USB_TYPEDEF)+EP_NUM)												  //Get EPR Register for an Endpoint

err_t lwUSB_pmaInit( void );
err_t lwUSB_pmaWriteEntry( uint32_t ep_num , struct lwUSB_btable_ep_entry_s * entry );
struct lwUSB_btable_ep_entry_s * lwUSB_pmaGetEntry ( uint32_t ep_num );
err_t lwUSB_pmaRead( uint8_t * ptr , uint32_t len , uint32_t* addr  );
err_t lwUSB_pmaWrite( uint8_t * ptr , uint32_t len , uint32_t* addr  );


#endif /* LWPBUFFI_H_ */
