



#include "lwUSB.h"
#include "lwUSB_Descriptors.h"

#define STM32F103_USB_TRANSCEIVER_STARTUP_US 1u
#define STM32F103_USB_PERIPHERAL_RESET_DELAY_NS 1u



//todo remove
extern volatile int32_t usb_state ;
static uint32_t  sofc = 0  ;


static struct e_lwUSB_ep_s control_0 = {

			.isInitialized = 0u ,
			.dir = e_lwUSB_ep_dir_inout ,
			.ep_num = 0 ,
			.ep_addr = 0 ,
			.type = e_lwUSB_ep_type_control ,
			.kind = e_lwUSB_ep_kind_single_buffered ,
			.tx_state = e_lwUSB_ep_state_disabled ,
			.rx_state = e_lwUSB_ep_state_valid ,
			.txb_size = LWUSB_CONTROL_EP_TX_B_SIZE ,
			.rxb_size = LWUSB_CONTROL_EP_RX_B_SIZE

	};


static struct lwUSB_controller_s lwUSB_controller = {
		.isInitialized = 0u ,
		.lwUSB_device_state = e_lwUSB_controller_state_Default ,
		.lwUSB_eps = {NULL}
};





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


err_t lwUSB_Reset( void )
{

	/* Reset PMA , all EPS are De-allocated*/
	lwUSB_pmaInit();

	for ( int32_t i = 0 ; i < LWUSB_EP_NUM ; i++)
	{
		uint32_t * epxr = LWUSB_GET_EPR(i);
		lwUSB_controller.lwUSB_eps[i] = NULL ;
		/* Disable Endpoints */
		*(epxr) = 0u ;

	}

	/* Initialize Control EP */
	lwUSB_controller.lwUSB_eps[0] = &control_0 ;
	err_t status = lwUSB_Initialize_Endpoint( lwUSB_controller.lwUSB_eps[0] ) ;

	if ( status != ERR_OK )
	{
		lwUSB_controller.lwUSB_eps[0] = NULL ;
		return ERR_FAULT ;
	}
	lwUSB_controller.lwUSB_eps[0]->isInitialized++;

	lwUSB_EnableUSBFunction();

	return ERR_OK ;

}

err_t lwUSB_Init( void )
{
	lwUSB_HardwareReset();

	/* Clear Pending */

	USB->ISTR &= ~( USB_ISTR_WKUP_Msk | USB_ISTR_ERR_Msk | USB_ISTR_ESOF_Msk | USB_ISTR_PMAOVR_Msk | USB_ISTR_RESET_Msk |
					USB_ISTR_SUSP_Msk | USB_ISTR_SOF_Msk );
	/* Enable */

	USB->CNTR |= ( USB_CNTR_CTRM_Msk | USB_CNTR_ERRM_Msk | USB_CNTR_ESOFM_Msk |
				 USB_CNTR_PMAOVRM_Msk | USB_CNTR_RESETM_Msk | USB_CNTR_SOFM_Msk |
				 USB_CNTR_WKUPM_Msk )  ;


	return ERR_OK ;

}


err_t lwUSB_LoadPacket( uint8_t * ptr , uint32_t len  , struct e_lwUSB_ep_s * ep )
{

	LW_ASSERT( ep != NULL );
	LW_ASSERT( ep->isInitialized != 0u ) ;


	if ( !( len <= ep->txb_size ) ){
		return ERR_FAULT ;
	}

	if(!(ep->dir == e_lwUSB_ep_dir_s_in ||
			   ep->dir == e_lwUSB_ep_dir_inout ||
			   ep->dir == e_lwUSB_ep_dir_d_in ) ){
		return ERR_FAULT ;
	}

	//todo support double buffers

	lwUSB_pmaWrite( ptr, len, ep->txb_addr );

	struct lwUSB_btable_ep_entry_s * entry = lwUSB_pmaGetEntry( ep->ep_num );

	entry->tx_buff.count = len ;

	LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(ep->ep_num) , e_lwUSB_ep_state_valid );
	return  ERR_OK ;

}



static err_t lwUSB_Handle_CTR ( uint8_t ep_num )
{
	LW_ASSERT( ep_num <= LWUSB_EP_NUM-1 );
	LW_ASSERT( lwUSB_controller.isInitialized != 0u );
	LW_ASSERT( lwUSB_controller.lwUSB_eps[ep_num] != NULL );
	LW_ASSERT( lwUSB_controller.lwUSB_eps[ep_num]->isInitialized != 0u );


	uint32_t* epxr = LWUSB_GET_EPR(ep_num);
	struct e_lwUSB_ep_s * curr_ep = lwUSB_controller.lwUSB_eps[ep_num] ;

	if( !curr_ep || curr_ep->isInitialized == 0u )
	{
		//todo disable the EP
		LWUSB_WRITE_STAT_TX(epxr , e_lwUSB_ep_state_disabled);
		LWUSB_WRITE_STAT_RX(epxr , e_lwUSB_ep_state_disabled);
		*epxr = 0u ;
		return ERR_FAULT ;
	}

	if ( curr_ep->type == e_lwUSB_ep_type_control )
	{
		if ( *(epxr) & USB_EP0R_CTR_RX_Msk )
		{
			uint32_t setup = *(epxr) & USB_EP0R_SETUP_Msk ;
			LWUSB_CLEAR_CTR_RX(epxr) ;
			if ( setup ){
				struct lwUSB_btable_ep_entry_s * entry = lwUSB_pmaGetEntry(ep_num);
				uint16_t t_len = entry->rx_buff.count & USB_COUNT0_RX_COUNT0_RX_Msk ;
				uint8_t t_data[t_len] ;
				if ( t_data == NULL )
				{
					return ERR_NULL ;
				}
				lwUSB_pmaRead( t_data , t_len , curr_ep->rxb_addr );
				struct lwUSB_setup_data * request = (struct lwUSB_setup_data *)t_data;

				switch( request->bRequest )
				{
				case e_lwUSB_setup_request_get_status  :
					break;
				case e_lwUSB_setup_request_clear_feature :
					break;
				case e_lwUSB_setup_request_set_feature :
					break;
				case e_lwUSB_setup_request_set_address  :
					break;
				case e_lwUSB_setup_request_get_descriptor  :

					lwUSB_LoadPacket( (uint8_t*)&lwUSB_Device_Descriptor , sizeof(struct lwUSB_device_descriptor_s) , curr_ep);

					break;
				case e_lwUSB_setup_request_set_descriptor :
					break;
				case e_lwUSB_setup_request_get_configuration  :
					break;
				case e_lwUSB_setup_request_set_configuration    :
					break;
				case e_lwUSB_setup_request_get_interface     :
					break;
				case e_lwUSB_setup_request_set_interface      :
					break;
				case e_lwUSB_setup_request_synch_frame        :
					break;
				case e_lwUSB_setup_request_set_sel            :
					break;
				case e_lwUSB_setup_request_set_isochronous_delay :
					break;
				default:
					return ERR_FAULT;
				}
			}


		}
	}





	return ERR_OK ;
}


//todo only 2 byte sized blocks are used make it work with both 32 and 2

err_t lwUSB_Initialize_Endpoint ( struct e_lwUSB_ep_s * lwusb_ep )
{
	LW_ASSERT( lwusb_ep != NULL ) ;
	LW_ASSERT( lwusb_ep->ep_num <= LWUSB_EP_NUM-1 );
	LW_ASSERT( lwusb_ep->type <= LWUSB_EP_TYPE_RANGE );
	LW_ASSERT( lwusb_ep->kind <= LWUSB_EP_KIND_RANGE );


	lwusb_ep->rxb_addr = NULL ;
	lwusb_ep->txb_addr = NULL ;

	struct lwUSB_btable_ep_entry_s entry ;

	entry.tx_buff.count = 0u ;
	entry.rx_buff.count = 0u ;
	entry.rx_buff_d.count = 0u ;
	entry.tx_buff_d.count = 0u ;


	if ( lwusb_ep->kind == e_lwUSB_ep_kind_single_buffered )
	{
		if ( lwusb_ep->dir == e_lwUSB_ep_dir_s_in || lwusb_ep->dir == e_lwUSB_ep_dir_inout )
		{
			lwusb_ep->txb_addr = (uint32_t*) lwUSB_Allocate(lwusb_ep->ep_num, lwusb_ep->txb_size );
			entry.tx_buff.addr = UINT32T_CAST(lwusb_ep->txb_addr);
			if ( lwusb_ep->txb_addr == NULL )
			{
				return ERR_NULL ;
			}

		}
		if (lwusb_ep->dir == e_lwUSB_ep_dir_s_out || lwusb_ep->dir == e_lwUSB_ep_dir_inout )
		{
			lwusb_ep->rxb_addr  = (uint32_t*) lwUSB_Allocate(lwusb_ep->ep_num, lwusb_ep->rxb_size );
			entry.rx_buff.addr  = UINT32T_CAST(lwusb_ep->rxb_addr);
			entry.rx_buff.count =  ((lwusb_ep->rxb_size/LWUSB_PMA_BUFF_ALIGNMENT) <<  USB_COUNT0_RX_NUM_BLOCK_Pos) ;
			if ( lwusb_ep->rxb_addr == NULL )
			{
				return ERR_NULL ;
			}
		}

	}
	else{

		if ( lwusb_ep->dir == e_lwUSB_ep_dir_d_in )
		{
			lwusb_ep->txb_addr   = (uint32_t*) lwUSB_Allocate(lwusb_ep->ep_num, lwusb_ep->txb_size );
			lwusb_ep->txdb_addr  = (uint32_t*) lwUSB_Allocate(lwusb_ep->ep_num, lwusb_ep->txdb_size );
			entry.tx_buff.addr   = UINT32T_CAST(lwusb_ep->txb_addr);
			entry.tx_buff_d.addr = UINT32T_CAST(lwusb_ep->txdb_addr);
			if ( lwusb_ep->txb_addr == NULL || lwusb_ep->txdb_addr  == NULL )
			{
				return ERR_NULL ;
			}

		}
		else if ( lwusb_ep->dir == e_lwUSB_ep_dir_d_out )
		{
			lwusb_ep->rxb_addr   = (uint32_t*) lwUSB_Allocate(lwusb_ep->ep_num, lwusb_ep->rxb_size );
			lwusb_ep->rxdb_addr  = (uint32_t*) lwUSB_Allocate(lwusb_ep->ep_num, lwusb_ep->rxdb_size );
			entry.rx_buff.addr   = UINT32T_CAST(lwusb_ep->rxb_addr);
			entry.rx_buff_d.addr = UINT32T_CAST(lwusb_ep->rxdb_addr);
			entry.rx_buff.count =  ((lwusb_ep->rxb_size/LWUSB_PMA_BUFF_ALIGNMENT) <<  USB_COUNT0_RX_NUM_BLOCK_Pos) ;
			entry.rx_buff_d.count =  ((lwusb_ep->rxdb_size/LWUSB_PMA_BUFF_ALIGNMENT) <<  USB_COUNT0_RX_NUM_BLOCK_Pos) ;
			if ( lwusb_ep->rxb_addr == NULL || lwusb_ep->rxdb_addr == NULL )
			{
				return ERR_NULL ;
			}

		}
	}

	/* Register Entries in BTABLE and Initialize EP specific registers */

	uint32_t * epxr = LWUSB_GET_EPR(lwusb_ep->ep_num);

	/* Setup Btable entry */
	int32_t btable_s = lwUSB_pmaWriteEntry( lwusb_ep->ep_num  , &entry );

	if ( btable_s != ERR_OK )
	{
		return ERR_FAULT ;
	}

	/* Setup EP */

	*(epxr) = (~( USB_EP0R_EP_TYPE_Msk | USB_EP0R_EP_KIND_Msk | USB_EP0R_EA_Msk ) )&LWUSB_SAVE_EP_TOGG;

	*(epxr) = ((lwusb_ep->type << USB_EP0R_EP_TYPE_Pos ) |
			   (lwusb_ep->kind << USB_EP0R_EP_KIND_Pos ) |
			   (lwusb_ep->ep_addr << USB_EP0R_EA_Pos   )
				)&LWUSB_SAVE_EP_TOGG;

	if (( lwusb_ep->dir == e_lwUSB_ep_dir_inout && lwusb_ep->kind == e_lwUSB_ep_kind_single_buffered ) ||  lwusb_ep->kind == e_lwUSB_ep_kind_double_buffered )
	{
		LWUSB_WRITE_STAT_RX(epxr , lwusb_ep->rx_state );
		LWUSB_WRITE_STAT_TX(epxr , lwusb_ep->tx_state );

	}
	else {
		if ( lwusb_ep->dir == e_lwUSB_ep_dir_s_in )
		{
			LWUSB_WRITE_STAT_RX(epxr , e_lwUSB_ep_state_disabled );
			LWUSB_WRITE_STAT_TX(epxr , lwusb_ep->tx_state );

		}
		else{
			LWUSB_WRITE_STAT_RX(epxr , lwusb_ep->rx_state );
			LWUSB_WRITE_STAT_TX(epxr , e_lwUSB_ep_state_disabled);
		}
	}


	return ERR_OK ;

}

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
		//Clear Flag
		USB->ISTR &= ~USB_ISTR_RESET_Msk ;
		//Reset Handler
		lwUSB_Reset();
		//Flag
		usb_state = USB_STATE_RESET;
	}
	else if ( USB->ISTR & USB_ISTR_CTR_Msk )
	{

		/* Handle Transfer*/
		lwUSB_Handle_CTR( USB->ISTR & USB_ISTR_EP_ID_Msk );
		USB->ISTR &= ~USB_ISTR_CTR_Msk ;

		usb_state = USB_STATE_CTR ;
	}
//	if ( USB->ISTR & USB_ISTR_SUSP_Msk )
//	{
//		USB->ISTR &= ~USB_ISTR_SUSP_Msk ;
//
//
//		USB->CNTR &= ~USB_CNTR_SUSPM_Msk ;
//		/* Go into low power mode */
//	//	USB->CNTR |= USB_CNTR_LP_MODE_Msk ;
//		/* This bit is clear on a wakeup event */
//		usb_state = USB_STATE_SUSP;
//
//	}
	else if ( USB->ISTR & USB_ISTR_SOF_Msk )
	{
		USB->ISTR &= ~USB_ISTR_SOF_Msk ;
		usb_state = USB_STATE_SOF;

	}
	else if ( USB->ISTR & USB_ISTR_ESOF_Msk )
	{
		/* Each sofc is 1ms passing*/
		sofc++;
		/* Ignore */
		USB->ISTR &= ~ USB_ISTR_ESOF_Msk ;
		usb_state = USB_STATE_ESOF;

	}
	else if ( USB->ISTR & USB_ISTR_ERR_Msk )
	{
		/*Ignore Errors*/
		/* NANS ERROR , NO Answer from Host */

		USB->ISTR &= ~ USB_ISTR_ERR_Msk ;
		usb_state = USB_STATE_ERR;

	}
	else if ( USB->ISTR & USB_ISTR_WKUP_Msk )
	{
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




