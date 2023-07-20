/*
 * BusHandlers.h
 *
 *  Created on: Jul 21, 2023
 *      Author: Garmoosh
 */

#ifndef INCLUDE_BUSHANDLERS_H_
#define INCLUDE_BUSHANDLERS_H_

#define DO_CB(ARG)              \
	if(ev->evCb != NULL ){      \
		Event_CallCB(ev, ARG); \
	}                           \

static uint32_t resetHandler ( struct Event_s* ev ){

	DO_CB(NULL);
	return 1u;
}

static uint32_t suspendHandler ( struct Event_s* ev ){

	DO_CB(NULL);
	return 1u;
}

static uint32_t rxHandler ( struct Event_s* ev ){

	DO_CB(NULL);
	return 1u;
}

static uint32_t txHandler ( struct Event_s* ev ){

	DO_CB(NULL);
	return 1u;
}

static uint32_t setupHandler ( struct Event_s* ev ){

	DO_CB(NULL);
	return 1u;
}

static uint32_t errorHandler ( struct Event_s* ev ){

	DO_CB(NULL);
	return 1u;
}

static uint32_t wakeupHandler ( struct Event_s* ev ){

	DO_CB(NULL);
	return 1u;
}

#define LWUSB_BUS_HANDLERS   \
		resetHandler,        \
		suspendHandler,      \
		rxHandler,           \
		txHandler,           \
		setupHandler,        \
		errorHandler,        \
		wakeupHandler        \

#endif /* INCLUDE_BUSHANDLERS_H_ */
