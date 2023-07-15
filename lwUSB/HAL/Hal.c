/* Author : mrashada
 * Date   : 7/14/2023
 * ***********************************************************************
 * ***********************************************************************
 * This defines the HAL(Hardware Abstraction Layer)
 * ***********************************************************************
 * - This file Provides the interface to user code .
 * - Communicating to the stack is done via Events , each event shall take
 * - Certain arguments , ex : communication of data received event shall be done
 * - via indicating an RX event and given event specific parameters like
 * - Endpoint number , Pointer to Data , and Data Size.
 * - there are void or argumentless events such as reset/suspend etc.
 * - when stack will try to communicate with hal is also through events.
 * - so user code shall do :
 * - set Events with Proper Arguments after hardware events occur.
 * - handle Events Set by the stack.
 * ***********************************************************************
 */

#include "../lwUSB.h"
#include "../Include/lwUSB_Debug.h"
#include "../Include/lwUSB_Std.h"
#include "../lwUSB_Intf.h"
#include <stdarg.h>

#define NO_SIGNAL() goto EVT_NO_SIGNAL

void HAL_notifyEvent ( enum lwUSB_BusEvent_e event_e , ... ){

    va_list args;
    va_start(args, event_e);

    if ( event_e >=  lwUSB_BusEvent_e_End ||
    	 event_e <=  lwUSB_BusEvent_e_Start){
    	LW_PRINTF("HAL_notifyEvent" ERRS_BAD_ARGS);
    	return;
    }

    /* Handle Event  Specific Args */
    switch ( event_e ){

    case lwUSB_BusEvent_e_RX :
    {
    	uint8_t epNum = (uint8_t) va_arg(args, int);
    	uint8_t* epData = (uint8_t*) va_arg(args,void*);
    	uint16_t epDataSz = (uint16_t) va_arg(args,int);
    	lwUSB_Intf_PushData(epNum,epData,epDataSz);
        NO_SIGNAL();
    }

    case lwUSB_BusEvent_e_RESET :

    case lwUSB_BusEvent_e_ERR :

    case lwUSB_BusEvent_e_SUSPEND :

    default :
    	break;

    }

	/* Signal the Event */
    lwUSB_Intf_SignalBusEvents(event_e);

EVT_NO_SIGNAL:

    va_end(args);
    return;
}

static void HAL_handleEvent ( enum lwUSB_BusEvent_e event_e , ... ){

    va_list args;
    va_start(args, event_e);

    if ( event_e >=  lwUSB_BusEvent_e_End ||
    	 event_e <=  lwUSB_BusEvent_e_Start){
    	LW_PRINTF("HAL_notifyEvent" ERRS_BAD_ARGS);
    	return;
    }

    switch ( event_e ){

    case lwUSB_BusEvent_e_TX :
    {
    	uint8_t epNum = (uint8_t) va_arg(args, int);
    	uint8_t* epData = (uint8_t*) va_arg(args,void*);
    	uint16_t epDataSz = (uint16_t) va_arg(args,int);

    	HAL_TxHandler(epNum,epData,epDataSz);
    }
    	break;

    case lwUSB_BusEvent_e_RESET :
    	HAL_ResetHandler();
    	break;
    case lwUSB_BusEvent_e_ERR :
    	HAL_ErrorHandler();
    	break;
    case lwUSB_BusEvent_e_SUSPEND :
    	HAL_SuspendHandler();
    	break;
    default :
    	break;
    }

    va_end(args);
    return ;
}

/* Simplified USER Interface */
WEAK void HAL_TxHandler(uint8_t epNum ,  uint8_t * dataPtr , uint16_t dataSz ){
}

WEAK void HAL_ResetHandler(){
}

WEAK void HAL_ErrorHandler(){
}

WEAK void HAL_SuspendHandler(){
}
