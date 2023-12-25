/* Author : mrashada
 * Date   : 7/14/2023
 * ***********************************************************************
 * ***********************************************************************
 * This defines the Interface Between Stack Logic and User API's
 * ***********************************************************************
 * ***********************************************************************
 */

#include "sw_itf.h"
#include "tree.h"
#include "memctrl.h"
#include "mem.h"
#include "arbiter.h"

static struct sw_state_s state;

static void sw_itf_Init( void ){

	state.init_d  = 0u;
	state.start_d = 0u;
	state.NC2     = 0u;
	state.NC3     = 0u;
	state.NC4     = 0u;
	state.NC5     = 0u;
	state.NC6     = 0u;
	state.NC7     = 0u;

	return;

}

void sw_itf_SysInit( void ){

	sw_itf_Init();
	memctrl_init();
	tree_Init();
	Arbiter_Init();

	state.init_d = 1u;
	return;
}

void sw_itf_SysStart( void ){

	//Call for Verification of Tree Structures and do necessary Calculations
	sw_itf_CalculateDescriptors();
	state.start_d = 1u;
	return;
}


struct sw_state_s sw_itf_GetStatus( void ){
	return state;
}

Handle sw_itf_CreateConfigurationHandle ( struct Configuration_Inf_s * info ){

	return tree_CreateConfigurationHandle(info);

}

Handle sw_itf_CreateInterfaceHandle ( struct Interface_Inf_s * info ) {

	return tree_CreateInterfaceHandle(info);
}

Handle sw_itf_CreateStringHandle( struct String_Inf_s * info ){

	Handle H;

	H.hc = NULL;
	H.ht = e_handle_type_invalid;

	LW_ASSERT(info->str_content);
	LW_ASSERT(info->str_id != 0u);
	LW_ASSERT(info->str_id != 1u);
	LW_ASSERT(info->str_id != 2u);
	LW_ASSERT(info->str_id != 3u);
	LW_ASSERT(info->str_length != 0u);
	LW_ASSERT(info->str_encoding > lwUSB_String_Encoding_e_Start);
	LW_ASSERT(info->str_encoding < lwUSB_String_Encoding_e_End);

	LW_ASSERT_RETURN(info->str_content,H);
	LW_ASSERT_RETURN(info->str_id != 0u,H);
	LW_ASSERT_RETURN(info->str_id != 1u,H);
	LW_ASSERT_RETURN(info->str_id != 2u,H);
	LW_ASSERT_RETURN(info->str_id != 3u,H);
	LW_ASSERT_RETURN(info->str_length != 0u,H);
	LW_ASSERT_RETURN(info->str_encoding > lwUSB_String_Encoding_e_Start,H);
	LW_ASSERT_RETURN(info->str_encoding < lwUSB_String_Encoding_e_End,H);

	return tree_CreateStringHandle(info);
}

Handle sw_itf_CreateEndpointHandle( struct EndPoint_Inf_s * info ) {

	Handle H;

	H.hc = NULL;
	H.ht = e_handle_type_invalid;

	LW_ASSERT( info->epType < e_lwUSB_EndPoint_Type_MAX );
	LW_ASSERT( info->epDir < e_lwUSB_EndPoint_Direction_MAX );
	LW_ASSERT( info->epAddress <= 15u );
	LW_ASSERT( info->maxPacketSize <= 1024 );
	LW_ASSERT( info->phyHandle.ht == e_handle_type_phy_endpoint );
	LW_ASSERT( info->phyHandle.hc != NULL );

	LW_ASSERT_RETURN( info->epType < e_lwUSB_EndPoint_Type_MAX , H );
	LW_ASSERT_RETURN( info->epDir < e_lwUSB_EndPoint_Direction_MAX , H );
	LW_ASSERT_RETURN( info->epAddress <= 15u , H );
	LW_ASSERT_RETURN( info->maxPacketSize <= 1024 , H );
	LW_ASSERT_RETURN( info->phyHandle.ht == e_handle_type_phy_endpoint , H);
	LW_ASSERT_RETURN( info->phyHandle.hc != NULL , H);

	return tree_CreateEndpointHandle(info);
}

Handle sw_itf_CreatePhyHandle( struct Phy_Inf_s * info ){

	Handle H;

	H.hc = NULL;
	H.ht = e_handle_type_invalid;


	LW_ASSERT(info->phy_buffSize);
	LW_ASSERT_RETURN(info->phy_buffSize,H);

	return tree_CreatePhyHandle(info);
}

lwUSB_Err sw_itf_RegisterEndPoint ( Handle ep , Handle itf ){

	LW_ASSERT(ep.ht == e_handle_type_endpoint);
	LW_ASSERT(ep.hc != NULL);

	LW_ASSERT(itf.ht == e_handle_type_interface);
	LW_ASSERT(itf.hc != NULL);

	LW_ASSERT_RETURN(ep.ht == e_handle_type_endpoint,LWUSB_ERR_PARAM);
	LW_ASSERT_RETURN(ep.hc != NULL,LWUSB_ERR_PARAM);

	LW_ASSERT_RETURN(itf.ht == e_handle_type_interface,LWUSB_ERR_PARAM);
	LW_ASSERT_RETURN(itf.hc != NULL,LWUSB_ERR_PARAM);

	return tree_registerChild(ep,itf) ;
}

lwUSB_Err sw_itf_RegisterInterface ( Handle itf , Handle cfg ){

	LW_ASSERT(itf.ht == e_handle_type_interface);
	LW_ASSERT(itf.hc != NULL);

	LW_ASSERT(cfg.ht == e_handle_type_configuration);
	LW_ASSERT(cfg.hc != NULL);

	LW_ASSERT_RETURN(itf.ht == e_handle_type_interface,LWUSB_ERR_PARAM);
	LW_ASSERT_RETURN(itf.hc != NULL,LWUSB_ERR_PARAM);

	LW_ASSERT_RETURN(cfg.ht == e_handle_type_configuration,LWUSB_ERR_PARAM);
	LW_ASSERT_RETURN(cfg.hc != NULL,LWUSB_ERR_PARAM);

	return tree_registerChild(itf,cfg) ;
}

lwUSB_Err sw_itf_RegisterConfiguration( Handle cfg ){

	LW_ASSERT(cfg.ht == e_handle_type_configuration);
	LW_ASSERT(cfg.hc != NULL);

	LW_ASSERT_RETURN(cfg.ht == e_handle_type_configuration,LWUSB_ERR_PARAM);
	LW_ASSERT_RETURN(cfg.hc != NULL,LWUSB_ERR_PARAM);

	return tree_registerChild(cfg,tree_getDev()) ;
}

