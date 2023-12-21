/*
 * tree.h
 *
 *  Created on: Sep 3, 2023
 *      Author: mrashada
 */

#include "Include/lwUSB_Debug.h"
#include "Include/lwUSB_Std.h"


typedef enum {
	e_handle_type_invalid  = 0x00,
	e_handle_type_endpoint = 0x01,
	e_handle_type_phy_endpoint = 0x02,
	e_handle_type_interface = 0x03,
	e_handle_type_configuration = 0x04,
	e_handle_type_string = 0x05,
	e_handle_type_device = 0x06
} htype ;

typedef struct {
	htype ht;
	void * hc;
}Handle;

typedef struct Configuration_Inf_s {

	uint8_t cfg_value;
	uint8_t cfg_maxPower;
	boolean cfg_selfPowered;
	boolean cfg_remoteWakeUp;

}Configuration_Inf ;

typedef struct Interface_Inf_s {

	uint8_t itf_class;
	uint8_t itf_subClass;
	uint8_t itf_protocol;
	uint8_t itf_number;
	uint8_t itf_numAlternateSettings;

}Interface_Inf ;

typedef struct String_Inf_s {

	uint8_t * str_content;
	uint8_t str_id;
	uint8_t str_encoding;
	uint8_t str_length;

}String_Inf ;

typedef struct EndPoint_Inf_s {

	Handle phyHandle;
	uint8_t epAddress;
	e_lwUSB_EndPoint_Type epType;
	e_lwUSB_EndPoint_Direction epDir;
	uint16_t maxPacketSize;
	uint16_t pollTms;

}EndPoint_Inf;

typedef struct Phy_Inf_s {

	boolean  phy_isDouble;
	uint16_t phy_buffSize;
	uint8_t  phy_EpNumber;

}Phy_Inf;


Handle tree_CreateEndpointHandle( struct EndPoint_Inf_s * info ) ;
Handle tree_CreateStringHandle( struct String_Inf_s * info );
Handle tree_CreateInterfaceHandle ( struct Interface_Inf_s * info );
Handle tree_CreateConfigurationHandle ( struct Configuration_Inf_s * info );
Handle tree_CreatePhyHandle( struct Phy_Inf_s * info );
lwUSB_Err tree_RegisterEndPoint ( Handle ep , Handle itf );
lwUSB_Err tree_RegisterInterface ( Handle itf , Handle cfg );
lwUSB_Err tree_RegisterConfiguration( Handle cfg );
