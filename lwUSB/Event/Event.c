/* Author : mrashada
 * Date   : 6/28/2023
 * ***********************************************************************
 * ***********************************************************************
 * This Defines an Abstract Type *** Event ****
 * ***********************************************************************
 * an Event describes either hardware events or stack events ( err , etc )
 * an Event can be set , if an event is already set before then its count
 * is incremented each time set is called.
 * an Event count is cleared once it had been handled , Clear()
 * an Event has a function pointer (callback) that may be called by handler.
 * ***********************************************************************
 */

#include "Event.h"

void Event_Init( struct Event_s* ev , uint32_t evID  , uint32_t(*evHdl)(struct Event_s*) ,void(*evcb)(void*) , void(*evErr)(struct Event_s*) , uint32_t evSize ){
	ev->evID = evID;
	ev->evCount = 0ul;
	ev->evCb = evcb;
	ev->evErr = evErr;
	ev->evHdl = evHdl;
	ev->evSize = evSize;
	return;
}

void Event_RegisterCallBack( struct Event_s* ev , void(*evcb)(void*) ){
	ev->evCb = evcb;
	return;
}

uint32_t Event_GetCount( struct Event_s* ev ){
	return ev->evCount;
}

void Event_CallHandler( struct Event_s * ev ){
	uint32_t(*hdl)(struct Event_s*) = ev->evHdl ;
	hdl(ev);
	return;
}
void Event_CallCB( struct Event_s* ev , void* arg ){
	void(*Cb)(void*) = ev->evCb ;
	Cb(arg);
	return;
}

void Event_Set( struct Event_s* ev ){
	if ( ev->evCount + 1 > ev->evSize ){
		void(*errH)(struct Event_s*) = ev->evErr;
		if ( errH ){
			errH(ev);
		}
		return;
	}
	ev->evCount++;
	return;
}

void Event_Clear(struct Event_s* ev ){
	ev->evCount = 0ul;
	return;
}
