
/* Target Interrupts */

#include "../TargetDrivers/Common.h"

#define USB_STATE_IDLE 0u
#define USB_STATE_RESET 1u
#define USB_STATE_SUSP 2u
#define USB_STATE_SOF 3u
#define USB_STATE_ESOF 4u
#define USB_STATE_ERR 5u
#define USB_STATE_WKUP 6u
#define USB_STATE_CTR 7u

static uint32_t resetCounter = 0 ;
static uint32_t usb_state = 0;

/* Triggered only by a correct transfer event for isochronous and double-buffer bulk transfer
 * to reach the highest possible transfer rate.
 */
void USB_HP_CAN_TX_IRQHandler ( void )
{
	while(1);
}

/* Triggered by all USB events  (Correct transfer, USB reset, etc.).
 * The firmware has to check the interrupt source before serving the interrupt.
 */
void USB_LP_CAN_RX0_IRQHandler ( void )
{
	if ( USB->ISTR & USB_ISTR_RESET_Msk ){
		resetCounter++;
		//Clear Flag
		USB->ISTR &= ~USB_ISTR_RESET_Msk ;
		//Reset Handler
		lwUSB_Reset();
		usb_state = USB_STATE_RESET;
	}
	else if ( USB->ISTR & USB_ISTR_CTR_Msk ){
		USB->ISTR &= ~ USB_ISTR_CTR_Msk  ;
		/* Handle Transfer*/
		lwUSB_Handle_CTR( USB->ISTR & USB_ISTR_EP_ID_Msk );
		usb_state = USB_STATE_CTR ;
	}
	else if ( USB->ISTR & USB_ISTR_SUSP_Msk ){
		USB->ISTR &= ~ USB_ISTR_SUSP_Msk  ;
		usb_state = USB_STATE_SUSP ;
	}
	else if ( USB->ISTR & USB_ISTR_SOF_Msk ){
		USB->ISTR &= ~USB_ISTR_SOF_Msk ;
		usb_state = USB_STATE_SOF;
	}
	else if ( USB->ISTR & USB_ISTR_ESOF_Msk ){
		/* Each sofc is 1ms passing*/
		USB->ISTR &= ~ USB_ISTR_ESOF_Msk ;
		usb_state = USB_STATE_ESOF;
	}
	else if ( USB->ISTR & USB_ISTR_ERR_Msk ){
		/*Ignore Errors*/
		/* NANS ERROR , NO Answer from Host */
		USB->ISTR &= ~ USB_ISTR_ERR_Msk ;
		usb_state = USB_STATE_ERR;
	}
	else if ( USB->ISTR & USB_ISTR_WKUP_Msk ){
		/* RE-Enable SUSPEND INTERRUPTS */
		USB->CNTR |= USB_CNTR_SUSPM_Msk ;
		USB->ISTR &= ~USB_ISTR_WKUP_Msk ;
		usb_state = USB_STATE_WKUP;
	}
}

/* Triggered by wakeup events */

void USBWakeUp_IRQHandler( void )
{
	while(1);
}


