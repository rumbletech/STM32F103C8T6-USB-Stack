/*
 * tree.c
 *
 *  Created on: Sep 3, 2023
 *      Author: mrashada
 */

#include "tree.h"
#include "ringbuffer.h"
#include "linearbuffer.h"
#include "memctrl.h"

struct parent_node_s {
	struct ll_s * child_nodes;
	void * descriptor;
	void * params;
};

static struct lwUSB_device_descriptor_s dev_ds;
static struct lwUSB_device_s dev;
static Handle dev_h;

static inline Handle createHandle( htype t , void* c ){
	Handle h;
	h.ht = t;
	h.hc = c;
	return h;
}

lwUSB_Err tree_CalculateDescriptors ( void ){

	struct ll_s * w_cfg = dev.dev_c;

	uint8_t n_cfg = 0u;

	while( w_cfg != NULL ){
		struct lwUSB_configuration_s * cfg = w_cfg->content;
		struct ll_s * w_itf = cfg->cfg_c;
		uint8_t n_eps = 0u;
		uint8_t n_itf = 0u;

		while( w_itf != NULL ){

			struct lwUSB_interface_s * itf = w_itf->content;
			struct ll_s * w_ep = itf->itf_c;
			while( w_ep != NULL ){
				n_eps++;
				w_ep = w_ep->next;

			}
			n_itf++;
			w_itf = w_itf->next;
		}
		cfg->cfg_d->wTotalLength += n_itf * 9u + n_eps * 7u ;
		n_cfg++;
		w_cfg = w_cfg->next ;
	}

	dev.dev_d->bNumConfigurations = n_cfg;

	return LWUSB_ERR_OK;
}

void tree_Init( void ){

	dev_ds.bDescriptorType = 0x01;
	dev_ds.bDeviceClass = LWUSB_OPTS_DEV_CLASS;
	dev_ds.bDeviceProtocol = LWUSB_OPTS_DEV_PROTOCOL;
	dev_ds.bDeviceSubClass = LWUSB_OPTS_DEV_SUBCLASS;
	dev_ds.bLength = 18u;
	dev_ds.bMaxPacketSize0 = LWUSB_OPTS_DEV_CTRL_MAX_PACKET_SZ;
	dev_ds.bNumConfigurations = 0u;
	dev_ds.bcdDevice = LWUSB_OPTS_DEV_BCD_VERSION;
	dev_ds.bcdUSB = 0x0200;
	dev_ds.idProduct = LWUSB_OPTS_DEV_PRODUCT_ID;
	dev_ds.idVendor = LWUSB_OPTS_DEV_VENDOR_ID;
	dev_ds.iSerialNumber = LWUSB_OPTS_DEV_SERIAL_NUMBER_STRINGD_I;
	dev_ds.iManufacturer = LWUSB_OPTS_DEV_MANUFACTURER_STRINGD_I;
	dev_ds.iProduct = LWUSB_OPTS_DEV_PRODUCT_STRINGD_I;

	dev.dev_c = NULL;
	dev.dev_d = &dev_ds;

	dev_h.hc = (void*)&dev;
	dev_h.ht = e_handle_type_device;

}
Handle tree_CreateConfigurationHandle ( struct Configuration_Inf_s * info ){

	Handle H;

	H.hc = NULL;
	H.ht = e_handle_type_invalid;

	uint32_t tvs = sizeof(struct lwUSB_configuration_s);
	uint32_t tds = sizeof( struct lwUSB_configuration_descriptor_s);

	if ( VAR_PEEK(tvs)  == FALSE ||
		 DESC_PEEK(tds) == FALSE ){
		return H;
	}

	struct lwUSB_configuration_s * cfg = (struct lwUSB_configuration_s * )VAR_ALLOC(sizeof(struct lwUSB_configuration_s ));
	struct lwUSB_configuration_descriptor_s * cfgd = (struct lwUSB_configuration_descriptor_s*)DESC_ALLOC(sizeof(struct lwUSB_configuration_descriptor_s));

	cfgd->selfPowered = info->cfg_selfPowered;
	cfgd->remoteWakeup = info->cfg_remoteWakeUp;
	cfgd->bConfigurationValue = info->cfg_value;
	cfgd->bMaxPower = info->cfg_maxPower;

	cfgd->bLength = sizeof(struct lwUSB_configuration_descriptor_s);
	cfgd->bDescriptorType = 0x02;
	cfgd->wTotalLength = cfgd->bLength;
	cfgd->bNumInterfaces = 0u;
	cfgd->iConfiguration = 0u;
	cfgd->reserved = 0u;
	cfgd->usb1p1_Compatibility = 0u;

	cfg->cfg_d = cfgd;
	cfg->cfg_c = NULL;
	cfg->cfg_s = NULL;

	H = createHandle(e_handle_type_configuration,cfg);

	return H;
}

Handle tree_CreateInterfaceHandle ( struct Interface_Inf_s * info ) {

	Handle H;

	H.hc = NULL;
	H.ht = e_handle_type_invalid;

	uint32_t tvs = sizeof(struct lwUSB_interface_s);
	uint32_t tds = sizeof( struct lwUSB_interface_descriptor_s);

	if ( VAR_PEEK(tvs)  == FALSE ||
		 DESC_PEEK(tds) == FALSE ){
		return H;
	}

	struct lwUSB_interface_s * itf = (struct lwUSB_interface_s * )VAR_ALLOC(sizeof(struct lwUSB_interface_s ));
	struct lwUSB_interface_descriptor_s * itfd = (struct lwUSB_interface_descriptor_s*)DESC_ALLOC(sizeof(struct lwUSB_interface_descriptor_s));

	/* Init Descriptor */
	itfd->bInterfaceClass = info->itf_class;
	itfd->bInterfaceSubclass = info->itf_subClass;
	itfd->bInterfaceProtocol = info->itf_protocol;

	itfd->bLength = sizeof(struct lwUSB_interface_descriptor_s);
	itfd->bDescriptorType = 0x04;
	itfd->bAlternateSetting = 0x00;
	itfd->bNumEndpoints = 0u;
	itfd->iInterface = 0u;
	itfd->bInterfaceNumber = info->itf_number;
	/* Actually Number of Settings is an Interface Specific Field.
	 * The Descriptor fields just describes the ID of the alternate Setting.
	 *   */
	itf->ns = info->itf_numAlternateSettings;
	itf->itf_d = itfd;
	itf->itf_s = NULL;
	itf->itf_c = NULL;

	H = createHandle(e_handle_type_interface,itf);
	return H;
}

Handle tree_CreateStringHandle( struct String_Inf_s * info ){

	Handle H;

	H.hc = NULL;
	H.ht = e_handle_type_invalid;

	uint32_t tvs = sizeof(struct lwUSB_string_s);
	uint32_t tds = sizeof(struct lwUSB_string_descriptor_s);

	if ( VAR_PEEK(tvs)  == FALSE ||
		 DESC_PEEK(tds) == FALSE ){
		return H;
	}
	struct lwUSB_string_s * s = (struct lwUSB_string_s * )VAR_ALLOC(sizeof(struct lwUSB_string_s ));
	struct lwUSB_string_descriptor_s * sd = (struct lwUSB_string_descriptor_s * )DESC_ALLOC(sizeof(struct lwUSB_string_descriptor_s ));

	sd->bDescriptorType = e_lwUSB_bdescriptor_type_string;
	sd->bLength = sizeof(struct lwUSB_string_descriptor_s) +  info->str_length * (info->str_encoding == lwUSB_String_Encoding_e_ASCII ? (2u) : (1u));

	s->s_d = sd;
	s->s_id  = info->str_id;
	s->s_enc = info->str_encoding;
	s->s_len = info->str_length;
	s->s_ptr = info->str_content;

	H =  createHandle(e_handle_type_string,s);

	return H;
}

Handle tree_CreateEndpointHandle( struct EndPoint_Inf_s * info ) {

	Handle H;

	H.hc = NULL;
	H.ht = e_handle_type_invalid;

	uint32_t tvs = sizeof(struct lwUSB_EndPoint_s);
	uint32_t tds = sizeof( struct lwUSB_endpoint_descriptor_s);

	if ( VAR_PEEK(tvs)  == FALSE ||
		 DESC_PEEK(tds) == FALSE ){
		return H;
	}

	struct lwUSB_EndPoint_s * ep = (struct lwUSB_EndPoint_s * )VAR_ALLOC(sizeof(struct lwUSB_EndPoint_s ));
	struct lwUSB_endpoint_descriptor_s * epd = (struct lwUSB_endpoint_descriptor_s*)DESC_ALLOC(sizeof(struct lwUSB_endpoint_descriptor_s));

    /* Init Descriptor */
	epd->bLength = 7u ;
	epd->bDescriptorType = e_lwUSB_bdescriptor_type_endpoint ;
	epd->bInterval =  info->pollTms ;
	epd->Direction = info->epDir ;
	epd->Endpoint_number = info->epAddress ;
	epd->reserved1 = 0;
	epd->reserved2 = 0;
	epd->MaxPacketSize = info->maxPacketSize ;
	epd->Transfer_Type = info->epType ;
	epd->Additional_Transactions_PerMicroFrame = 0u ;
	/* Note Synchronization in case of Isochronous Endpoints are hardcoded to No Synchronization and Usage to Data Endpoint */
	epd->Synchronization_Type = 0u ;
	epd->Usage_Type = 0u ;

	/* Init EndPoint */
	ep->ep_d = epd;
	ep->ep_phy = (struct lwUSB_PhyEndPoint_s *) info->phyHandle.hc;

	H = createHandle(e_handle_type_endpoint,ep);
	return H;
}

Handle tree_CreatePhyHandle( struct Phy_Inf_s * info ){

	Handle H;

	H.hc = NULL;
	H.ht = e_handle_type_invalid;

	uint32_t tbs =  (1u+info->phy_isDouble)*info->phy_buffSize;
	uint32_t tvs = sizeof(struct lwUSB_PhyEndPoint_s) + (1u+info->phy_isDouble)*(sizeof(struct RingBuffer_s) + sizeof(struct LinearBuffer_s));

	if ( VAR_PEEK(tvs)  == FALSE  ||
		 BUFF_PEEK(tbs) == FALSE ){
		return H;
	}

	struct lwUSB_PhyEndPoint_s * phy = NULL;
	struct RingBuffer_s * dp0 = NULL;
	struct RingBuffer_s * dp1 = NULL;
	struct LinearBuffer_s * tb0 = NULL;
	struct LinearBuffer_s * tb1 = NULL;
	uint8_t* b0 = NULL;
	uint8_t* b1 = NULL;

	phy = (struct lwUSB_PhyEndPoint_s *)VAR_ALLOC(sizeof(struct lwUSB_PhyEndPoint_s));
	dp0 = (struct RingBuffer_s *)VAR_ALLOC(sizeof(struct RingBuffer_s));
	tb0 = (struct LinearBuffer_s *)VAR_ALLOC(sizeof(struct LinearBuffer_s));
	tb1 = (struct LinearBuffer_s *)VAR_ALLOC(sizeof(struct LinearBuffer_s));
	b0 = (uint8_t*) BUFF_ALLOC(info->phy_buffSize);
	RingBuffer_Init(dp0, b0, info->phy_buffSize);

	if ( info->phy_IN ){
		LinearBuffer_Init(tb0, info->phy_IN, info->phy_hwbSize);
	}
	if ( info->phy_OUT ){
		LinearBuffer_Init(tb0, info->phy_OUT, info->phy_hwbSize);
	}
	if ( info->phy_isDouble ){
		dp1 = (struct RingBuffer_s *)VAR_ALLOC(sizeof(struct RingBuffer_s));
		b1 = (uint8_t*) BUFF_ALLOC(info->phy_buffSize);
		RingBuffer_Init(dp1, b1, info->phy_buffSize);
	}
	phy->dp = dp0;
	phy->dp_r = dp1;
	phy->lb = tb0;
	phy->lb_r = tb1;
	phy->ep_n = info->phy_EpNumber;

	H = createHandle(e_handle_type_phy_endpoint,phy);

	return H;
}

lwUSB_Err tree_registerChild( Handle child , Handle parent ){

	struct parent_node_s * c_parent = (struct parent_node_s *)parent.hc;
	void * c_child  = child.hc;

	uint32_t tvs = sizeof(struct ll_s);

	if ( VAR_PEEK(tvs)  == FALSE ){
		return LWUSB_ERR_MEM;
	}

	struct ll_s * c_child_wrap = (struct ll_s *)VAR_ALLOC(sizeof(struct ll_s));
	c_child_wrap->content =  (void*)c_child;
	c_child_wrap->next = (struct ll_s*)c_parent->child_nodes;
	c_parent->child_nodes = c_child_wrap;

	return LWUSB_ERR_OK ;

}

Handle tree_getDev( void ){
	return dev_h;
}
