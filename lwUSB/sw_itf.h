/*
 * sw_itf.h
 *
 *  Created on: Dec 24, 2023
 *      Author: Garmoosh
 */

#ifndef SW_ITF_H_
#define SW_ITF_H_

#include "Include/lwUSB_Std.h"
#include "tree.h"

struct sw_state_s {

	uint8_t init_d   : 1;
	uint8_t start_d  : 1;
	uint8_t NC2      : 1;
	uint8_t NC3      : 1;
	uint8_t NC4      : 1;
	uint8_t NC5      : 1;
	uint8_t NC6      : 1;
	uint8_t NC7      : 1;
};


void sw_itf_SysStart( void );
void sw_itf_SysInit( void );
struct sw_state_s sw_itf_GetStatus( void );
Handle sw_itf_CreateEndpointHandle( struct EndPoint_Inf_s * info ) ;
Handle sw_itf_CreateStringHandle( struct String_Inf_s * info );
Handle sw_itf_CreateInterfaceHandle ( struct Interface_Inf_s * info );
Handle sw_itf_CreateConfigurationHandle ( struct Configuration_Inf_s * info );
Handle sw_itf_CreatePhyHandle( struct Phy_Inf_s * info );
lwUSB_Err sw_itf_RegisterEndPoint ( Handle ep , Handle itf );
lwUSB_Err sw_itf_RegisterInterface ( Handle itf , Handle cfg );
lwUSB_Err sw_itf_RegisterConfiguration( Handle cfg );
lwUSB_Err sw_itf_CalculateDescriptors ( void );

#endif /* SW_ITF_H_ */
