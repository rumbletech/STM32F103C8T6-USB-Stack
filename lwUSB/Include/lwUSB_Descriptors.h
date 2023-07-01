/*
 * lwUSB_Descriptors.h
 *
 *  Created on: Dec 30, 2022
 *      Author: Garmoosh
 */

#ifndef LWUSB_DESCRIPTORS_H_
#define LWUSB_DESCRIPTORS_H_

#include "lwUSB_Opts.h"
#include "lwUSB_SPECS.h"

//todo try to make static


struct lwUSB_device_descriptor_s lwUSB_Device_Descriptor = {

	    .bLength                = 18u ,
		.bDescriptorType 		= e_lwUSB_bdescriptor_type_device ,
		.bcdUSB                 = 0x200 ,
	    .bDeviceClass			= e_lwUSB_device_class_interface_defined ,
		.bDeviceSubClass		= e_lwUSB_device_class_interface_defined ,
		.bDeviceProtocol        = e_lwUSB_device_class_interface_defined ,
		.bMaxPacketSize0        = LWUSB_OPTS_CONTROL_EP_RX_B_SIZE ,
		.idVendor               = LWUSB_OPTS_VENDOR_ID ,
		.idProduct              = LWUSB_OPTS_PRODUCT_ID ,
		.bcdDevice              = LWUSB_OPTS_BCD_VERSION ,
#if LWUSB_OPTS_MANUFACTURER_STRINGD_EN != 0u
		.iManufacturer          = LWUSB_OPTS_MANUFACTURER_STRINGD_I ,
#else
		.iManufacturer          = LWUSB_OPTS_MANUFACTURER_STRINGD_EN ,
#endif
#if LWUSB_OPTS_PRODUCT_STRINGD_EN != 0u
		.iProduct               = LWUSB_OPTS_PRODUCT_STRINGD_I ,
#else
		.iProduct               = LWUSB_OPTS_PRODUCT_STRINGD_EN ,
#endif
#if LWUSB_OPTS_SERIAL_NUMBER_STRINGD_EN != 0u
		.iSerialNumber          = LWUSB_OPTS_PRODUCT_STRINGD_I ,
#else
		.iSerialNumber          = LWUSB_OPTS_SERIAL_NUMBER_STRINGD_EN ,
#endif
		.bNumConfigurations     = 1u ,

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

};










#endif /* LWUSB_DESCRIPTORS_H_ */
