/* Author : mrashada
 * Date   : 7/20/2023
 * ***********************************************************************
 * ***********************************************************************
 * This Defines the Main State Machine for the Stack
 * ***********************************************************************
 * ***********************************************************************
 */
#include "lwUSB_Intf.h"
#include "Event/Event.h"
#include "Include/lwUSB_Debug.h"
#include "BusHandlers.h"
#define INTF_POOL_SZ 512u
#define BUS_EVENTS_SZ 32u

static uint8_t intf_pool_p[INTF_POOL_SZ];
static struct DataPool_s intf_datapool ;
static struct Event_s BusEvents[lwUSB_BusEvent_e_End-lwUSB_BusEvent_e_Start-1u];
static uint32_t (*busHandlers[lwUSB_BusEvent_e_End-lwUSB_BusEvent_e_Start-1u])(struct Event_s * ev ) = {
LWUSB_BUS_HANDLERS
};


static void EventErrorHandler ( struct Event_s * ev ){
	LW_PRINTF("!EV:%d , C:%d\r\n" , ev->evID , ev->evCount);
	return;
}
void Arbiter_Init ( void ){

	DataPool_Init(&intf_datapool, &intf_pool_p[0ul], sizeof(intf_pool_p));
	/* Init Interface */
	lwUSB_Intf_Init(&intf_datapool);
	/* Init Bus Events Structures */
	for ( uint32_t iD = lwUSB_BusEvent_e_Start + 1u , j = 0u ; iD < lwUSB_BusEvent_e_End ; iD++ , j++ ){
		Event_Init(&BusEvents[j], iD , busHandlers[j] ,  NULL, EventErrorHandler, UINT32_MAX);
	}
	return ;
}

struct Event_s * getEventbyID (uint32_t evid){
	return &BusEvents[evid-lwUSB_BusEvent_e_Start-1];
}
/* this function will be the state machine of the stack :D */

static void Arbiter_Main( void ){

	/* Check for BusEvents */
    /* Events are Checked in the order of their Priority */
	for ( uint32_t iD = lwUSB_BusEvent_e_Start + 1u  ; iD < lwUSB_BusEvent_e_End ; iD++  ){
		struct Event_s * ev = getEventbyID(iD);
		if ( Event_GetCount(ev)){
			if ( ev->evHdl ){
				Event_CallHandler(ev);
			}
			Event_Clear(ev);
		}
	}





}