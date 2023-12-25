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

void Event_Init( struct Event_s* ev , uint32_t evID ,void(*evcb)(void*) ){
	ev->evID = evID;
	ev->evSCount = 0ul;
	ev->evRCount = 0ul;
	ev->evCb = evcb;
	return;
}

void Event_RegisterCallBack( struct Event_s* ev , void(*evcb)(void*) ){
	ev->evCb = evcb;
	return;
}

uint32_t Event_GetCount( struct Event_s* ev ){
	return ev->evSCount;
}

void Event_CallCB( struct Event_s* ev , void* arg ){
	void(*Cb)(void*) = ev->evCb ;
	Cb(arg);
	return;
}

void Event_Set( struct Event_s* ev ){
	ev->evSCount++;
	return;
}

void Event_Reset( struct Event_s* ev){
	ev->evRCount++;
	return;
}
void Event_Clear(struct Event_s* ev ){
	ev->evSCount = 0ul;
	ev->evRCount = 0ul;
	return;
}
