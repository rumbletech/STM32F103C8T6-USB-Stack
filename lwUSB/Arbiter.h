/*
 * Arbiter.h
 *
 *  Created on: Jul 20, 2023
 *      Author: Garmoosh
 */

#ifndef ARBITER_H_
#define ARBITER_H_

#include "Include/lwUSB_Descriptors.h"
#include "DataPool/DataPool.h"



typedef enum {

	e_lwUSB_transaction_state_null          = 0u ,
	e_lwUSB_transaction_state_setup_stage   = 1u ,
	e_lwUSB_transaction_state_status_stage =  2u ,
	e_lwUSB_transaction_state_data_in       = 3u ,
	e_lwUSB_transaction_state_data_out      = 4u ,

} e_lwUSB_transaction_state ;


typedef enum {

	e_lwUSB_EndPoint_Type_Control     = 0u  ,
	e_lwUSB_EndPoint_Type_Isochronous = 1u  ,
	e_lwUSB_EndPoint_Type_Bulk        = 2u  ,
	e_lwUSB_EndPoint_Type_Interrupt   = 3u  ,
	e_lwUSB_EndPoint_Type_MAX

} e_lwUSB_EndPoint_Type ;


typedef enum {


	e_lwUSB_EndPoint_Direction_OUT     = 0u  ,
	e_lwUSB_EndPoint_Direction_IN      = 1u  ,
	e_lwUSB_EndPoint_Direction_INOUT   = 2u  ,
	e_lwUSB_EndPoint_Direction_MAX


} e_lwUSB_EndPoint_Direction ;


typedef enum {

	e_lwUSB_EndPoint_State_Disabled  = 0b00 ,
	e_lwUSB_EndPoint_State_Stall     = 0b01 ,
	e_lwUSB_EndPoint_State_Nak       = 0b10 ,
	e_lwUSB_EndPoint_State_Valid     = 0b11 ,

} e_lwUSB_EndPoint_State ;


/* Descripes a String Descriptor */

struct lwUSB_string_s {

	struct lwUSB_string_descriptor_s * d_string ;
	uint8_t sid ;
	struct lwUSB_string_s * next ;
};

/* This describes the physical aspect of the EndPoint ,
 * it Contains :
 * Handles to its Allocated Buffers.
 * EndPoint Hadrware Identification Number. ( this is recognized by HAL Level ).
 */

struct lwUSB_PhyEndPoint_s {

	uint8_t ep_n;              /* This is the link between Hardware EndPoint and a FW EndPoint */
    struct DataPool_s * dp;    /* Handle to the Internal Allocated Memory for the EndPoint */
    struct DataPool_s * dp_r;  /* another Handle in case of INOUT EndPoints */

};
/* Note : any EndPoint that shall be created , Shall have
 * handle to EndPoint Information ( Direction , Adress , Type ) , as well as specific info about each type.
 * handle to the EndPoint Base , this is understood by HAL
 */

struct lwUSB_EndPoint_s {

	struct lwUSB_interface_s * p_itf;          /* Link to Parent Interface */
	struct lwUSB_endpoint_descriptor_s * ep_d; /* Referes to the meta Info about thie EP */
	struct lwUSB_PhyEndPoint_s * ep_phy;       /* Refers to the physical EP , that this EP is based on. */
};


/* This Defines a USB Interface , and  it is fully represented by
 * Interface Descriptor.
 * Optional String Descriptor.
 * Child EndPoints.
 * Parent Configuration.
 */
struct lwUSB_interface_s {

	struct lwUSB_interface_descriptor_s * itf_d;   /* Interface Descriptor */
	struct lwUSB_string_s * itf_sd;                /* Interface String Descriptor */
	struct ll_s * itf_c;                           /* Wrapped Child EndPoints */
	struct lwUSB_configuration_s * itf_pcfg ;      /* Link to Parent Configuration */

};

/* This Defines a USB Configuration , and  it is fully represented by
 * Configuration Descriptor.
 * Optional String Descriptor.
 * Child Interfaces.
 */

struct lwUSB_configuration_s {

	struct lwUSB_configuration_descriptor_s * cfg_d  ; /* Configuration Descriptor */
	struct ll_s * cfg_c ;                              /* Wrapped Child Interfaces */
	struct lwUSB_string_s * cfg_sd ;                   /* Configuration String Descriptor */

};


struct Event_s * getEventbyID (uint32_t evid);
#endif /* ARBITER_H_ */
