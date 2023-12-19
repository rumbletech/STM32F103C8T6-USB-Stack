/*
 * lwUSB_Std.h
 *
 *  Created on: Jun 28, 2023
 *      Author: Garmoosh
 */

#ifndef LWUSB_STD_H_
#define LWUSB_STD_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "lwUSB_Opts.h"
#include "../DataUnit/DataUnit.h"

/* MAXIMUM Value Allowed By Stack */
#define LWUSB_MAX_EP_N     8u
#define LWUSB_MAX_CONFIG_N 1u
#define LWUSB_MAX_INTF_N   1u

/* MINIMUM Value Allowed By Stack */
#define LWUSB_MIN_EP_N     1u
#define LWUSB_MIN_CONFIG_N 1u
#define LWUSB_MIN_INTF_N   1u

/* Default Values */
#define LWUSB_DEFAULT_EP_TX_BS 256u
#define LWUSB_DEFAULT_EP_RX_BS 256u
#define BM_BS 4u

#define LWUSB_ENUM_TYPE_S 32u
#define LWUSB_BUS_EVENTS_S 0u
#define LWUSB_INTERNAL_EVENTS_S 33u
#define LWUSB_ENUM_STRING_ENCODING_S 0U

#define LWUSB_ERR_MEM   -3
#define LWUSB_ERR_NULL  -2
#define LWUSB_ERR_FAULT -1
#define LWUSB_ERR_OK     0

#define WEAK __attribute__((weak))

typedef uint8_t boolean;
typedef int32_t lwUSB_Err ;
#define BOOL boolean;
#define TRUE 1u
#define FALSE 0u

#define LWERR_
struct ll_s {
	void * content;
	struct ll_s * next ;
};

#define BMREQUEST_DIRECTION_HOST_TO_DEVICE 0u
#define BMREQUEST_DIRECTION_DEVICE_TO_HOST 1u
#define BMREQUEST_TYPE_STANDARD 0u
#define BMREQUEST_TYPE_CLASS    1u
#define BMREQUEST_TYPE_VENDOR   2u
#define BMREQUEST_RECIPIENT_DEVICE 0u
#define BMREQUEST_RECIPIENT_INTERFACE 1u
#define BMREQUEST_RECIPIENT_ENDPOINT 2u
#define BMREQUEST_RECIPIENT_OTHERS 3u
#define USB_LANGID_ENGLISH  0x0409

enum lwUSB_InternalEvents_e {
	lwUSB_InternalEvents_e_Start = LWUSB_INTERNAL_EVENTS_S,
	lwUSB_InternalEvents_e_OVF ,
	lwUSB_InternalEvents_e_End = LWUSB_INTERNAL_EVENTS_S +  LWUSB_ENUM_TYPE_S + 1u ,
};

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
	e_lwUSB_EndPoint_Direction_MAX     = 3u  ,

} e_lwUSB_EndPoint_Direction ;

typedef enum {

	e_lwUSB_EndPoint_State_Disabled  = 0b00 ,
	e_lwUSB_EndPoint_State_Stall     = 0b01 ,
	e_lwUSB_EndPoint_State_Nak       = 0b10 ,
	e_lwUSB_EndPoint_State_Valid     = 0b11 ,

} e_lwUSB_EndPoint_State ;

typedef enum {

	e_lwUSB_endpoint_transfer_type_control     = 0b00 ,
	e_lwUSB_endpoint_transfer_type_isochronous = 0b01 ,
	e_lwUSB_endpoint_transfer_type_bulk        = 0b10 ,
	e_lwUSB_endpoint_transfer_type_interrupt   = 0b11 ,


} e_lwUSB_endpoint_transfer_type ;

typedef enum {

	e_lwUSB_endpoint_synchronization_type_no_synchronization     = 0b00 ,
	e_lwUSB_endpoint_synchronization_type_asynchronous           = 0b01 ,
	e_lwUSB_endpoint_synchronization_type_adaptive               = 0b10 ,
	e_lwUSB_endpoint_synchronization_type_synchronous            = 0b11 ,


} e_lwUSB_endpoint_synchronization_type ;

typedef enum {

	e_lwUSB_endpoint_usage_type_data                = 0b00 ,
	e_lwUSB_endpoint_usage_type_feedback            = 0b01 ,
	e_lwUSB_endpoint_usage_implicit_feedback        = 0b10 ,

} e_lwUSB_endpoint_usage_type ;

typedef enum {

	e_lwUSB_bdescriptor_type_device                              = 0x01 ,
	e_lwUSB_bdescriptor_type_configuration                       = 0x02 ,
	e_lwUSB_bdescriptor_type_string                              = 0x03 ,
	e_lwUSB_bdescriptor_type_interface                           = 0x04 ,
	e_lwUSB_bdescriptor_type_endpoint                            = 0x05 ,
	e_lwUSB_bdescriptor_type_device_qualifier                    = 0x06 ,
	e_lwUSB_bdescriptor_type_other_speed_configuration           = 0x07 ,
	e_lwUSB_bdescriptor_type_interface_power                     = 0x08 ,
	e_lwUSB_bdescriptor_type_otg                                 = 0x09 ,
	e_lwUSB_bdescriptor_type_debug                               = 0x0A ,
	e_lwUSB_bdescriptor_type_interface_association               = 0x0B ,
	e_lwUSB_bdescriptor_type_security                            = 0X0C ,
	e_lwUSB_bdescriptor_type_key                                 = 0X0D ,
	e_lwUSB_bdescriptor_type_encryption_type                     = 0X0E ,
	e_lwUSB_bdescriptor_type_bos                                 = 0X0F ,
	e_lwUSB_bdescriptor_type_device_capability                   = 0X10 ,
	e_lwUSB_bdescriptor_type_wireless_endpoint_companion         = 0X11 ,
	e_lwUSB_bdescriptor_type_superspeed_endpoint_companion       = 0X30

} e_lwUSB_bdescriptor_type ;

typedef enum {

	e_lwUSB_device_class_interface_defined    		= 0x00 ,
	e_lwUSB_device_class_communication_device 		= 0x02 ,
	e_lwUSB_device_class_hub                  		= 0x09 ,
	e_lwUSB_device_class_personal_healthcare_device = 0x0F ,
	e_lwUSB_device_class_diagnostic_device          = 0xDC ,
	e_lwUSB_device_class_wireless_controller        = 0xE0 ,
	e_lwUSB_device_class_miscellaneous              = 0xEF ,
	e_lwUSB_device_class_vendor_specific            = 0xFF ,

} e_lwUSB_device_class ;

typedef enum {

	e_lwUSB_setup_request_get_status            = 0x00 ,
	e_lwUSB_setup_request_clear_feature         = 0x01 ,
	e_lwUSB_setup_request_set_feature           = 0x03 ,
	e_lwUSB_setup_request_set_address           = 0x05 ,
	e_lwUSB_setup_request_get_descriptor        = 0x06 ,
	e_lwUSB_setup_request_set_descriptor        = 0x07 ,
	e_lwUSB_setup_request_get_configuration     = 0x08 ,
	e_lwUSB_setup_request_set_configuration     = 0x09 ,
	e_lwUSB_setup_request_get_interface         = 0x0A ,
	e_lwUSB_setup_request_set_interface         = 0x0B ,
	e_lwUSB_setup_request_synch_frame           = 0x0C ,
	e_lwUSB_setup_request_set_sel               = 0x30 ,
	e_lwUSB_setup_request_set_isochronous_delay = 0x31 ,

} e_lwUSB_setup_request ;

struct lwUSB_device_descriptor_s {

	uint8_t  bLength ;										/* Size of descriptor in bytes */
	uint8_t  bDescriptorType ;								/* DEVICE Descriptor type */
	uint16_t bcdUSB ;										/* USB Descriptor Compliance 210H for 2.10 */
	uint8_t  bDeviceClass ;									/* Class Code */
	uint8_t  bDeviceSubClass ;								/* SubClass Code */
	uint8_t  bDeviceProtocol ;								/* Class-Specific Protocols */
	uint8_t  bMaxPacketSize0 ;								/* MaxPacket size for end point zero */
	uint16_t idVendor ;										/* Vendor ID */
	uint16_t idProduct ;									/* Product ID */
	uint16_t bcdDevice ;									/* Device Release Number in BCD Format */
	uint8_t  iManufacturer ;								/* Index for Manufacturer String Descriptor */
	uint8_t  iProduct ;									    /* Index for Product String Descriptor */
	uint8_t  iSerialNumber ;								/* Index for SerialNumber String Descriptor */
	uint8_t  bNumConfigurations ;							/* Number of Possible Configurations */

};

struct lwUSB_device_qualifier_descriptor_s {

	uint8_t  bLength ;
	uint8_t  bDescriptorType ;
	uint16_t bcdUSB ;
	uint8_t  bDeviceClass ;
	uint8_t  bDeviceSubClass ;
	uint8_t  bDeviceProtocol ;
	uint8_t  bMaxPacketSize0 ;
	uint8_t  bNumConfigurations ;
	uint8_t  reserved ;

};

struct lwUSB_configuration_descriptor_s {

	uint8_t  bLength ;
	uint8_t  bDescriptorType ;
	uint16_t wTotalLength ;
	uint8_t  bNumInterfaces ;
	uint8_t  bConfigurationValue  ;
	uint8_t  iConfiguration ;
	union {
		uint8_t  bmAttributes ;
		struct {
			uint8_t reserved : 5 ;
			uint8_t remoteWakeup : 1 ;
			uint8_t selfPowered  : 1 ;
			uint8_t usb1p1_Compatibility : 1 ;
		};
	};
	uint8_t  bMaxPower ;

};

struct lwUSB_other_speed_configuration_descriptor_s {

	uint8_t  bLength ;
	uint8_t  bDescriptorType ;
	uint16_t wTotalLength ;
	uint8_t  bNumInterfaces ;
	uint8_t  bConfigurationValue  ;
	uint8_t  iConfiguration ;
	uint8_t  bmAttributes ;
	uint8_t  bMaxPower ;

};

struct lwUSB_interface_descriptor_s {

	uint8_t  bLength ;
	uint8_t  bDescriptorType ;
	uint8_t  bInterfaceNumber ;
	uint8_t  bAlternateSetting ;
	uint8_t  bNumEndpoints  ;
	uint8_t  bInterfaceClass ;
	uint8_t  bInterfaceSubclass ;
	uint8_t  bInterfaceProtocol ;
	uint8_t  iInterface ;

};

struct lwUSB_endpoint_descriptor_s {

	uint8_t  bLength ;
	uint8_t  bDescriptorType ;
	union {
		uint8_t  bEndpointAddress ;
		struct {
			uint8_t Endpoint_number : 4 ;
			uint8_t reserved1        : 3 ;
			uint8_t Direction       : 1 ;
		};
	};
	union {
		uint8_t  bmAttributes ;
		struct {
			uint8_t Transfer_Type : 2 ;
			uint8_t Synchronization_Type : 2 ;
			uint8_t Usage_Type : 2 ;
		};
	};
	union {
		uint16_t wMaxPacketSize  ;
		struct {
			uint16_t MaxPacketSize : 11 ;
			uint16_t Additional_Transactions_PerMicroFrame : 2 ;
			uint16_t reserved2 : 3 ;
		};
	};
	uint8_t  bInterval  ;

};

struct lwUSB_string_descriptor0_s {

	uint8_t  bLength ;
	uint8_t  bDescriptorType ;
	uint16_t wLANGID  ;

};

struct lwUSB_string_descriptor_s {

	uint8_t  bLength ;
	uint8_t  bDescriptorType ;

};

struct lwUSB_setup_data {


	struct {
		uint8_t Recipient   : 5 ;
		uint8_t RequestType : 2 ;
		uint8_t Direction   : 1 ;
	} bmRequestType  ;

	uint8_t bRequest ;
	union {
		uint16_t wValue  ;
		struct {
			uint8_t descriptorIndex;
			uint8_t descriptorType;
		};
	};
	union {
		uint16_t wIndex  ;
		uint16_t LangID  ;
	};
	uint16_t wLength ;
};


/* Windows Descriptors */

struct lwUSB_microsoft_os_string_descriptor_s {
	uint8_t  bLength ;
	uint8_t  bDescriptorType ;
	char*  bString ;
	uint8_t  VendorCode ;
	uint8_t  reserved   ;

};


struct lwUSB_Node {

	void * content;                  /* Describes Data in this Node */
	struct lwUSB_Node * parentNode;
	struct lwUSB_Node * childNodes;  /* Describes Children of this Node */
};

/* Descripes a USB String Object */

struct lwUSB_string_s {

	struct lwUSB_string_descriptor_s * s_d ;
	uint8_t s_id ;
	struct DataUnit_s s_du;
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

	struct ll_s * itf_c;                           /* Wrapped Child EndPoints */
	struct lwUSB_interface_descriptor_s * itf_d;   /* Interface Descriptor */
	struct lwUSB_string_s * itf_s;                 /* Interface String Descriptor */
	uint8_t ns;                                    /* Number of Alternate Settings */

};

/* This Defines a USB Configuration , and  it is fully represented by
 * Configuration Descriptor.
 * Optional String Descriptor.
 * Child Interfaces.
 */

struct lwUSB_configuration_s {

	struct ll_s * cfg_c ;                              /* Wrapped Child Interfaces */
	struct lwUSB_configuration_descriptor_s * cfg_d  ; /* Configuration Descriptor */
	struct lwUSB_string_s * cfg_s ;                    /* Configuration String Descriptor */

};

struct lwUSB_device_s {
	struct ll_s * dev_c;                        /* Wrapped Child Configurations */
	struct lwUSB_device_descriptor_s * dev_d ;  /* Device Descriptor */
};

#endif /* LWUSB_STD_H_ */
