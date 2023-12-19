/*
 * tree.c
 *
 *  Created on: Sep 3, 2023
 *      Author: mrashada
 */

#include "tree.h"

#define NULL_HANDLE (uint64_t)0u

static inline Handle createHandle( htype t , void* c ){
	Handle h;
	h.ht = t;
	h.hc = c;
	return h;
}

static inline void AssertHandle( Handle h , htype t){
	LW_ASSERT(h.ht == t );
	LW_ASSERT(h.hc != NULL);
}

Handle createConfigurationHandle ( struct Configuration_Inf_s * info ){

	uint32_t tvs = sizeof(struct lwUSB_configuration_s);
	uint32_t tds = sizeof( struct lwUSB_configuration_descriptor_s);

	if ( VAR_PEEK(tvs)  == FALSE ||
		 DESC_PEEK(tds) == FALSE ){
		return NULL_HANDLE;
	}

	struct lwUSB_configuration_s * cfg = (struct lwUSB_configuration_s * )VAR_ALLOC(sizeof(struct lwUSB_configuration_s ));
	struct lwUSB_configuration_descriptor_s * cfgd = (struct lwUSB_configuration_descriptor_s*)DESC_ALLOC(sizeof(struct lwUSB_configuration_descriptor_s));

	cfgd->selfPowered = info->cfg_selfPowered;
	cfgd->remoteWakeup = info->cfg_remoteWakeUp;
	cfgd->bConfigurationValue = info->cfg_value;
	cfgd->bMaxPower = info->cfg_maxPower;

	cfgd->bLength = sizeof(lwUSB_configuration_descriptor_s);
	cfgd->bDescriptorType = 0x02;
	cfgd->wTotalLength = cfgd->bLength;
	cfgd->bNumInterfaces = 0u;
	cfgd->iConfiguration = 0u;
	cfgd->reserved = 0u;
	cfgd->usb1p1_Compatibility = 0u;

	cfg->cfg_d = cfgd;
	cfg->cfg_c = NULL;
	cfg->cfg_s = NULL;

	return createHandle(e_handle_type_configuration,cfg);
}

Handle createInterfaceHandle ( struct Interface_Inf_s * info ) {

	uint32_t tvs = sizeof(struct lwUSB_interface_s);
	uint32_t tds = sizeof( struct lwUSB_interface_descriptor_s);

	if ( VAR_PEEK(tvs)  == FALSE ||
		 DESC_PEEK(tds) == FALSE ){
		return NULL_HANDLE;
	}

	struct lwUSB_interface_s * itf = (struct lwUSB_interface_s * )VAR_ALLOC(sizeof(struct lwUSB_interface_s ));
	struct lwUSB_interface_descriptor_s * itfd = (struct lwUSB_interface_descriptor_s*)DESC_ALLOC(sizeof(struct lwUSB_interface_descriptor_s));

	/* Init Descriptor */
	itfd->bInterfaceClass = info->itf_class;
	itfd->bInterfaceSubclass = info->itf_subClass;
	itfd->bInterfaceProtocol = info->itf_protocol;

	itfd->bLength = sizeof(lwUSB_interface_descriptor_s);
	itfd->bDescriptorType = 0x04;
	itfd->bAlternateSetting = 0x00;
	itfd->bNumEndpoints = 0u;
	itfd->iInterface = 0u;
	/* Actually Number of Settings is an Interface Specific Field.
	 * The Descriptor fields just describes the ID of the alternate Setting.
	 *   */
	itf->ns = itfNumSettings;

	itf->itf_d = itfd;
	itf->itf_s = NULL;
	itf->itf_c = NULL;

	return createHandle(e_handle_type_interface,itf);
}

Handle CreateStringHandle( struct String_Inf_s * info ){

	LW_ASSERT(info->str_content);
	LW_ASSERT(info->str_id != 0u);
	LW_ASSERT(info->str_id != 1u);
	LW_ASSERT(info->str_id != 2u);
	LW_ASSERT(info->str_id != 3u);
	LW_ASSERT(info->str_length != 0u);
	LW_ASSERT(info->str_encoding > lwUSB_String_Encoding_e_Start);
	LW_ASSERT(info->str_encoding < lwUSB_String_Encoding_e_End);

	uint32_t tvs = sizeof(struct lwUSB_string_s);
	uint32_t tds = sizeof(struct lwUSB_string_descriptor_s);

	if ( VAR_PEEK(tvs)  == FALSE ||
		 DESC_PEEK(tds) == FALSE ){
		return NULL_HANDLE;
	}
	struct lwUSB_string_s * s = (struct lwUSB_string_s * )VAR_ALLOC(sizeof(struct lwUSB_string_s ));
	struct lwUSB_string_descriptor_s * sd = (struct lwUSB_string_descriptor_s * )DESC_ALLOC(sizeof(struct lwUSB_string_descriptor_s ));

	sd->bDescriptorType = e_lwUSB_bdescriptor_type_string;
	sd->bLength = sizeof(struct lwUSB_string_descriptor_s) +  info->str_length * (info->str_encoding == lwUSB_String_Encoding_e_ASCII ? (2u) : (1u));

	s->s_du = makeDataUnit((uint8_t*)info->str_content, makeDataInfo(info->str_length, info->str_encoding));
	s->s_d = sd;
	s->s_id = info->str_id;

	return createHandle(e_handle_type_string,s);
}

Handle CreateEndpointHandle( struct EndPoint_Inf_s * info ) {

	AssertHandle( phyHandle , e_handle_type_phy_endpoint);
	LW_ASSERT( epType < e_lwUSB_EndPoint_Type_MAX );
	LW_ASSERT( epDir < e_lwUSB_EndPoint_Direction_MAX );
	LW_ASSERT( epAddress <= 15u );

	if ( epType  == e_lwUSB_EndPoint_Type_Isochronous ||
		 epType  == e_lwUSB_EndPoint_Type_Interrupt ){
		LW_ASSERT(polltms);
	}

	uint32_t tvs = sizeof(struct lwUSB_EndPoint_s);
	uint32_t tds = sizeof( struct lwUSB_endpoint_descriptor_s);

	if ( VAR_PEEK(tvs)  == FALSE ||
		 DESC_PEEK(tds) == FALSE ){
		return NULL_HANDLE;
	}

	struct lwUSB_EndPoint_s * ep = (struct lwUSB_EndPoint_s * )VAR_ALLOC(sizeof(struct lwUSB_EndPoint_s ));
	struct lwUSB_endpoint_descriptor_s * epd = (struct lwUSB_endpoint_descriptor_s*)DESC_ALLOC(sizeof(struct lwUSB_endpoint_descriptor_s));

    /* Init Descriptor */
	epd->bLength = 7u ;
	epd->bDescriptorType = e_lwUSB_bdescriptor_type_endpoint ;
	epd->bInterval =  pollTms ;
	epd->Direction = epDir ;
	epd->Endpoint_number = epAddress ;
	epd->reserved1 = 0;
	epd->reserved2 = 0;
	epd->MaxPacketSize = maxPacketSize ;
	epd->Transfer_Type = epType ;
	epd->Additional_Transactions_PerMicroFrame = 0u ;
	/* Note Synchronization in case of Isochronous Endpoints are hardcoded to No Synchronization and Usage to Data Endpoint */
	epd->Synchronization_Type = 0u ;
	epd->Usage_Type = 0u ;

	/* Init EndPoint */
	ep->ep_d = epd;
	ep->ep_phy = (struct lwUSB_PhyEndPoint_s *) phyHandle;

	return createHandle(e_handle_type_endpoint,ep);
}

Handle CreatePhyHandle( struct Phy_Inf_s * info ){
	/* We Shall not assert epNum */
	LW_ASSERT(Buffersz);

	uint32_t tbs =  (1u+isDouble)*BufferSz;
	uint32_t tvs = sizeof(struct lwUSB_PhyEndPoint_s) + (1u+isDouble)*(sizeof(struct DataPool_s));

	if ( VAR_PEEK(tvs)  == FALSE  ||
		 BUFF_PEEK(tbs) == FALSE ){
		return NULL;
	}

	struct lwUSB_PhyEndPoint_s * phy = NULL;
	struct DataPool_s * dp0 = NULL;
	struct DataPool_s * dp1 = NULL;
	uint8_t* b0 = NULL;
	uint8_t* b1 = NULL;

	phy = (struct lwUSB_PhyEndPoint_s *)VAR_ALLOC(sizeof(struct lwUSB_PhyEndPoint_s));
	dp0 = (struct DataPool_s *)VAR_ALLOC(sizeof(struct DataPool_s));
	b0 = (uint8_t*) BUFF_ALLOC(BufferSz);
	DataPool_Init(dp0, b0, BufferSz);
	if ( isDouble ){
		dp1 = (struct DataPool_s *)VAR_ALLOC(sizeof(struct DataPool_s));
		b1 = (uint8_t*) BUFF_ALLOC(BufferSz);
		DataPool_Init(dp1, b1, BufferSz);
	}
	phy->dp = dp0;
	phy->dp_r = dp1;
	phy->ep_n = epNum;

	return createHandle(e_handle_type_phy_endpoint,phy);
}

