/*
 * USB_hwInterface.h
 *
 *  Created on: Jul 23, 2023
 *      Author: Garmoosh
 */

#ifndef SRC_USB_HWINTERFACE_H_
#define SRC_USB_HWINTERFACE_H_

#include "../Common.h"

err_t hwRegisterReset( void );
err_t hwInit( void );
err_t hwMemoryReset( void );
err_t hwEnable( void );
err_t hwDisable ( void );
err_t hwSetAddress( uint8_t Address );
int32_t hwWriteData( uint8_t lwEPNum , uint8_t * lwData , uint16_t lwLen );
int32_t hwReadData( uint8_t lwEPNum , uint8_t* lwData );

#endif /* SRC_USB_HWINTERFACE_H_ */
