/*
 * Hal.h
 *
 *  Created on: Jul 14, 2023
 *      Author: Garmoosh
 *      Provides User Interface Functions
 */

#ifndef HAL_HAL_H_
#define HAL_HAL_H_

#include "Include/lwUSB_Std.h"

struct lwUSB_Status HAL_GetStatus( void );
uint32_t HAL_GetDownloadLength( uint8_t endpoint );
void HAL_SetAddress( uint8_t devAddr );
void HAL_Reset( void );
void HAL_Suspend( void );
void HAL_UploadPacket( uint8_t endpoint );

#endif /* HAL_HAL_H_ */
