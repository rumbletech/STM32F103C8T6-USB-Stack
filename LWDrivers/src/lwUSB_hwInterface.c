/*
 * lwUSB_hwInterface.c
 *
 *  Created on: Jan 14, 2023
 *      Author: mrashada
 */

#include "lwUSB_Opts.h"
#include "lwCommon.h"
#include "lwPbuffi.h"


#define LWUSB_USB_TYPEDEF USB
#define STM32F103_USB_TRANSCEIVER_STARTUP_US 1u
#define STM32F103_USB_PERIPHERAL_RESET_DELAY_NS 1u

#define LWUSB_SAVE_EP_TOGG (~(USB_EP0R_DTOG_RX_Msk|USB_EP0R_DTOG_TX_Msk|USB_EP0R_STAT_RX_Msk|USB_EP0R_STAT_TX_Msk))
#define LWUSB_GET_EPR(EP_NUM) (((uint32_t*)LWUSB_USB_TYPEDEF)+EP_NUM)	//Get EPR Register for an Endpoint
#define LWUSB_READ_EPR(EP_NUM) (*(LWUSB_GET_EPR(EP_NUM)))	//Get EPR Register for an Endpoint
#define LWUSB_WRITE_DTOG_RX(EPXR,DTOG) *(EPXR) = (( ((*EPXR)&LWUSB_SAVE_EP_TOGG)^(DTOG << USB_EP0R_DTOG_RX_Pos) ) ) ;
#define LWUSB_WRITE_STAT_RX(EPXR,STAT)  *(EPXR) = (( (*EPXR)^(STAT << USB_EP0R_STAT_RX_Pos) )&((LWUSB_SAVE_EP_TOGG|USB_EP0R_STAT_RX_Msk)) );
#define LWUSB_WRITE_STAT_TX(EPXR,STAT)  *(EPXR) = (( (*EPXR)^(STAT << USB_EP0R_STAT_TX_Pos) )&((LWUSB_SAVE_EP_TOGG|USB_EP0R_STAT_TX_Msk)) );
#define LWUSB_WRITE_DTOG_TX(EPXR,DTOG) *(EPXR) = (( ((*EPXR)&LWUSB_SAVE_EP_TOGG)^(DTOG << USB_EP0R_DTOG_TX_Pos) ) ) ;
#define LWUSB_CLEAR_CTR_RX(EPXR)       *(EPXR) = (((*EPXR)&(~USB_EP0R_CTR_RX_Msk))&LWUSB_SAVE_EP_TOGG)
#define LWUSB_CLEAR_CTR_TX(EPXR)       *(EPXR) = (((*EPXR)&(~USB_EP0R_CTR_TX_Msk))&LWUSB_SAVE_EP_TOGG)

static inline err_t lwUSB_InitializeTranceivers( void )
{
	/* Enable Tranceivers */
	USB->CNTR &= ~USB_CNTR_PDWN_Msk  ;
	/* wait for T_startup */
	lw_waitfor_us(STM32F103_USB_TRANSCEIVER_STARTUP_US);
	/* Exit Reset State*/
	USB->CNTR &= ~USB_CNTR_FRES_Msk ;
	while( USB->CNTR & USB_CNTR_FRES_Msk ) ;
	/* Clear Pending Interrupts*/
	USB->ISTR = 0u ;

	return ERR_OK ;
}


static inline err_t lwUSB_PerphReset( void )
{
	RCC->APB1RSTR |= RCC_APB1RSTR_USBRST_Msk ;
	lw_waitfor_ns(STM32F103_USB_PERIPHERAL_RESET_DELAY_NS);
	RCC->APB1RSTR &= ~(RCC_APB1RSTR_USBRST_Msk);
	lw_waitfor_ns(STM32F103_USB_PERIPHERAL_RESET_DELAY_NS);
	return ERR_OK ;

}



static inline err_t lwUSB_Enable( void )
{
	RCC->APB1ENR|= RCC_APB1ENR_USBEN_Msk ;
	return ERR_OK ;
}



static inline err_t lwUSB_Disable( void )
{
	RCC->APB1ENR&= ~RCC_APB1ENR_USBEN_Msk ;
	return ERR_OK ;
}

static inline err_t lwUSB_Enable_NVIC_IRQ( void )
{
	__NVIC_SetPriority(USB_HP_IRQn ,NVIC_EncodePriority(LW_PRIO_GROUP, 1UL , 0UL) );
	__NVIC_EnableIRQ(USB_HP_IRQn);
	__NVIC_SetPriority(USB_LP_IRQn ,NVIC_EncodePriority(LW_PRIO_GROUP, 2UL , 0UL) );
	__NVIC_EnableIRQ(USB_LP_IRQn);
	__NVIC_SetPriority(USBWakeUp_IRQn ,NVIC_EncodePriority(LW_PRIO_GROUP, 1UL , 0UL) );
	__NVIC_EnableIRQ(USBWakeUp_IRQn);

	return ERR_OK ;

}

static inline err_t lwUSB_Disable_NVIC_IRQ( void )
{
	__NVIC_DisableIRQ(USB_HP_IRQn);
	__NVIC_DisableIRQ(USB_LP_IRQn);
	__NVIC_DisableIRQ(USBWakeUp_IRQn);

	return ERR_OK ;
}

static inline err_t lwUSB_Clear_NVIC_IRQ( void )
{
	__NVIC_ClearPendingIRQ(USB_HP_IRQn);
	__NVIC_ClearPendingIRQ(USB_LP_IRQn);
	__NVIC_ClearPendingIRQ(USBWakeUp_IRQn);

	return ERR_OK ;
}
static inline err_t  lwUSB_EnableUSBFunction ( void )
{
	USB->DADDR |= (USB_DADDR_EF_Msk) ;
	return ERR_OK ;
}


static inline err_t lwUSB_DisableUSBFunction ( void )
{
    USB->DADDR &= ~(USB_DADDR_EF_Msk);
    return ERR_OK ;
}

err_t lwUSB_HardwareReset( void )
{
	lwUSB_Disable_NVIC_IRQ();
	lwUSB_Clear_NVIC_IRQ();
	lwUSB_Disable();
	lwUSB_PerphReset();
	lwUSB_Enable();
	lwUSB_InitializeTranceivers();
	lwUSB_Enable_NVIC_IRQ();

	return ERR_OK ;
}


static inline err_t lwUSB_HW_SetAddress ( uint8_t addr )
{
	USB->DADDR &= ~ ( USB_DADDR_ADD_Msk );
	USB->DADDR |= ( addr << USB_DADDR_ADD_Pos );
	return ERR_OK ;
}




err_t lwUSB_hwInit( void ){

	lwUSB_Disable_NVIC_IRQ();
	lwUSB_Clear_NVIC_IRQ();
	lwUSB_Disable();
	lwUSB_PerphReset();
	lwUSB_Enable();
	lwUSB_InitializeTranceivers();
	lwUSB_Enable_NVIC_IRQ();

}

err_t lwUSB_hwRegisterReset( void ){

	for ( int32_t i = 0 ; i < LWUSB_OPTS_NUM_HW_EPS ; i++){
		uint32_t * epxr = LWUSB_GET_EPR(i);
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

//todo never pass an endpoint struct to these functions only pass endpoint number
err_t lwUSB_hwInitializeEP( struct lwUSB_ep_s * lwEP ){
	return ERR_OK ;
}

err_t lwUSB_hwSetAddress( uint8_t Address ){

	USB->DADDR &= ~ ( USB_DADDR_ADD_Msk );
	USB->DADDR |= ( Address << USB_DADDR_ADD_Pos );
	return ERR_OK ;
}

int32_t lwUSB_hwWriteData( uint8_t lwEPNum , uint8_t * lwData , uint16_t lwLen ){

	int32_t status = lwUSB_pmaWrite(lwEPNum, lwData , lwLen);
	if ( status < 0 ){
		return ERR_FAULT ;
	}
	lwUSB_pmaWriteNumBytes(lwEPNum ,lwLen);
	return status;
}


int32_t lwUSB_hwReadData( uint8_t lwEPNum , uint8_t* lwData ){

	int32_t status = lwUSB_pmaGetNumBytes( lwEPNum );
	int32_t Length = status ;
	if ( status < 0 ){
		return ERR_FAULT ;
	}
	lwUSB_pmaRead( lwEPNum , lwData, Length );
	return Length ;
}

int32_t  lwUSB_hwSetTXResponse ( uint8_t lwEPNum ,  uint8_t lwResp ){
	return LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(lwEPNum) , lwResp  );
}


int32_t  lwUSB_hwSetRXResponse ( uint8_t lwEPNum  ,  uint8_t lwResp ){
	return LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(lwEPNum) , lwResp  );
}


int32_t  lwUSB_hwIsSetupFlag ( uint8_t lwEPNum  ){
	return LWUSB_READ_EPR(lwEPNum)&USB_EP0R_SETUP_Msk ;
}

int32_t  lwUSB_hwIsOUTFlag (  uint8_t lwEPNum  ){
	return LWUSB_READ_EPR(lwEPNum)&USB_EP0R_CTR_RX_Msk ;
}

int32_t  lwUSB_hwIsINFlag (  uint8_t lwEPNum  ) {
	return LWUSB_READ_EPR(lwEPNum)&USB_EP0R_CTR_TX_Msk ;
}

void  lwUSB_hwClearINFlag(  uint8_t lwEPNum  ){
	LWUSB_CLEAR_CTR_TX(LWUSB_GET_EPR(lwEPNum));
}

void  lwUSB_hwClearOUTFlag(  uint8_t lwEPNum  ){
	LWUSB_CLEAR_CTR_RX(LWUSB_GET_EPR(lwEPNum));
}
void  lwUSB_hwClearSetupFlag(  uint8_t lwEPNum  ){
	LWUSB_CLEAR_CTR_RX(LWUSB_GET_EPR(lwEPNum));
}

int32_t lwUSB_hwGetNumReceivedBytes ( uint8_t lwEPNum ){
	return lwUSB_pmaGetNumBytes( lwEPNum );
}

int32_t lwUSB_hwWriteNumTransmittedBytes ( uint8_t lwEPNum , uint32_t lwLen ){
	return lwUSB_pmaWriteNumBytes(lwEPNum ,lwLen);
}

int32_t lwUSB_hwConfigureEndPoint ( uint8_t lwEPNum , uint8_t lwEPAddr , uint8_t lwEPType , uint8_t lwEPDir ){

	uint32_t * epxr = LWUSB_GET_EPR(lwEPNum);

	/* Setup EP */
	*(epxr) = (~( USB_EP0R_EP_TYPE_Msk | USB_EP0R_EP_KIND_Msk | USB_EP0R_EA_Msk ) )&LWUSB_SAVE_EP_TOGG;
	*(epxr) = ((lwEPType << USB_EP0R_EP_TYPE_Pos ) | (lwEPAddr << USB_EP0R_EA_Pos   ))&LWUSB_SAVE_EP_TOGG;
}

