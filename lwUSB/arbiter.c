/* Author : mrashada
 * Date   : 7/20/2023
 * ***********************************************************************
 * ***********************************************************************
 * This Defines the Main State Machine for the Stack
 * ***********************************************************************
 * ***********************************************************************
 */
#include "Event/Event.h"
#include "hal.h"
#include "hw_itf.h"
#include "memctrl.h"
#include "tree.h"
#include "sw_itf.h"

#define BUS_EVENTS_SZ 32u
#define A_STATE_UNDEFINED  0u
#define A_STATE_INIT_DONE  1u
#define A_STATE_START_DONE 2u

struct arbiter_ctl_block_s {
	uint32_t state;

};
static struct arbiter_ctl_block_s a_cb;
static struct Event_s BusEvents[lwUSB_BusEvent_e_End];


/* this function must be called , before any other call to the stack. */
void Arbiter_Init ( void ){

	a_cb.state = A_STATE_UNDEFINED;
	Event_Init(&BusEvents[lwUSB_BusEvent_e_RESET],lwUSB_BusEvent_e_RESET,NULL);
	Event_Init(&BusEvents[lwUSB_BusEvent_e_SUSPEND],lwUSB_BusEvent_e_SUSPEND,NULL);
	Event_Init(&BusEvents[lwUSB_BusEvent_e_RX],lwUSB_BusEvent_e_RX,NULL);
	Event_Init(&BusEvents[lwUSB_BusEvent_e_TX],lwUSB_BusEvent_e_TX,NULL);
	Event_Init(&BusEvents[lwUSB_BusEvent_e_SetAddress],lwUSB_BusEvent_e_SetAddress,NULL);
	Event_Init(&BusEvents[lwUSB_BusEvent_e_SETUP],lwUSB_BusEvent_e_SETUP,NULL);
	Event_Init(&BusEvents[lwUSB_BusEvent_e_ERR],lwUSB_BusEvent_e_ERR,NULL);
	Event_Init(&BusEvents[lwUSB_BusEvent_e_WAKEUP],lwUSB_BusEvent_e_WAKEUP,NULL);


	return ;
}

struct Event_s * getEventbyID (uint32_t evid){
	return &BusEvents[evid];
}


void Arbiter_Start( void ){



}
/* this function will be the state machine of the stack :D */

void Arbiter_Main( void ){

	/* Super LOOP */
	struct sw_state_s sw_state = sw_itf_GetStatus();
	/* SW Init  */
	if ( sw_state.init_d == 0u ){
		a_cb.state = A_STATE_UNDEFINED;
	}
	else{
		if ( sw_state.start_d == 1u ){
			a_cb.state = A_STATE_START_DONE;
		}
		else{
			a_cb.state = A_STATE_INIT_DONE;
		}
	}


	return ;
}
