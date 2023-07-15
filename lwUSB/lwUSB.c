#include "lwUSB.h"
#include "Include/lwUSB_Descriptors.h"
#include "Include/lwUSB_Opts.h"
#include "Common.h"

#include "Include/lwUSB_Debug.h"

#define LWUSB_GET_CONFIG_V_NUM(VALUE) (VALUE-LWUSB_OPTS_CONFIG_VALUE_START)
#define LWUSB_STRLEN(STR) (strlen((char*)STR))
#define LWUSB_REGISTER_STRING_UTF16(ISTR  , STR ) ( lwUSB_RegisterString(ISTR , STR ,LWUSB_STRLEN(STR) , LWUSB_STRINGD_ENCODE_UTF16 ) )
#define LWUSB_REGISTER_STRING_RAW(ISTR ,  STR , LEN ) ( lwUSB_RegisterString(ISTR , STR , LEN , LWUSB_STRINGD_ENCODE_RAW ) )

	static uint8_t debug = 0 ;

//todo remove
extern volatile int32_t usb_state ;
static uint32_t string_indexing  = 0 ;
static uint32_t get               = 0 ;

typedef enum  {

	e_lwUSB_ep_type_bulk      = 0b00 ,
	e_lwUSB_ep_type_control   = 0b01 ,
	e_lwUSB_ep_type_iso       = 0b10 ,
	e_lwUSB_ep_type_interrupt = 0b11
} e_lWUSB_ep_type ;

typedef enum {

	e_lwUSB_ep_dir_inout   = 0u ,
	e_lwUSB_ep_dir_in    = 1u ,
	e_lwUSB_ep_dir_out   = 2u ,

} e_lwUSB_ep_dir ;


typedef enum {

	e_lwUSB_controller_state_Default = 0u ,
	e_lwUSB_controller_state_address = 1u ,
	e_lwUSB_controller_state_configured = 2u ,
	e_lwUSB_controller_state_suspended = 3u ,
	e_lwUSB_controller_state_attached = 4u ,                    //attached but not powered todo


} e_lwUSB_controller_state ;

struct lwUSB_controller_s {

	uint32_t isInitialized ;					  /* Initialization Checker */
	e_lwUSB_controller_state lwUSB_device_state ; /* device state */
	uint8_t device_addr ;						  /* assigned address to the device */
	uint8_t active_configuration ;                /* Index of Current Active Configuration */
	struct lwUSB_setup_data request ;		      /* Pointer to the setup request data */
	uint8_t isrequest ;                           /* is set when a setup transaction is completed and cleared on the end of the status stage */
	struct lwUSB_ep_s * lwUSB_eps[LWUSB_OPTS_NUM_HW_EPS];  /* Array of pointers to EPs data structs  */
	struct lwUSB_configuration_s * configurations ;
	struct lwUSB_string_s *        strings ;
	uint8_t nextConfigIndex    ;
	uint8_t nextInterfaceIndex ;
	uint8_t nextStringIndex    ;

};

static struct lwUSB_transaction_s c_ep_t1 = {
		.current_state  = e_lwUSB_transaction_state_null ,
		.previous_state = e_lwUSB_transaction_state_null ,
		.payload_buffer = NULL ,
		.payload_length = 0u ,
		.payload_ptr    = 0u ,
		.next = NULL ,

};


static struct lwUSB_ep_s control_0 = {

			.EP_isInitialized = 0u ,
			.EP_Type = e_lwUSB_EndPoint_Type_Control ,
			.EP_Direction = e_lwUSB_EndPoint_Direction_INOUT ,
			.EP_Number = 0 ,
			.EP_Address = 0 ,
			.EP_Transaction = &c_ep_t1 ,
			.EP_TxChannel = {	.EP_BufferHandle = NULL ,
								.EP_Size         = LWUSB_OPTS_CONTROL_EP_TX_B_SIZE ,
								.EP_State        = e_lwUSB_EndPoint_State_Disabled
			},
			.EP_RxChannel = {	.EP_BufferHandle = NULL ,
								.EP_Size         = LWUSB_OPTS_CONTROL_EP_RX_B_SIZE ,
								.EP_State        = e_lwUSB_EndPoint_State_Disabled
			}

	};


static struct lwUSB_controller_s lwUSB_controller = {
		.isInitialized = 0u ,
		.lwUSB_device_state = e_lwUSB_controller_state_Default ,
		.configurations = NULL ,
		.active_configuration   = 0U ,
		.strings = NULL ,
		.device_addr = 0u ,
		.isrequest = 0u ,
		.lwUSB_eps              = {NULL} ,
		.nextConfigIndex        = LWUSB_OPTS_CONFIG_VALUE_START  ,
		.nextInterfaceIndex     = LWUSB_OPTS_INTERFACE_VALUE_START ,
		.nextStringIndex        = LWUSB_OPTS_STRING_INDEX_START
};




struct lwUSB_string_s * lwUSB_FindStringbyIndex ( uint8_t stringID ){

	struct lwUSB_string_s * s ;

	for ( s = lwUSB_controller.strings ; s != NULL ; s = s->next ){
		if ( s->sid == stringID ){
			return s ;
		}
	}

	return NULL ;

}
struct lwUSB_string_s *  lwUSB_RegisterString( uint8_t stringID , uint8_t * IString , uint16_t IStringLen , uint8_t Encoding ) {

	if ( !IString || !(IStringLen) ){
		return NULL ;
	}

	if ( lwUSB_FindStringbyIndex(stringID) != NULL ){
		return NULL ;
	}
	int32_t S_alloc_len = 0 ;

	if( Encoding == LWUSB_STRINGD_ENCODE_RAW ){
		 S_alloc_len = sizeof(struct lwUSB_string_descriptor_s)+ IStringLen ;
	}
	else if ( Encoding == LWUSB_STRINGD_ENCODE_UTF16 ){
		 S_alloc_len = sizeof(struct lwUSB_string_descriptor_s)+ IStringLen * 2u ;
	}
	struct lwUSB_string_descriptor_s* d_string =  (struct lwUSB_string_descriptor_s* )malloc(S_alloc_len) ;
	if ( !d_string ){
		return NULL ;
	}
	struct lwUSB_string_s * s = (struct lwUSB_string_s *)malloc(sizeof(struct lwUSB_string_s ));

	if ( !s ){
		free(d_string);
		return NULL ;
	}

	d_string->bLength = S_alloc_len ;
	d_string->bDescriptorType = e_lwUSB_bdescriptor_type_string ;
	uint8_t * sd_string_cast = (((uint8_t*)(d_string))+sizeof(struct lwUSB_string_descriptor_s));

	for ( int32_t i = 0 ;  i < IStringLen ; i++ ){
		if( Encoding == LWUSB_STRINGD_ENCODE_RAW ){
			sd_string_cast[i] = IString[i] ;
		}
		else if ( Encoding == LWUSB_STRINGD_ENCODE_UTF16 ){
			sd_string_cast[i] = (i%2) ? 0u : IString[i] ;
		}
	}


	s->sid = stringID ;
	s->d_string = d_string ;

	s->next = lwUSB_controller.strings ;
	lwUSB_controller.strings = s ;

	return s ;
}

uint32_t lwUSB_Initialize_Endpoint ( struct lwUSB_ep_s * EndPoint );

uint32_t lwUSB_Reset( void )
{

	/* Reset Hardware EndPoint Registers */
	lwUSB_hwRegisterReset();
	/* Reset Packet Buffer Memory */
	lwUSB_hwMemoryReset();

	/* Initialize Control EP */
	lwUSB_controller.isInitialized++;
	lwUSB_controller.lwUSB_eps[0] = &control_0 ;
	/* return to default state */
	lwUSB_controller.lwUSB_device_state = e_lwUSB_controller_state_Default ;

	lwUSB_hwSetAddress(0u);

	lwUSB_Initialize_Endpoint( lwUSB_controller.lwUSB_eps[0u] ) ;

	lwUSB_hwEnable();

	return ERR_OK ;

}

uint32_t lwUSB_Init( void ){

	/* Call Low Level Initialization Routine */
	lwUSB_hwInit();
	/* Set Default Address */
	lwUSB_hwSetAddress(0u);
	/* Disable USB Function Until Reset Request is Issued */
	lwUSB_hwDisable();



	uint16_t langid[] = {LWUSB_OPTS_LANGID} ;
	/* Register Device String Descriptors */
	LWUSB_REGISTER_STRING_RAW( 0u, (uint8_t*)langid , sizeof(langid) );

#if defined(LWUSB_OPTS_MANUFACTURER_STRINGD_EN) && LWUSB_OPTS_MANUFACTURER_STRINGD_EN != 0u
	LWUSB_REGISTER_STRING_UTF16(LWUSB_OPTS_MANUFACTURER_STRINGD_I , LWUSB_OPTS_MANUFACTURER_STRING );
#endif
#if defined(LWUSB_OPTS_PRODUCT_STRINGD_EN) && LWUSB_OPTS_PRODUCT_STRINGD_EN != 0u
	LWUSB_REGISTER_STRING_UTF16( LWUSB_OPTS_PRODUCT_STRINGD_I , LWUSB_OPTS_PRODUCT_STRING );
#endif
#if defined(LWUSB_OPTS_SERIAL_NUMBER_STRINGD_EN) && LWUSB_OPTS_SERIAL_NUMBER_STRINGD_EN != 0u
	LWUSB_REGISTER_STRING_UTF16( LWUSB_OPTS_SERIAL_NUMBER_STRINGD_I , LWUSB_OPTS_SERIAL_NUMBER_STRING );
#endif
	uint8_t msft[] = { 'M' , 'S' , 'F' , 'T' , '1' , '0' , '0'  , LWUSB_OPTS_VENDOR_CODE , 0 } ;
	LWUSB_REGISTER_STRING_UTF16( 0xee , msft  );


	//manually allocate for a transaction  todo

	return ERR_OK ;

}

//todo make err Generic to the stack
uint32_t lwUSB_LoadPacket( struct lwUSB_ep_s * EndPoint  , uint8_t * DataPtr , uint32_t Length  )
{

	if ( !EndPoint || !EndPoint->EP_isInitialized ){
		return ERR_NULL ;
	}

	if ( EndPoint->EP_TxChannel.EP_Size <  Length                  ||
		 !(EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_IN    ||
		   EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_INOUT ) ){
		return ERR_FAULT ;
	}

	return  lwUSB_hwWriteData( EndPoint->EP_Number , DataPtr , Length ) ;

}


static inline uint32_t lwUSB_SetAddress ( uint8_t  addr )
{
	lwUSB_controller.device_addr = addr ;
	lwUSB_controller.lwUSB_device_state = e_lwUSB_controller_state_address ;
	lwUSB_hwSetAddress(addr);
	return ERR_OK ;
}
static  uint32_t lwUSB_Protocol_Stall ( struct lwUSB_ep_s * EndPoint )
{
	if ( EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_IN ){
		lwUSB_hwSetTXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Stall);
	}
	else if ( EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_OUT ){
		lwUSB_hwSetRXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Stall);
	}
	else if ( EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_INOUT ){
		lwUSB_hwSetTXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Stall);
		lwUSB_hwSetRXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Stall);
	}
	return ERR_OK ;
}


static  uint32_t lwUSB_DisableChannels ( struct lwUSB_ep_s * EndPoint )
{

	lwUSB_hwSetTXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Disabled);
	lwUSB_hwSetRXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Disabled);

	return ERR_OK ;
}


static uint32_t zlp = 0xa5a5a5a5;




uint32_t lwUSB_Set_Transaction_State ( struct lwUSB_ep_s * EndPoint , e_lwUSB_transaction_state State  )
{
	EndPoint->EP_Transaction->current_state = State ;
	return ERR_OK ;
}

/* This Function Registers an IN Transaction on the EP */
uint32_t lwUSB_Transfer_IN(struct lwUSB_ep_s * EndPoint ,  uint8_t * DataPtr , uint32_t Length  )
{
	if ( !EndPoint || !DataPtr || !EndPoint->EP_isInitialized ){
		return ERR_NULL ;
	}
	/* Make Sure EP is Initialized , and has an IN channel */
	if ( !(EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_IN ||
		   EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_INOUT ) ){
		return ERR_FAULT ;
	}

	if (!EndPoint->EP_Transaction  ||
		 EndPoint->EP_Transaction->current_state == e_lwUSB_transaction_state_null ){
		return ERR_FAULT ;
	}

	if ( EndPoint->EP_TxChannel.EP_Size >= Length ){
		/* Whole payload in one transaction */
		lwUSB_LoadPacket( EndPoint , DataPtr , Length );
		EndPoint->EP_Transaction->payload_ptr = Length ;
	}
	else{
		lwUSB_LoadPacket( EndPoint , DataPtr , EndPoint->EP_TxChannel.EP_Size );
		EndPoint->EP_Transaction->payload_ptr = EndPoint->EP_TxChannel.EP_Size ;
	}

	EndPoint->EP_Transaction->payload_buffer = DataPtr ;
	EndPoint->EP_Transaction->payload_length = Length ;


	lwUSB_hwSetTXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Valid);
	if ( EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_INOUT ){
		lwUSB_hwSetRXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Stall);
	}
	EndPoint->EP_Transaction->previous_state = EndPoint->EP_Transaction->current_state ;
	EndPoint->EP_Transaction->current_state  = e_lwUSB_transaction_state_data_in ;

	return ERR_OK ;


}



int32_t lwUSB_Transfer_OUT(struct lwUSB_ep_s * EndPoint ,  uint8_t * DataPtr  )
{
	if ( !EndPoint || !DataPtr || !EndPoint->EP_isInitialized ){
		return ERR_NULL ;
	}
	/* Make Sure EP is Initialized , and has an IN channel */
	if ( !(EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_OUT ||
		   EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_INOUT ) ){
		return ERR_FAULT ;
	}

	if (!EndPoint->EP_Transaction  ||
		 EndPoint->EP_Transaction->current_state == e_lwUSB_transaction_state_null ){
		return ERR_FAULT ;
	}

	//todo handle OUT Transaction
	return ERR_OK ;


}



static uint32_t lwUSB_Copy_Configuration ( uint8_t * dest , struct lwUSB_configuration_s * config )
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

static uint32_t lwUSB_GetEndPointByAddress( uint8_t EPn ){

	for ( int32_t i = 0 ; i < LWUSB_OPTS_NUM_HW_EPS ; i++ ){
		if ( lwUSB_controller.lwUSB_eps[i] && lwUSB_controller.lwUSB_eps[i]->EP_isInitialized ){
			if ( lwUSB_controller.lwUSB_eps[i]->EP_Address ==  EPn ){
				return i ;
			}
		}
	}

	return ERR_FAULT ;
}
uint32_t lwUSB_Handle_CTR ( uint8_t EPn ){

	int32_t state = lwUSB_GetEndPointByAddress( EPn );
	/* Doesn't Exist or Invalid EP Num */
	if ( state < 0 || state > LWUSB_OPTS_NUM_HW_EPS-1 ){
		return ERR_FAULT ;
	}
	uint8_t EndPointNumber = (uint8_t)state ;
	//todo remove
	volatile uint32_t unused ;
	LW_ASSERT( EndPointNumber <= LWUSB_OPTS_NUM_HW_EPS-1 );
	LW_ASSERT( lwUSB_controller.isInitialized != 0u );
	LW_ASSERT( lwUSB_controller.lwUSB_eps[EndPointNumber] != NULL );
	LW_ASSERT( lwUSB_controller.lwUSB_eps[EndPointNumber]->EP_isInitialized != 0u );

	struct lwUSB_ep_s * EndPoint = lwUSB_controller.lwUSB_eps[EndPointNumber] ;

	if( !EndPoint || EndPoint->EP_isInitialized == 0u ){

		lwUSB_hwSetTXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Disabled);
		lwUSB_hwSetRXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Disabled);
		return ERR_FAULT ;
	}



	uint32_t setup_completed  = lwUSB_hwIsSetupFlag(EndPointNumber);
	uint32_t out_completed    = lwUSB_hwIsOUTFlag(EndPointNumber);
	uint32_t in_completed     = lwUSB_hwIsINFlag(EndPointNumber);

	static int setup_count = 0 ;
	if ( setup_completed ){
		setup_count++ ;
		lwUSB_hwClearSetupFlag(EndPointNumber);

		if ( setup_count == 2 ){
			debug++;

		}
		uint16_t request_length = lwUSB_hwReadData( EndPoint->EP_Number , (uint8_t*)&lwUSB_controller.request  );
		LW_ASSERT( request_length == sizeof(struct lwUSB_setup_data));

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
					lwUSB_Transfer_IN(EndPoint, (void*)(&zlp) , 0u);
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

					lwUSB_Set_Transaction_State( EndPoint , e_lwUSB_transaction_state_setup_stage );
					if ( request->descriptorType == e_lwUSB_bdescriptor_type_device ){
						if( lwUSB_controller.lwUSB_device_state == e_lwUSB_controller_state_address ){
							debug++;
						}
						lwUSB_Transfer_IN( EndPoint , (uint8_t*)&lwUSB_Device_Descriptor , sizeof( struct lwUSB_device_descriptor_s) ) ;
					}
					else if ( request->descriptorType == e_lwUSB_bdescriptor_type_configuration ){
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
							lwUSB_Protocol_Stall(EndPoint);
						}
						else{
							uint8_t cbuff[p->d_configuration->wTotalLength];
							lwUSB_Copy_Configuration(cbuff, p);
							/* Send Configuration Descriptor */
							lwUSB_Transfer_IN( EndPoint , cbuff , p->d_configuration->wTotalLength  ) ;

						}
					}
					else if (  request->descriptorType == e_lwUSB_bdescriptor_type_string ){

						string_indexing++;

						struct lwUSB_string_s * s = lwUSB_FindStringbyIndex(request->descriptorIndex);
						if ( !s ){
							lwUSB_Protocol_Stall(EndPoint);
							break ;
						}
						lwUSB_Transfer_IN( EndPoint , (uint8_t*)s->d_string , s->d_string->bLength  ) ;

					}
					else if ( request->descriptorType == e_lwUSB_bdescriptor_type_device_qualifier )
					{
						debug++;
						debug++;
					}
					else{

						lwUSB_Protocol_Stall(EndPoint);
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
		lwUSB_hwClearOUTFlag(EndPointNumber);


		static int out_twice = 0 ;

		out_twice++;

		if ( out_twice == 2 ){
			debug++;
		}

		if( EndPoint->EP_Type == e_lwUSB_EndPoint_Type_Control ){

			/* Status received for now todo */
			lwUSB_controller.isrequest = 0u ;
			lwUSB_Protocol_Stall(EndPoint);

		}

	}
	else if ( in_completed ){
		lwUSB_hwClearINFlag(EndPointNumber);

		static int in_twice = 0 ;

		in_twice++ ;

		if ( in_twice == 3 ){
			debug++;
		}
		if( lwUSB_controller.lwUSB_device_state == e_lwUSB_controller_state_address ){
			debug++;
		}

		if ( EndPoint->EP_Transaction->payload_ptr < EndPoint->EP_Transaction->payload_length  ){
			/* We need to load another packet , since more stages are to come  */
			lwUSB_Transfer_IN(EndPoint, &EndPoint->EP_Transaction->payload_buffer[EndPoint->EP_Transaction->payload_ptr] , EndPoint->EP_Transaction->payload_length - EndPoint->EP_Transaction->payload_ptr );
		}
		else {
			/* IN Completed */
			if( EndPoint->EP_Type == e_lwUSB_EndPoint_Type_Control )
			{
			//	if ( curr_ep->t_in->previous_state == e_lwUSB_transaction_state_setup_stage ){
					//assume that all control ep transactions came from setup stage 183
				 	// we must now receive the ACK in the Status Stage
				if (  lwUSB_controller.isrequest && lwUSB_controller.request.bRequest == e_lwUSB_setup_request_set_address ){
					/* ZLP Handle , means an ACK in the Status Stage */
					if ( EndPoint->EP_Transaction->payload_length == 0u ) {
						lwUSB_SetAddress(lwUSB_controller.device_addr);
						lwUSB_controller.lwUSB_device_state = e_lwUSB_controller_state_address ;
						lwUSB_Protocol_Stall(EndPoint);
						lwUSB_controller.isrequest = 0u ;


					}
					else{
						/* NON ZLP Status Stage */
						lwUSB_Protocol_Stall(EndPoint);
					}

				}
				else{

					lwUSB_hwSetTXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Stall);
					lwUSB_hwSetRXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Valid);

				}

		//		}

			}
		}
	}
	else{
		//todo error
		lwUSB_Protocol_Stall(EndPoint);
		LW_DEBUG("CTR Flag Raised with no source ");
		return ERR_FAULT ;
	}



	return ERR_OK ;
}


//todo moved to hw interface only 2 byte sized blocks are used make it work with both 32 and 2
//todo this function needs a visist with regards to where it fits in the stack
uint32_t lwUSB_Initialize_Endpoint ( struct lwUSB_ep_s * EndPoint ){
	LW_ASSERT( EndPoint != NULL ) ;
	LW_ASSERT( EndPoint->EP_Number <= LWUSB_OPTS_NUM_HW_EPS-1 );
	LW_ASSERT( EndPoint->EP_Type <= LWUSB_EP_TYPE_MAX );

	/* Disable Both Channels */
	lwUSB_DisableChannels(EndPoint);

	/* Configure */
	lwUSB_hwConfigureEndPoint(EndPoint->EP_Number,
			                  EndPoint->EP_Address,
							  EndPoint->EP_Type,
						      EndPoint->EP_Direction );

	/* Allocate */
	if ( EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_IN ||
		 EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_INOUT ){
		EndPoint->EP_TxChannel.EP_BufferHandle = (uint8_t*) lwUSB_hwAllocate( EndPoint->EP_Number , EndPoint->EP_TxChannel.EP_Size , EndPoint->EP_Type , e_lwUSB_EndPoint_Direction_IN );
		if ( EndPoint->EP_TxChannel.EP_BufferHandle == NULL ){
			LW_DEBUG("malloc out of memory : EP_TXCHANNEL \r\n");
			return ERR_NULL ;
		}
		lwUSB_hwSetTXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Stall);

	}
	if ( EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_OUT ||
		 EndPoint->EP_Direction == e_lwUSB_EndPoint_Direction_INOUT ){
		EndPoint->EP_RxChannel.EP_BufferHandle  = (uint8_t*) lwUSB_hwAllocate( EndPoint->EP_Number , EndPoint->EP_RxChannel.EP_Size ,  EndPoint->EP_Type , e_lwUSB_EndPoint_Direction_OUT  );
		if ( EndPoint->EP_RxChannel.EP_BufferHandle == NULL ){
			LW_DEBUG("malloc out of memory : EP_RXCHANNEL \r\n");
			return ERR_NULL ;
		}
		lwUSB_hwSetRXResponse( EndPoint->EP_Number , e_lwUSB_EndPoint_State_Stall);
	}

	EndPoint->EP_isInitialized++;

	return ERR_OK ;

}

struct lwUSB_endpoint_s * lwUSB_CreateEndpoint( uint16_t maxPacketSize  , uint8_t epType , uint8_t epDir  , uint16_t pollTms ) {

	if ( epType > LWUSB_EP_TYPE_MAX || epDir > LWUSB_EP_DIRECTION_MAX ){
		return NULL ;
	}

	if ( !pollTms || ( epType == e_lwUSB_EndPoint_Type_Isochronous && pollTms > LWUSB_ISOCHRONOUS_MAX_PERIOD_MS ) ||
					 ( epType != e_lwUSB_EndPoint_Type_Isochronous && pollTms > LWUSB_ENDPOINT_MAX_PERIOD_MS )){
		return NULL ;
	}

	if ( epType == e_lwUSB_EndPoint_Type_Isochronous ){
		while ( pollTms > 16u ){
			pollTms >>= 1u ;
		}
	}

	struct lwUSB_endpoint_s * endpoint = (struct lwUSB_endpoint_s * )malloc(sizeof(struct lwUSB_endpoint_s ));
	if ( !endpoint ){
		return NULL ;
	}
	struct lwUSB_endpoint_descriptor_s * d_endpoint = (struct lwUSB_endpoint_descriptor_s* )malloc(sizeof(struct lwUSB_endpoint_descriptor_s));
	if ( !d_endpoint ){
		free(endpoint);
		return NULL ;
	}


	d_endpoint->bLength = 7u ;
	d_endpoint->bDescriptorType = e_lwUSB_bdescriptor_type_endpoint ;
	d_endpoint->bInterval =  pollTms ;
	d_endpoint->Direction = epDir ;
	d_endpoint->bEndpointAddress = 0u ;
	d_endpoint->MaxPacketSize = maxPacketSize ;
	d_endpoint->Transfer_Type = epType ;
	d_endpoint->Additional_Transactions_PerMicroFrame = 0u ;
	/* Note Synchronization in case of Isochronous Endpoints are hardcoded to No Synchronization and Usage to Data Endpoint */
	d_endpoint->Synchronization_Type = 0u ;
	d_endpoint->Usage_Type = 0u ;

	endpoint->d_endpoint = d_endpoint ;
	return endpoint ;
}

struct lwUSB_interface_s * lwUSB_CreateInterface ( uint8_t * IString ) {

	struct lwUSB_interface_s * interface = (struct lwUSB_interface_s * )malloc(sizeof(struct lwUSB_interface_s ));
	if ( !interface ){
		return NULL ;
	}
	struct lwUSB_interface_descriptor_s * d_interface = (struct lwUSB_interface_descriptor_s* )malloc(sizeof(struct lwUSB_interface_descriptor_s));
	if( !d_interface ){
		free(interface);
		return NULL ;
	}

	uint8_t StringIndex = 0u  ;
	if ( IString ){
		StringIndex = lwUSB_controller.nextStringIndex++ ;
		interface->string = LWUSB_REGISTER_STRING_UTF16( StringIndex , IString );
		if ( !interface->string ){
			LW_DEBUG("String NULL for lwUSB_CreateInterface with S= %s \r\n" , IString );
		}
	}

	d_interface->bLength            = 9u ;
	d_interface->bDescriptorType    = e_lwUSB_bdescriptor_type_interface ;
	d_interface->bInterfaceNumber   = lwUSB_controller.nextInterfaceIndex++ ;
	d_interface->bAlternateSetting  = 0u ;
	/* todo Add Class Support */
	d_interface->bInterfaceClass    = 0u ;
	d_interface->bInterfaceSubclass = 0u ;
	d_interface->bInterfaceProtocol = 0u ;
	d_interface->bNumEndpoints      = 0u ;
	d_interface->iInterface         = StringIndex ;


	interface->d_interface = d_interface ;
	return interface ;
}

struct lwUSB_configuration_s *  lwUSB_CreateConfiguration ( uint8_t * IString ){

	struct lwUSB_configuration_s * config = (struct lwUSB_configuration_s * )malloc(sizeof(struct lwUSB_configuration_s ));
	if ( !config ){
		return NULL ;
	}
	struct lwUSB_configuration_descriptor_s * d_config = (struct lwUSB_configuration_descriptor_s* )malloc(sizeof(struct lwUSB_configuration_descriptor_s));
	if( !d_config ){
		free(config);
		return NULL ;
	}

	uint8_t StringIndex = 0u  ;
	if ( IString ){
		StringIndex = lwUSB_controller.nextStringIndex++ ;
		config->string = LWUSB_REGISTER_STRING_UTF16( StringIndex , IString );

		if ( !config->string ){
			LW_DEBUG("String NULL for lwUSB_CreateConfiguration with S= %s \r\n" , IString );
		}
	}


	d_config->bLength             = 9u ;
	d_config->wTotalLength        = 0u ;
	d_config->bNumInterfaces      = 0u ;
	d_config->bDescriptorType     = e_lwUSB_bdescriptor_type_configuration ;
	d_config->bConfigurationValue = lwUSB_controller.nextConfigIndex++ ;
	d_config->iConfiguration      = StringIndex ;


	config->d_configuration = d_config ;
	return config ;

}



uint32_t lwUSB_RegisterEndpoint ( struct lwUSB_endpoint_s * endpoint , struct lwUSB_interface_s * interface ){
	/* Null Check */
	if ( !endpoint || !interface ){
		return ERR_NULL ;
	}
	/* EP Address is 4 bits which gives a max of 16 Endpoints , this number is further limited by Number of Actual HW endpoints */
	if ( interface->d_interface->bNumEndpoints > LWUSB_OPTS_NUM_HW_EPS-1 || interface->d_interface->bNumEndpoints >= 0x0F ){
		return ERR_FAULT ;
	}

	if ( interface->endpoint != NULL ){
		if ( interface->endpoint->d_endpoint->Endpoint_number >= LWUSB_OPTS_NUM_HW_EPS-1 ){
			return ERR_FAULT ;
		}
		/* Assign another EndPoint Number */
		endpoint->d_endpoint->Endpoint_number = interface->endpoint->d_endpoint->Endpoint_number + 1u ;
	}
	else{
		/* Interface has no Endpoints so assign the first */
		endpoint->d_endpoint->Endpoint_number = 1u ;
	}

	interface->d_interface->bNumEndpoints++ ;
	/* Register EP to the interface */
	endpoint->next = interface->endpoint ;
	interface->endpoint = endpoint ;

	return ERR_OK ;

}

uint32_t lwUSB_RegisterInterface ( struct lwUSB_interface_s * interface , struct lwUSB_configuration_s * config ){
	/* Null Check */
	if ( !interface || !config ){
		return ERR_NULL ;
	}
	/* Look for an Interface with the same index , so we don't register twice in the same configuration */
	for ( struct lwUSB_interface_s *  temp_intf = config->interface ; temp_intf != NULL ; temp_intf = temp_intf->next ){
		if ( temp_intf->d_interface->bInterfaceNumber == interface->d_interface->bInterfaceNumber ){
			return ERR_FAULT ;
		}
	}

	config->d_configuration->bNumInterfaces++ ;
	/* Register EP to the interface */
	interface->next = config->interface ;
	config->interface = interface ;
	return ERR_OK ;


}

uint32_t lwUSB_RegisterConfiguration ( struct lwUSB_configuration_s * config ){
	/* Null Check */
	if ( !config || !config->interface || !config->d_configuration ){
		return ERR_NULL ;
	}
	/* Check for Config Value Uniqueness */
	for ( struct lwUSB_configuration_s *  temp_config = lwUSB_controller.configurations ; temp_config != NULL ; temp_config = temp_config->next ){
		if ( temp_config->d_configuration->bConfigurationValue == config->d_configuration->bConfigurationValue ){
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

	/* Register Configuration */
	config->next = lwUSB_controller.configurations ;
	lwUSB_controller.configurations = config ;

	return ERR_OK ;

}
