/*
 * Pbuffi.h
 *
 *  Created on: Jan 14, 2023
 *      Author: mrashada
 */

#ifndef LWPBUFFI_H_
#define LWPBUFFI_H_

#include <Common.h>

#include "../../lwUSB/Include/lwUSB_Opts.h"

/* Configuration Parameters for Microcontroller Specific PMA Buffer */

#define LWUSB_PMA_EP_NUM LWUSB_OPTS_NUM_HW_EPS                      /* Number of Hardware EPS , To Calculate Size of Btable */
#define LWUSB_PMA_BUFF_ALIGNMENT 2u									/* Alignment of PMA Memory */
#define LWUSB_PMA_MEM_SIZE 512u									    /* Total Supported Memory Size */
#define LWUSB_PMA_START_ADDR USB_PMAADDR							/* PMA Start Address */
#define LWUSB_PMA_BTABLE_ENTRY_SIZE 8u								/* PMA Entry Size Per EP */
#define LWUSB_PMA_FILL_VALUE 0XA5									/* PMA Buffer Initialization Fill Value */
#define LWUSB_PMA_BTABLE_SIZE (LWUSB_PMA_EP_NUM*LWUSB_PMA_BTABLE_ENTRY_SIZE/LWUSB_PMA_BUFF_ALIGNMENT)  /* Size of EP PMA Entry */
#define LWUSB_PMA_BUFF_SPACE_SIZE (LWUSB_PMA_MEM_SIZE/LWUSB_PMA_BUFF_ALIGNMENT - LWUSB_PMA_BTABLE_SIZE) /* Memory Space for EP Buffers*/

/* Macros */
#define LWUSB_PMA_GET_PMA_ADDR(X) ((uint16_t)((UINT32T_CAST(X)-UINT32T_CAST(LWUSB_PMA_START_ADDR))/LWUSB_PMA_BUFF_ALIGNMENT)) //Convert from SYS Addr to PMA
#define LWUSB_PMA_GET_SYS_ADDR(X) ((uint32_t*)((uint32_t)LWUSB_PMA_START_ADDR + X*LWUSB_PMA_BUFF_ALIGNMENT))				  //Convert from PMA Addr to SYS


err_t USB_pmaInit( void );
void* USB_pmaAllocate( uint32_t ep_num , size_t size  );
err_t USB_pmaRead( uint8_t EPNum , uint8_t * DataPtr , uint32_t len  );
err_t USB_pmaWrite( uint8_t EPNum , uint8_t * DataPtr  , uint32_t len   );
err_t USB_pmaWriteTXEntry( uint32_t ep_num , void *addr );
err_t USB_pmaWriteRXEntry( uint32_t ep_num , void *addr , size_t size  );
int32_t USB_pmaWriteNumBytes( uint8_t EPNum , uint32_t Length );
int32_t USB_pmaGetNumBytes( uint8_t EPNum  );

#endif /* LWPBUFFI_H_ */
