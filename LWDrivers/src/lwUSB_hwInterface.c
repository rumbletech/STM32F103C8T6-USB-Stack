/*
 * lwUSB_hwInterface.c
 *
 *  Created on: Jan 14, 2023
 *      Author: mrashada
 */

#include "lwUSB_Opts.h"



err_t lwUSB_hwRegisterReset( void ){

	for ( int32_t i = 0 ; i < LWUSB_EP_NUM ; i++)
	{
		uint32_t * epxr = LWUSB_GET_EPR(i);
		lwUSB_controller.lwUSB_eps[i] = NULL ;
		/* Disable Endpoints */
		*(epxr) = 0u ;

	}

	return ERR_OK ;
}


err_t lwUSB_hwMemoryReset( void ){

	lwUSB_pmaInit();
	return ERR_OK ;
}


err_t lwUSB_hwEnable( void ){

	USB->DADDR |= (USB_DADDR_EF_Msk);
	return ERR_OK ;
}

err_t lwUSB_hwDisable ( void ){

	USB->DADDR &= ~(USB_DADDR_EF_Msk);
	return ERR_OK ;
}

err_t lwUSB_hwInitializeEP( lwUSB_btable_ep_entry_s * lwEP ){

	return ERR_OK ;


}

err_t lwUSB_hwSetAddress( uint8_t Address ){

	USB->DADDR &= ~ ( USB_DADDR_ADD_Msk );
	USB->DADDR |= ( addr << USB_DADDR_ADD_Pos );
	return ERR_OK ;
}

err_t lwUSB_hwWriteData( lwUSB_btable_ep_entry_s * lwEP , uint8_t * lwData , uint16_t lwLen ){

	return ERR_OK ;


}


err_t lwUSB_hwReadData(  lwUSB_btable_ep_entry_s * lwEP , uint8_t * lwData , uint16_t lwLen ){


	return ERR_OK ;

}
