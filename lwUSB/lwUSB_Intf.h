/*
 * lwUSB_Intf.h
 *
 *  Created on: Jul 14, 2023
 *      Author: Garmoosh
 */

#ifndef LWUSB_INTF_H_
#define LWUSB_INTF_H_


void lwUSB_Intf_PushData( uint8_t epNum , uint8_t* epData , uint16_t epDataSz );
void lwUSB_Intf_SignalBusEvents(enum lwUSB_BusEvent_e busEvent_e );
#endif /* LWUSB_INTF_H_ */
