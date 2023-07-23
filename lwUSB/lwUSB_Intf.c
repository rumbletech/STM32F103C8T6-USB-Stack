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
#include "Include/lwUSB_Err.h"

static struct DataPool_s * intfPool;

void lwUSB_Intf_Init ( struct DataPool_s * pool ){
	intfPool = pool ;
	return;
}

void lwUSB_Intf_SignalBusEvents(enum lwUSB_BusEvent_e busEvent_e ){
	struct Event_s * ev = getEventbyID(busEvent_e);
	Event_Set(ev);
	return ;
}

void lwUSB_Intf_PushData( uint8_t epNum , uint8_t* epData , uint16_t epDataSz ){

	struct DataUnit_s du = makeDataUnit(epData, makeDataInfo(epDataSz, epNum));
	uint8_t ps = DataPool_Push(intfPool, du);
	if ( !ps ){
		sigInternal(lwUSB_InternalEvents_e_OVF);
	}
	return;
}
