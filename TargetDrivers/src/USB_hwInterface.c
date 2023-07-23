/*
 * lwUSB_hwInterface.c
 *
 *  Created on: Jan 14, 2023
 *      Author: mrashada
 */

#include "USB_hwInterface.h"
#include "Pbuffi.h"

#define HW_NUM_EPS 8u

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

static inline err_t InitializeTranceivers( void )
{
	/* Enable Tranceivers */
	USB->CNTR &= ~USB_CNTR_PDWN_Msk  ;
	/* wait for T_startup */
	_waitfor_us(STM32F103_USB_TRANSCEIVER_STARTUP_US);
	/* Exit Reset State*/
	USB->CNTR &= ~USB_CNTR_FRES_Msk ;
	while( USB->CNTR & USB_CNTR_FRES_Msk ) ;
	/* Clear Pending Interrupts*/
	USB->ISTR = 0u ;

	return ERR_OK ;
}


static inline err_t PerphReset( void )
{
	RCC->APB1RSTR |= RCC_APB1RSTR_USBRST_Msk ;
	_waitfor_ns(STM32F103_USB_PERIPHERAL_RESET_DELAY_NS);
	RCC->APB1RSTR &= ~(RCC_APB1RSTR_USBRST_Msk);
	_waitfor_ns(STM32F103_USB_PERIPHERAL_RESET_DELAY_NS);
	return ERR_OK ;

}



static inline err_t Enable( void )
{
	RCC->APB1ENR|= RCC_APB1ENR_USBEN_Msk ;
	return ERR_OK ;
}



static inline err_t Disable( void )
{
	RCC->APB1ENR&= ~RCC_APB1ENR_USBEN_Msk ;
	return ERR_OK ;
}

static inline err_t Enable_NVIC_IRQ( void )
{
	__NVIC_SetPriority(USB_HP_IRQn ,NVIC_EncodePriority(TARGET_PRIO_GROUP, 1UL , 0UL) );
	__NVIC_EnableIRQ(USB_HP_IRQn);
	__NVIC_SetPriority(USB_LP_IRQn ,NVIC_EncodePriority(TARGET_PRIO_GROUP, 2UL , 0UL) );
	__NVIC_EnableIRQ(USB_LP_IRQn);
	__NVIC_SetPriority(USBWakeUp_IRQn ,NVIC_EncodePriority(TARGET_PRIO_GROUP, 1UL , 0UL) );
	__NVIC_EnableIRQ(USBWakeUp_IRQn);

	return ERR_OK ;

}

static inline err_t Disable_NVIC_IRQ( void )
{
	__NVIC_DisableIRQ(USB_HP_IRQn);
	__NVIC_DisableIRQ(USB_LP_IRQn);
	__NVIC_DisableIRQ(USBWakeUp_IRQn);

	return ERR_OK ;
}

static inline err_t Clear_NVIC_IRQ( void )
{
	__NVIC_ClearPendingIRQ(USB_HP_IRQn);
	__NVIC_ClearPendingIRQ(USB_LP_IRQn);
	__NVIC_ClearPendingIRQ(USBWakeUp_IRQn);

	return ERR_OK ;
}


err_t hwInit( void ){

	/* Disable USB NVIC Line */
	Disable_NVIC_IRQ();
	/* Clear Pending */
	Clear_NVIC_IRQ();
	/* Disable Perihperal */
	Disable();
	/* Reset Peripheral */
	PerphReset();
	/* Enable Peripheral */
	Enable();
	/* Initialize HW Transceivers */
	InitializeTranceivers();
	/* Enable NVIC IRQ */
	Enable_NVIC_IRQ();

	/* Clear Pending IRQ*/
	USB->ISTR &= ~( USB_ISTR_WKUP_Msk | USB_ISTR_ERR_Msk | USB_ISTR_ESOF_Msk | USB_ISTR_PMAOVR_Msk | USB_ISTR_RESET_Msk |
					USB_ISTR_SUSP_Msk | USB_ISTR_SOF_Msk );
	/* Enable all Interrupts  */
	USB->CNTR |= ( USB_CNTR_CTRM_Msk | USB_CNTR_ERRM_Msk | USB_CNTR_ESOFM_Msk |
				 USB_CNTR_PMAOVRM_Msk | USB_CNTR_RESETM_Msk | USB_CNTR_SOFM_Msk |
				 USB_CNTR_WKUPM_Msk )  ;
	return ERR_OK ;

}

err_t hwRegisterReset( void ){

	for ( int32_t i = 0 ; i < HW_NUM_EPS ; i++){
		uint32_t * epxr = LWUSB_GET_EPR(i);
		/* Disable Endpoints */
		*(epxr) = 0u ;
	}
	return ERR_OK ;
}


err_t hwMemoryReset( void ){

	USB_pmaInit();
	return ERR_OK ;
}

err_t hwEnable( void ){

	USB->DADDR |= (USB_DADDR_EF_Msk);
	return ERR_OK ;
}

err_t hwDisable ( void ){

	USB->DADDR &= ~(USB_DADDR_EF_Msk);
	return ERR_OK ;
}

err_t hwSetAddress( uint8_t Address ){

	USB->DADDR &= ~ ( USB_DADDR_ADD_Msk );
	USB->DADDR |= ( Address << USB_DADDR_ADD_Pos );
	return ERR_OK ;
}

int32_t hwWriteData( uint8_t lwEPNum , uint8_t * lwData , uint16_t lwLen ){

	int32_t status = USB_pmaWrite(lwEPNum, lwData , lwLen);
	if ( status < 0 ){
		return ERR_FAULT ;
	}
	USB_pmaWriteNumBytes(lwEPNum ,lwLen);
	return status;
}

int32_t hwReadData( uint8_t lwEPNum , uint8_t* lwData ){

	int32_t status = USB_pmaGetNumBytes( lwEPNum );
	int32_t Length = status ;
	if ( status < 0 ){
		return ERR_FAULT ;
	}
	USB_pmaRead( lwEPNum , lwData, Length );
	return Length ;
}

int32_t  hwSetTXResponse ( uint8_t lwEPNum ,  uint8_t lwResp ){
	return LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(lwEPNum) , lwResp  );
}


int32_t  hwSetRXResponse ( uint8_t lwEPNum  ,  uint8_t lwResp ){
	return LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(lwEPNum) , lwResp  );
}


int32_t  hwIsSetupFlag ( uint8_t lwEPNum  ){
	return LWUSB_READ_EPR(lwEPNum)&USB_EP0R_SETUP_Msk ;
}

int32_t  hwIsOUTFlag (  uint8_t lwEPNum  ){
	return LWUSB_READ_EPR(lwEPNum)&USB_EP0R_CTR_RX_Msk ;
}

int32_t  hwIsINFlag (  uint8_t lwEPNum  ) {
	return LWUSB_READ_EPR(lwEPNum)&USB_EP0R_CTR_TX_Msk ;
}

void  hwClearINFlag(  uint8_t lwEPNum  ){
	LWUSB_CLEAR_CTR_TX(LWUSB_GET_EPR(lwEPNum));
}

void  hwClearOUTFlag(  uint8_t lwEPNum  ){
	LWUSB_CLEAR_CTR_RX(LWUSB_GET_EPR(lwEPNum));
}
void  hwClearSetupFlag(  uint8_t lwEPNum  ){
	LWUSB_CLEAR_CTR_RX(LWUSB_GET_EPR(lwEPNum));
}

int32_t hwGetNumReceivedBytes ( uint8_t lwEPNum ){
	return USB_pmaGetNumBytes( lwEPNum );
}

int32_t hwWriteNumTransmittedBytes ( uint8_t lwEPNum , uint32_t lwLen ){
	return USB_pmaWriteNumBytes(lwEPNum ,lwLen);
}

int32_t hwConfigureEndPoint ( uint8_t lwEPNum , uint8_t lwEPAddr , uint8_t lwEPType , uint8_t lwEPDir ){

	uint8_t EndPoint_Type_Translate[4u];

	EndPoint_Type_Translate[e_lwUSB_EndPoint_Type_Control]     = 1u ;
	EndPoint_Type_Translate[e_lwUSB_EndPoint_Type_Bulk]        = 0u ;
	EndPoint_Type_Translate[e_lwUSB_EndPoint_Type_Isochronous] = 2u ;
	EndPoint_Type_Translate[e_lwUSB_EndPoint_Type_Interrupt]   = 3u ;

	uint32_t * epxr = LWUSB_GET_EPR(lwEPNum);


	/* Setup EP */
	*(epxr) = (~( USB_EP0R_EP_TYPE_Msk | USB_EP0R_EP_KIND_Msk | USB_EP0R_EA_Msk ) )&LWUSB_SAVE_EP_TOGG;
	*(epxr) = ((EndPoint_Type_Translate[lwEPType] << USB_EP0R_EP_TYPE_Pos ) | (lwEPAddr << USB_EP0R_EA_Pos   ))&LWUSB_SAVE_EP_TOGG;

	return ERR_OK ;
}

void * hwAllocate( uint8_t lwEPNum , size_t lwSize , uint8_t lwEPType , uint8_t lwEPDirection ){

	void* retval =  USB_pmaAllocate(lwEPNum, lwSize);

	if ( retval && lwEPDirection == e_lwUSB_EndPoint_Direction_IN ){
		USB_pmaWriteTXEntry(lwEPNum, retval ) ;
	}
	else if ( retval && lwEPDirection == e_lwUSB_EndPoint_Direction_OUT ){
		USB_pmaWriteRXEntry(lwEPNum ,retval , lwSize);

	}

	return retval ;
}
