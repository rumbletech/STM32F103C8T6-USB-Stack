/*
 * Hal.h
 *
 *  Created on: Jul 14, 2023
 *      Author: Garmoosh
 *      Provides User Interface Functions
 */

#ifndef HAL_HAL_H_
#define HAL_HAL_H_

#include "../lwUSB.h"
#include <stdint.h>

void HAL_notifyEvent ( enum lwUSB_BusEvent_e event_e , ... );
void HAL_handleEvent ( enum lwUSB_BusEvent_e event_e , ... );
void HAL_TxHandler(uint8_t epNum ,  uint8_t * dataPtr , uint16_t dataSz );
void HAL_ResetHandler();
void HAL_ErrorHandler();
void HAL_SuspendHandler();

#endif /* HAL_HAL_H_ */
