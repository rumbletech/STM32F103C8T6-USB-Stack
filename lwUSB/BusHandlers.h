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

	/* Call the Hardware Reset Handler */
	HAL_handleEvent(lwUSB_BusEvent_e_SetAddress,0u);
	HAL_handleEvent(lwUSB_BusEvent_e_RESET);
	/* Call User Reset/ CallBacks */
	DO_CB((void*)ev);
	/* Success */
	return 1u;
}

static uint32_t suspendHandler ( struct Event_s* ev ){

	DO_CB((void*)ev);
	return 1u;
}

static uint32_t rxHandler ( struct Event_s* ev ){

	DO_CB((void*)ev);
	return 1u;
}

static uint32_t txHandler ( struct Event_s* ev ){

	DO_CB((void*)ev);
	return 1u;
}

static uint32_t setupHandler ( struct Event_s* ev ){

	DO_CB((void*)ev);
	return 1u;
}

static uint32_t errorHandler ( struct Event_s* ev ){
	/* USB Error Happened , the host shall try and recover this error , so
	 * we shall not do anything , but notify user CallBacks about this
	 * occurence.
	 */
	LWUSB_PRINTF("LWUSB errorHandler\r\n");
	DO_CB((void*)ev);
	return 1u;
}

static uint32_t wakeupHandler ( struct Event_s* ev ){

	DO_CB((void*)ev);
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
