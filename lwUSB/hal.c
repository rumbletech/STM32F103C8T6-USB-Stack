/* Author : mrashada
 * Date   : 7/14/2023
 * ***********************************************************************
 * ***********************************************************************
 * This defines the HAL(Hardware Abstraction Layer)
 * ***********************************************************************
 * ***********************************************************************
 */

#include "lwUSB.h"

/* Target Device Implementation Layer */

/* Sets the Address of the USB entity */

WEAK void HAL_SetAddress( uint8_t devAddr ){

}

/* Gets the status bits of the underlying hardware */
/* Status is packed in bit fields */

WEAK struct lwUSB_Status HAL_GetStatus( void  ){

}

/* Triggers the underlying hardware to call its reset routine */

WEAK void HAL_Reset( void ){

}

/* Triggers the underlying hardware to call its Suspend routine */

WEAK void HAL_Suspend( void ){

}

/* Triggers the underlying hardware to upload IN packet for the endpoint */

WEAK void HAL_UploadPacket( uint8_t endpoint ){

}

/* Triggers the underlying hardware to fetch the received packet size for the endpoint */

WEAK uint32_t HAL_GetDownloadLength( uint8_t endpoint ){

}


