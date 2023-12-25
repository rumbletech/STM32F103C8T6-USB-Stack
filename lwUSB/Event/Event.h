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

	uint32_t evSCount;
	uint32_t evRCount;
	uint32_t evID;
	void(*evCb)(void*);

} Event , *Event_p;

void Event_Init( struct Event_s* ev , uint32_t evID ,void(*evcb)(void*));
uint32_t Event_GetCount( struct Event_s* ev );
void Event_CallCB( struct Event_s* ev , void* arg );
void Event_Set( struct Event_s* ev );
void Event_Reset( struct Event_s* ev);
void Event_Clear(struct Event_s* ev );

#endif /* EVENT_EVENT_H_ */
