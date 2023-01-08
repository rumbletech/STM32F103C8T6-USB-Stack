/*
 * lwUSB_SPECS.h
 *
 *  Created on: Dec 27, 2022
 *      Author: Garmoosh
 */

#ifndef SRC_LWUSB_SPECS_H_
#define SRC_LWUSB_SPECS_H_

#define BMREQUEST_DIRECTION_HOST_TO_DEVICE 0u
#define BMREQUEST_DIRECTION_DEVICE_TO_HOST 1u
#define BMREQUEST_TYPE_STANDARD 0u
#define BMREQUEST_TYPE_CLASS    1u
#define BMREQUEST_TYPE_VENDOR   2u
#define BMREQUEST_RECIPIENT_DEVICE 0u
#define BMREQUEST_RECIPIENT_INTERFACE 1u
#define BMREQUEST_RECIPIENT_ENDPOINT 2u
#define BMREQUEST_RECIPIENT_OTHERS 3u




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
	char*  bString  ;

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



#endif /* SRC_LWUSB_SPECS_H_ */
