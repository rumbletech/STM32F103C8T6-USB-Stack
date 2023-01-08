/*
 * lwUSB_Descriptors.h
 *
 *  Created on: Dec 30, 2022
 *      Author: Garmoosh
 */

#ifndef SRC_LWUSB_DESCRIPTORS_H_
#define SRC_LWUSB_DESCRIPTORS_H_

#include "lwUSB_SPECS.h"
#include "lwUSB.h"

//todo try to make static


struct lwUSB_device_descriptor_s lwUSB_Device_Descriptor = {

	    .bLength                = 18u ,
		.bDescriptorType 		= e_lwUSB_bdescriptor_type_device ,
		.bcdUSB                 = 0x200 ,
	    .bDeviceClass			= e_lwUSB_device_class_interface_defined ,
		.bDeviceSubClass		= e_lwUSB_device_class_interface_defined ,
		.bDeviceProtocol        = e_lwUSB_device_class_interface_defined ,
		.bMaxPacketSize0        = 64u ,
		.idVendor               = 0x1672 ,
		.idProduct              = 0x0407 ,
		.bcdDevice              = 0x4003 ,
		.iManufacturer          = 1 ,
		.iProduct               = 2 ,
		.iSerialNumber          = 3 ,
		.bNumConfigurations     = 0u ,

};


struct lwUSB_configuration_descriptor_s lwUSB_Configuration_Descriptor = {

		.bLength = 9u ,
		.bDescriptorType = e_lwUSB_bdescriptor_type_configuration ,
		.wTotalLength =  0u ,
		.bNumInterfaces = 1u ,
		.bConfigurationValue = 0x23  ,
		.iConfiguration = 0 ,
		.bmAttributes = 0xA0 ,
		.bMaxPower  = 0x2D ,

};

struct lwUSB_endpoint_descriptor_s  lwUSB_endpoint1_Descriptor = {

		.bLength = 7u ,
		.bDescriptorType = e_lwUSB_bdescriptor_type_endpoint ,
		.Endpoint_number = 1u ,
		.Transfer_Type = e_lwUSB_endpoint_transfer_type_bulk ,
		.MaxPacketSize = 32u ,
		.bInterval = 6u ,

};

struct lwUSB_interface_descriptor_s lwUSB_interface1_Descriptor  = {

	 .bLength = 9u ,
	 .bDescriptorType = e_lwUSB_bdescriptor_type_interface ,
	 .bInterfaceNumber = 0u ,
	 .bAlternateSetting = 0u ,
	 .bNumEndpoints = 1u ,
	 .bInterfaceClass = 0xFF ,
	 .bInterfaceSubclass = 0xFF ,
	 .bInterfaceProtocol = 0xFF ,
	 .iInterface = 0u ,


};


struct lwUSB_string_descriptor0_s lwUSB_string0_descriptor  = {

	  .bLength = 4u ,
	  .bDescriptorType = e_lwUSB_bdescriptor_type_string ,
	  .wLANGID = 0x409 ,

};

struct lwUSB_string_descriptor_s  lwUSB_manufacturer_string_descriptor = {

	  .bLength =  13u ,
	  .bDescriptorType = e_lwUSB_bdescriptor_type_string ,
	  .bString   = "Rumble-Tech"

};

struct lwUSB_string_descriptor_s  lwUSB_product_string_descriptor = {

	  .bLength =  12u ,
	  .bDescriptorType = e_lwUSB_bdescriptor_type_string ,
	  .bString   = "Digi Probe"

};

struct lwUSB_string_descriptor_s  lwUSB_serial_string_descriptor = {

	  .bLength =  16u ,
	  .bDescriptorType = e_lwUSB_bdescriptor_type_string ,
	  .bString   = "X345IOPQK1239E"

};

struct lwUSB_microsoft_os_string_descriptor_s lwUSB_microsoft_os_string_descriptor  = {
	  .bLength = 18u ,
	  .bDescriptorType = e_lwUSB_bdescriptor_type_string ,
	  .bString = "MSFT100" ,
	  .VendorCode = 0X32 ,
	  .reserved   = 0X00 ,

};



#endif /* SRC_LWUSB_DESCRIPTORS_H_ */
