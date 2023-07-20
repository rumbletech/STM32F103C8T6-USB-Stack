/*
 * Event.h
 *
 *  Created on: Jul 15, 2023
 *      Author: Garmoosh
 */

#ifndef EVENT_EVENT_H_
#define EVENT_EVENT_H_

#include <stdint.h>

typedef struct Event_s {

	uint32_t evCount;
	uint32_t evSize;
	uint32_t evID;
	uint32_t(*evHdl)(struct Event_s*);
	void(*evErr)(struct Event_s*);
	void(*evCb)(void*);

} Event , *Event_p;

void Event_Init( struct Event_s* ev , uint32_t evID , uint32_t(*evHdl)(struct Event_s*) ,void(*evcb)(void*) , void(*evErr)(struct Event_s*) , uint32_t evSize );
void Event_RegisterCallBack( struct Event_s* ev , void(*evcb)(void*) );
uint32_t Event_GetCount( struct Event_s* ev );
void Event_CallCB( struct Event_s* ev , void* arg );
void Event_CallHandler( struct Event_s * ev );
void Event_Set( struct Event_s* ev );
void Event_Clear(struct Event_s* ev );

#endif /* EVENT_EVENT_H_ */
