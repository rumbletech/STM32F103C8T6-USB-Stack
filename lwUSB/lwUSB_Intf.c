/* Author : mrashada
 * Date   : 7/14/2023
 * ***********************************************************************
 * ***********************************************************************
 * This defines the Interface Between HAL and the Arbiter
 * ***********************************************************************
 * - the HAL can notifyEvents , so the interface will take an event from HAL
 * - and set the appropriate signal to notify events to the arbiter.
 * - the HAL can also notifyData by EndPoint , the Interface will take data
 * - and meta data from HAL , packetize it as dataUnit , and re-route to the
 * - Arbiter( State Machine )
 * ***********************************************************************
 */

#include "lwUSB_Intf.h"
#include "Event/Event.h"
#include "Arbiter.h"
#include "ringbuffer.h"

static struct RingBuffer_s  itf_b;
static uint8_t itf_mem[LWUSB_ITF_M_S];

void lwUSB_Intf_Init ( void ){
	RingBuffer_Init(&itf_b, &itf_mem[0], LWUSB_ITF_M_S);
	return;
}

void lwUSB_Intf_SignalBusEvents(enum lwUSB_BusEvent_e busEvent_e ){
	struct Event_s * ev = getEventbyID(busEvent_e);
	Event_Set(ev);
	return ;
}

void lwUSB_Intf_PushData( uint8_t epNum , uint8_t* epData , uint16_t epDataSz ){

//	if ( !ps ){
//		sigInternal(lwUSB_InternalEvents_e_OVF);
//	}
	return;
}
