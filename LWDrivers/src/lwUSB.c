



#include "lwUSB.h"
#include "lwUSB_Descriptors.h"

#define STM32F103_USB_TRANSCEIVER_STARTUP_US 1u
#define STM32F103_USB_PERIPHERAL_RESET_DELAY_NS 1u

	static uint8_t debug = 0 ;

//todo remove
extern volatile int32_t usb_state ;
static uint32_t  sofc = 0  ;
static uint32_t string_indexing  = 0 ;
static uint32_t get               = 0 ;
static uint8_t lwUSB_next_config_value  = LWUSB_CONFIG_VALUE_START ;
static uint8_t lwUSB_next_string_index  = LWUSB_STRING_INDEX_START ;
struct lwUSB_controller_s {

	uint32_t isInitialized ;					  /* Initialization Checker */
	e_lwUSB_controller_state lwUSB_device_state ; /* device state */
	uint8_t device_addr ;						  /* assigned address to the device */
	struct lwUSB_setup_data request ;		      /* Pointer to the setup request data */
	uint8_t isrequest ;                           /* is set when a setup transaction is completed and cleared on the end of the status stage */
	struct lwUSB_ep_s * lwUSB_eps[LWUSB_EP_NUM];  /* Array of pointers to EPs data structs  */
	struct lwUSB_configuration_s * configurations ;
	struct lwUSB_string_s *        strings ;
	struct lwUSB_configuration_s * current_configuration ;



};

static struct lwUSB_transaction_s c_ep_t1 = {
		.current_state  = e_lwUSB_transaction_state_null ,
		.previous_state = e_lwUSB_transaction_state_null ,
		.payload_buffer = NULL ,
		.payload_length = 0u ,
		.payload_ptr    = 0u ,
		.next = NULL ,

};
static struct lwUSB_transaction_s c_ep_t2 = {
		.current_state  = e_lwUSB_transaction_state_null ,
		.previous_state = e_lwUSB_transaction_state_null ,
		.payload_buffer = NULL ,
		.payload_length = 0u ,
		.payload_ptr    = 0u ,
		.next = NULL ,

};


static struct lwUSB_ep_s control_0 = {

			.isInitialized = 0u ,
			.dir = e_lwUSB_ep_dir_inout ,
			.ep_num = 0 ,
			.ep_addr = 0 ,
			.type = e_lwUSB_ep_type_control ,
			.kind = e_lwUSB_ep_kind_single_buffered ,
			.t_in = &c_ep_t1 ,
			.t_out = &c_ep_t2 ,
			.tx_state = e_lwUSB_ep_state_disabled ,
			.rx_state = e_lwUSB_ep_state_valid ,
			.txb_size = LWUSB_CONTROL_EP_TX_B_SIZE ,
			.rxb_size = LWUSB_CONTROL_EP_RX_B_SIZE

	};


static struct lwUSB_controller_s lwUSB_controller = {
		.isInitialized = 0u ,
		.lwUSB_device_state = e_lwUSB_controller_state_Default ,
		.configurations = NULL ,
		.current_configuration = NULL ,
		.strings = NULL ,
		.device_addr = 0u ,
		.isrequest = 0u ,
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


static inline err_t lwUSB_HW_SetAddress ( uint8_t addr )
{
	USB->DADDR &= ~ ( USB_DADDR_ADD_Msk );
	USB->DADDR |= ( addr << USB_DADDR_ADD_Pos );
	return ERR_OK ;
}


struct lwUSB_string_s * lwUSB_FindStringbyIndex ( uint8_t stringID ){

	struct lwUSB_string_s * s ;

	for ( s = lwUSB_controller.strings ; s != NULL ; s = s->next ){
		if ( s->sid == stringID ){
			return s ;
		}
	}

	return NULL ;

}
err_t  lwUSB_RegisterString( uint8_t stringID , struct lwUSB_string_descriptor_s * d_string  ) {

	if ( !d_string ){
		return ERR_NULL ;
	}

	if ( lwUSB_FindStringbyIndex(stringID) != NULL ){
		return ERR_FAULT ;
	}

	struct lwUSB_string_s * s = (struct lwUSB_string_s *)malloc(sizeof(struct lwUSB_string_s ));
	if ( !s ){
		return ERR_NULL ;
	}
	s->sid = stringID ;
	s->d_string = d_string ;

	s->next = lwUSB_controller.strings ;
	lwUSB_controller.strings = s ;

	return ERR_OK ;
}

err_t lwUSB_Copy_String( uint8_t* sbuff ,struct lwUSB_string_s * string ){

	if (!string || !sbuff ){
		return ERR_NULL ;
	}
	uint32_t p ;

	if ( string->sid == 0u ){
		 p = 0u ;
		 uint8_t* s_d_cast = (uint8_t*) string->d_string ;
		 for ( uint32_t i = 0 ; i < sizeof(struct lwUSB_string_descriptor0_s) ; i++ ){
			 sbuff[p++] = s_d_cast[i];
		 }
	}
	else{
		/* UTF16-LE Support */
		p = 2;
		uint8_t* s_d_cast = (uint8_t*) string->d_string->bString ;
		for ( uint32_t i = 0 ; s_d_cast[i] != 0u ; i++ ) {
			sbuff[p++] = s_d_cast[i] ;
			if ( !( s_d_cast[i] >= 0x20 && s_d_cast[i]  <= 0x7F )){
				return ERR_FAULT ;
			}
			sbuff[p++] = 0u ;
		}
		if ( string->sid == 0xee ){
			sbuff[p++] = ((struct lwUSB_microsoft_os_string_descriptor_s*)string->d_string)->VendorCode ;
			sbuff[p++] = ((struct lwUSB_microsoft_os_string_descriptor_s*)string->d_string)->reserved ;

		}
		sbuff[0] = p ;
		sbuff[1] = e_lwUSB_bdescriptor_type_string ;
	}

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
	lwUSB_controller.isInitialized++;
	lwUSB_controller.lwUSB_eps[0] = &control_0 ;
	/* return to default state */
	lwUSB_controller.lwUSB_device_state = e_lwUSB_controller_state_Default ;

	lwUSB_HW_SetAddress(0u);

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

	USB->DADDR&= ~( USB_DADDR_ADD_Msk | USB_DADDR_EF_Msk );


	/* Register Device String Descriptors */
	lwUSB_RegisterString( 0u, (struct lwUSB_string_descriptor_s*)&lwUSB_string0_descriptor);
	lwUSB_RegisterString( 1u, &lwUSB_manufacturer_string_descriptor);
	lwUSB_RegisterString( 2u, &lwUSB_product_string_descriptor);
	lwUSB_RegisterString( 3u, &lwUSB_serial_string_descriptor);
	lwUSB_RegisterString( 0xee ,(struct lwUSB_string_descriptor_s*) &lwUSB_microsoft_os_string_descriptor);




	return ERR_OK ;

}


err_t lwUSB_LoadPacket( uint8_t * ptr , uint32_t len  , struct lwUSB_ep_s * ep )
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

	struct lwUSB_btable_ep_entry_s * entry = lwUSB_pmaGetEntry( ep->ep_num );
	entry->tx_buff.count = len ;

	if ( !len ){
		return ERR_OK ;
	}

	//todo support double buffers

	lwUSB_pmaWrite( ptr, len, ep->txb_addr );
	return  ERR_OK ;

}


static inline err_t lwUSB_SetAddress ( uint8_t  addr )
{
	lwUSB_controller.device_addr = addr ;
	lwUSB_controller.lwUSB_device_state = e_lwUSB_controller_state_address ;
	lwUSB_HW_SetAddress(addr);
	return ERR_OK ;
}
static inline err_t lwUSB_Protocol_Stall ( struct lwUSB_ep_s * ep )
{
	LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(ep->ep_num) , e_lwUSB_ep_state_stall );
	LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(ep->ep_num) , e_lwUSB_ep_state_stall );
	return ERR_OK ;
}

static inline err_t lwUSB_Ready_ZLP( struct lwUSB_ep_s * ep  )
{
	static const uint32_t zlp = 0xa5a5a5a5 ;
	lwUSB_LoadPacket( (void*)zlp , 0u , ep);
	return ERR_OK ;
}




err_t lwUSB_Set_Transaction_IN_State ( struct lwUSB_ep_s * ep , e_lwUSB_transaction_state state  )
{
	ep->t_in->current_state = state ;
	return ERR_OK ;
}

/* This Function Registers an IN Transaction on the EP */
err_t lwUSB_Transfer_IN(struct lwUSB_ep_s * ep ,  uint8_t * ptr , uint32_t length  )
{
	LW_ASSERT(ep);
	LW_ASSERT(ptr);
	/* Make Sure EP is Initialized , and has an IN channel */
	if ( !(ep->dir == e_lwUSB_ep_dir_inout ||  ep->dir == e_lwUSB_ep_dir_d_in) )
	{
		return ERR_FAULT ;
	}

	if ( ep->isInitialized == 0 )
	{
		return ERR_FAULT ;
	}
	//todo extend the linked list idea
	if ( ep->t_in == NULL || ep->t_in->current_state == e_lwUSB_transaction_state_null )
	{
		return ERR_FAULT ;
	}

	ep->t_in->payload_buffer = ptr ;
	ep->t_in->payload_length = length ;

	if ( length <= ep->txb_size )
	{
		/* Whole payload in one transaction */
		lwUSB_LoadPacket( ptr , length , ep);
		ep->t_in->payload_ptr = length ;
	}
	else{
		lwUSB_LoadPacket( ptr , ep->txb_size  , ep);
		ep->t_in->payload_ptr = ep->txb_size ;
	}

	LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(ep->ep_num) , e_lwUSB_ep_state_valid );
	LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(ep->ep_num) , e_lwUSB_ep_state_stall );

	ep->t_in->previous_state = ep->t_in->current_state ;
	ep->t_in->current_state  = e_lwUSB_transaction_state_data_in ;

	return ERR_OK ;


}

static err_t lwUSB_Copy_Configuration ( uint8_t * dest , struct lwUSB_configuration_s * config )
{

	struct lwUSB_interface_s * intf  ;
	struct lwUSB_endpoint_s  * ep    ;
	uint32_t p = 0 ;
	uint8_t* config_cast = (uint8_t*)config->d_configuration ;
	for ( uint32_t i = 0 ; i < config->d_configuration->bLength ; i++ ){
		dest[p++] = config_cast[i] ;
	}
	for( intf = config->interface ; intf != NULL ; intf = intf->next ){
		uint8_t * intf_cast = (uint8_t*)intf->d_interface ;
		for ( uint32_t i = 0 ; i < intf->d_interface->bLength ; i++ ){
			dest[p++] = intf_cast[i] ;
		}
		for( ep = intf->endpoint ; ep != NULL ; ep = ep->next ){
			uint8_t * ep_cast = (uint8_t*) ep->d_endpoint ;
			for ( uint32_t i = 0 ; i < ep->d_endpoint->bLength ; i++ ){
				dest[p++] = ep_cast[i];
			}

		}
	}

	if ( p != config->d_configuration->wTotalLength ){
		return ERR_FAULT ;
	}

	return ERR_OK ;

}
static err_t lwUSB_Handle_CTR ( uint8_t ep_num )
{
	//todo remove
	LW_ASSERT( ep_num <= LWUSB_EP_NUM-1 );
	LW_ASSERT( lwUSB_controller.isInitialized != 0u );
	LW_ASSERT( lwUSB_controller.lwUSB_eps[ep_num] != NULL );
	LW_ASSERT( lwUSB_controller.lwUSB_eps[ep_num]->isInitialized != 0u );

	int unused = 1 ;
	uint32_t* epxr = LWUSB_GET_EPR(ep_num);
	struct lwUSB_ep_s * curr_ep = lwUSB_controller.lwUSB_eps[ep_num] ;

	if( !curr_ep || curr_ep->isInitialized == 0u )
	{
		//todo disable the EP
		LWUSB_WRITE_STAT_TX(epxr , e_lwUSB_ep_state_disabled);
		LWUSB_WRITE_STAT_RX(epxr , e_lwUSB_ep_state_disabled);
		*epxr = 0u ;
		return ERR_FAULT ;
	}



	uint32_t setup_completed = *(epxr) & USB_EP0R_SETUP_Msk ;
	uint32_t out_completed    = *(epxr) & USB_EP0R_CTR_RX_Msk ;
	uint32_t in_completed  = *(epxr) & USB_EP0R_CTR_TX_Msk ;

	if ( setup_completed ){
		LWUSB_CLEAR_CTR_RX(epxr) ;
		struct lwUSB_btable_ep_entry_s * entry = lwUSB_pmaGetEntry(ep_num);
		uint16_t t_len = entry->rx_buff.count & USB_COUNT0_RX_COUNT0_RX_Msk ;
		lwUSB_pmaRead( (uint8_t*)&lwUSB_controller.request , t_len , curr_ep->rxb_addr );
		struct lwUSB_setup_data * request = &lwUSB_controller.request ;


		switch( request->bRequest )
		{
		case e_lwUSB_setup_request_get_status  :
			debug++;
			debug++;
			USE(unused);
			break;
		case e_lwUSB_setup_request_clear_feature :
			debug++;
			debug++;
			USE(unused);
			break;
		case e_lwUSB_setup_request_set_feature :
			debug++;
			debug++;
			USE(unused);
			break;
		case e_lwUSB_setup_request_set_address  :
			lwUSB_controller.isrequest++;
			if (request->wValue != 0u ){
				if ( request->wLength != 0 ){
					//todo handler for set Request with data stage
				}
				if ( lwUSB_controller.lwUSB_device_state == e_lwUSB_controller_state_Default ){
					lwUSB_controller.device_addr = (uint8_t)request->wValue ;
					/* Transfer a ZLP */
					lwUSB_Transfer_IN(curr_ep, (void*)(!NULL) , 0u);
				}
				else{
					//todo handler
				}
			}
			else{
				//todo handler
			}
			break;
		case e_lwUSB_setup_request_get_descriptor  :
			debug++;
			get++;

			if ( get == 7 )
			{
				debug++;
				debug++;
			}
			lwUSB_controller.isrequest++;
			if ( request->bmRequestType.Direction == BMREQUEST_DIRECTION_DEVICE_TO_HOST &&
				 request->bmRequestType.Recipient == BMREQUEST_RECIPIENT_DEVICE &&
				 request->bmRequestType.RequestType == BMREQUEST_TYPE_STANDARD ){

					lwUSB_Set_Transaction_IN_State( curr_ep , e_lwUSB_transaction_state_setup_stage );
					if ( request->descriptorType == e_lwUSB_bdescriptor_type_device ){
						lwUSB_Transfer_IN( curr_ep , (uint8_t*)&lwUSB_Device_Descriptor , sizeof(struct lwUSB_device_descriptor_s ) ) ;
					}
					else if ( request->descriptorType == e_lwUSB_bdescriptor_type_configuration )
					{
						uint8_t config_num = request->descriptorIndex  ;
						struct lwUSB_configuration_s * p = lwUSB_controller.configurations ;
						for (  ; p != NULL  ; p = p->next  ){
							if ( config_num == LWUSB_GET_CONFIG_V_NUM(p->d_configuration->bConfigurationValue ) ){
								break ;
							}
						}

						if ( !p ){
							/* No such configuration */
							/*todo  check handling  */
							LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );
							LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );
						}
						else{
							uint8_t cbuff[p->d_configuration->wTotalLength];
							lwUSB_Copy_Configuration(cbuff, p);
							/* Send Configuration Descriptor */
							lwUSB_Transfer_IN( curr_ep , cbuff , p->d_configuration->wTotalLength  ) ;

						}
					}
					else if (  request->descriptorType == e_lwUSB_bdescriptor_type_string ){

						string_indexing++;

						struct lwUSB_string_s * s = lwUSB_FindStringbyIndex(request->descriptorIndex);
						if ( !s ){
							LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );
							LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );
							break ;
						}
						uint32_t b_size = request->descriptorIndex == 0 ? (sizeof(struct lwUSB_string_descriptor0_s)) : ( strlen(s->d_string->bString)*2u + 2u );
						uint8_t sbuff[b_size];
						lwUSB_Copy_String(sbuff , s );
						lwUSB_Transfer_IN( curr_ep , sbuff , b_size  ) ;

					}
					else if ( request->descriptorType == e_lwUSB_bdescriptor_type_device_qualifier )
					{
						debug++;
						debug++;
					}
					else{

						LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );
						LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );
					}

			}
			else{
					//todo handler
				debug++;
				debug++;

			}
			break;
		case e_lwUSB_setup_request_set_descriptor :
			debug++;
			debug++;
			break;
		case e_lwUSB_setup_request_get_configuration  :
			debug++;
			debug++;
			USE(unused);
			break;
		case e_lwUSB_setup_request_set_configuration    :
			debug++;
			debug++;
			break;
		case e_lwUSB_setup_request_get_interface     :
			debug++;
			debug++;
			break;
		case e_lwUSB_setup_request_set_interface      :
			debug++;
			debug++;

			break;
		case e_lwUSB_setup_request_synch_frame        :
			break;
		case e_lwUSB_setup_request_set_sel            :
			break;
		case e_lwUSB_setup_request_set_isochronous_delay :
			break;
		default:
			debug++;
			debug++;
			return ERR_FAULT;
		}
	}
	else if ( out_completed ){

		LWUSB_CLEAR_CTR_RX(epxr) ;

		if( curr_ep->type == e_lwUSB_ep_type_control ){

			/* Status received for now todo */
			lwUSB_controller.isrequest = 0u ;
			LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );
			LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );

		}

	}
	else if ( in_completed ){

		LWUSB_CLEAR_CTR_TX(epxr) ;
		if ( curr_ep->t_in->payload_ptr < curr_ep->t_in->payload_length  ){
			/* We need to load another packet , since more stages are to come  */
			lwUSB_Transfer_IN(curr_ep, &curr_ep->t_in->payload_buffer[curr_ep->t_in->payload_ptr] , curr_ep->t_in->payload_length - curr_ep->t_in->payload_ptr );
		}
		else {
			/* IN Completed */
			if( curr_ep->type == e_lwUSB_ep_type_control )
			{
			//	if ( curr_ep->t_in->previous_state == e_lwUSB_transaction_state_setup_stage ){
					//assume that all control ep transactions came from setup stage 183
				 	// we must now receive the ACK in the Status Stage
				if (  lwUSB_controller.isrequest && lwUSB_controller.request.bRequest == e_lwUSB_setup_request_set_address ){
					/* ZLP Handle , means an ACK in the Status Stage */
					if ( curr_ep->t_in->payload_length == 0u ) {
						lwUSB_SetAddress(lwUSB_controller.device_addr);
						lwUSB_controller.lwUSB_device_state = e_lwUSB_controller_state_address ;
						LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );
						LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall);
						lwUSB_controller.isrequest = 0u ;


					}
					else{
						/* NON ZLP Status Stage */
					}

				}
				else{
				LWUSB_WRITE_STAT_TX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_stall );
				LWUSB_WRITE_STAT_RX(LWUSB_GET_EPR(curr_ep->ep_num) , e_lwUSB_ep_state_valid );
				}

		//		}

			}
		}
	}
	else{
		//todo error

	}



	return ERR_OK ;
}


//todo only 2 byte sized blocks are used make it work with both 32 and 2

err_t lwUSB_Initialize_Endpoint ( struct lwUSB_ep_s * lwusb_ep )
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




err_t lwUSB_RegisterInterface ( struct lwUSB_interface_s * interface , struct lwUSB_configuration_s * config ){

	if ( !interface || !config ){
		return ERR_NULL ;
	}
	uint32_t index = 0u ;
	for ( struct lwUSB_interface_s *  temp_intf = config->interface ; temp_intf != NULL ; temp_intf = temp_intf->next , index++ ){
		if ( UINT32T_CAST(temp_intf) == UINT32T_CAST(interface) ){
			return ERR_FAULT ;
		}
	}
	interface->d_interface->bInterfaceNumber = index ;
	config->d_configuration->bNumInterfaces++ ;
	/* Register EP to the interface */
	interface->next = config->interface ;
	config->interface = interface ;

	return ERR_OK ;


}

err_t lwUSB_RegisterEndpoint ( struct lwUSB_endpoint_s * endpoint , struct lwUSB_interface_s * interface ){

	if ( !endpoint || !interface ){
		return ERR_NULL ;
	}

	if ( interface->d_interface->bNumEndpoints >= LWUSB_EP_NUM ){
		return ERR_FAULT ;
	}

	for ( struct lwUSB_endpoint_s *  temp_ep = interface->endpoint ; temp_ep != NULL ; temp_ep = temp_ep->next ){
		if ( UINT32T_CAST(temp_ep) == UINT32T_CAST(endpoint) ){
			return ERR_FAULT ;
		}
	}
	interface->d_interface->bNumEndpoints++ ;
	/* Register EP to the interface */
	endpoint->next = interface->endpoint ;
	interface->endpoint = endpoint ;

	return ERR_OK ;

}
struct lwUSB_endpoint_s * lwUSB_InitializeEndpoint( struct lwUSB_endpoint_descriptor_s * d_endpoint ) {

	d_endpoint->bLength = 7u ;
	d_endpoint->bDescriptorType = e_lwUSB_bdescriptor_type_endpoint ;
	if ( ( d_endpoint->bEndpointAddress & 0x0F )  > LWUSB_EP_NUM -1 ){
		return NULL ;
	}
	if ( d_endpoint->bInterval  == 0u ){
		return NULL ;
	}

	struct lwUSB_endpoint_s * endpoint = (struct lwUSB_endpoint_s * )malloc(sizeof(struct lwUSB_endpoint_s ));

	if ( !endpoint ){
		return NULL ;
	}

	endpoint->d_endpoint = d_endpoint ;

	return endpoint ;

}

struct lwUSB_interface_s * lwUSB_InitializeInterface ( struct lwUSB_interface_descriptor_s * d_interface ) {

	d_interface->bLength            = 9u ;
	d_interface->bDescriptorType    = e_lwUSB_bdescriptor_type_interface ;
	d_interface->bInterfaceNumber   = 0u ;
	d_interface->bAlternateSetting  = 0u ;
	d_interface->bInterfaceClass    = 0u ;
	d_interface->bInterfaceSubclass = 0u ;
	d_interface->bInterfaceProtocol = 0u ;
	d_interface->bNumEndpoints      = 0u ;
	if ( d_interface->iInterface ){
		d_interface->iInterface = lwUSB_next_string_index++ ;
	}
	struct lwUSB_interface_s *  interface = (struct lwUSB_interface_s * )malloc(sizeof(struct lwUSB_interface_s ));

	if ( !interface ){
		return NULL ;
	}

	interface->d_interface = d_interface ;

	return interface ;

}

struct lwUSB_configuration_s *  lwUSB_InitializeConfiguration ( struct lwUSB_configuration_descriptor_s * d_config ){
	/* Force constant field values */
	d_config->bLength             = 9u ;
	d_config->wTotalLength        = 0u ;
	d_config->bNumInterfaces      = 0u ;
	d_config->bDescriptorType     = e_lwUSB_bdescriptor_type_configuration ;
	d_config->bConfigurationValue = lwUSB_next_config_value++ ;
	if ( d_config->iConfiguration ){
		d_config->iConfiguration = lwUSB_next_string_index++ ;
	}

	struct lwUSB_configuration_s *  config = (struct lwUSB_configuration_s *)malloc(sizeof(struct lwUSB_configuration_s));
	if ( !config  ){
		return NULL ;
	}

	config->d_configuration = d_config ;
	return config ;

}


err_t lwUSB_RegisterConfiguration ( struct lwUSB_configuration_s * config )
{
	if ( !config || !config->interface || !config->d_configuration ){
		return ERR_NULL ;
	}

	for ( struct lwUSB_configuration_s *  temp_config = lwUSB_controller.configurations ; temp_config != NULL ; temp_config = temp_config->next ){
		if ( UINT32T_CAST(temp_config) == UINT32T_CAST(config) ){
			return ERR_FAULT ;
		}
	}
	/* Check Valid total length */

	uint32_t totalLength = 0u ;
	struct lwUSB_interface_s * interface ;
	struct lwUSB_endpoint_s * endpoint ;
	for ( interface = config->interface ; interface != NULL ; interface = interface->next ){
		totalLength += interface->d_interface->bLength ;
		for ( endpoint = interface->endpoint ; endpoint != NULL ; endpoint = endpoint->next ){
			totalLength += endpoint->d_endpoint->bLength ;
		}
	}
	lwUSB_Device_Descriptor.bNumConfigurations++ ;
	config->d_configuration->wTotalLength = config->d_configuration->bLength + totalLength ;

	config->next = lwUSB_controller.configurations ;
	lwUSB_controller.configurations = config ;

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
		// register ep1
		if ( !(debug++) ){
		struct lwUSB_configuration_s * config_1 = lwUSB_InitializeConfiguration(&lwUSB_Configuration_Descriptor );
		struct lwUSB_interface_s     * intf_1   = lwUSB_InitializeInterface(&lwUSB_interface1_Descriptor);
		struct lwUSB_endpoint_s      * ep_1     = lwUSB_InitializeEndpoint(&lwUSB_endpoint1_Descriptor);
		lwUSB_RegisterEndpoint(ep_1, intf_1);
		lwUSB_RegisterInterface(intf_1, config_1);
		lwUSB_RegisterConfiguration(config_1);
		}
		//Flag
		usb_state = USB_STATE_RESET;
	}
	else if ( USB->ISTR & USB_ISTR_CTR_Msk )
	{

		/* Handle Transfer*/
		lwUSB_Handle_CTR( USB->ISTR & USB_ISTR_EP_ID_Msk );
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




