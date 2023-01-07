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



struct lwUSB_device_descriptor_s lwUSB_Device_Descriptor = {

	    .bLength                = 18u ,
		.bDescriptorType 		= e_lwUSB_bdescriptor_type_device ,
		.bcdUSB                 = 0x200 ,
	    .bDeviceClass			= e_lwUSB_device_class_interface_defined ,
		.bDeviceSubClass		= e_lwUSB_device_class_interface_defined ,
		.bDeviceProtocol        = e_lwUSB_device_class_interface_defined ,
		.bMaxPacketSize0        = 64u ,
		.idVendor               = 0x16d1 ,
		.idProduct              = 0x0407 ,
		.bcdDevice              = 0x4003 ,
		.iManufacturer          = 1 ,
		.iProduct               = 2 ,
		.iSerialNumber          = 3 ,
		.bNumConfigurations     = 1u ,

};


struct lwUSB_configuration_descriptor_s lwUSB_Configuration_Descriptor = {

		.bLength = 9u ,
		.bDescriptorType = e_lwUSB_bdescriptor_type_configuration ,
		.wTotalLength =  128 ,
		.bNumInterfaces = 1u ,
		.bConfigurationValue = 0x23  ,
		.iConfiguration = 0 ,
		.bmAttributes = 0x80 ,
		.bMaxPower  = 100 ,

};








#endif /* SRC_LWUSB_DESCRIPTORS_H_ */
