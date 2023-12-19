/*
 * lwUSB_Opts.h
 *
 *  Created on: Jan 13, 2023
 *      Author: mrashada
 */

#ifndef LWUSB_OPTS_H_
#define LWUSB_OPTS_H_

#include "lwUSB_Std.h"

/*****************************************************************************************************************************************/
/*****************************************************************************************************************************************/

/******************************** Disable/Enable Sanity Checks *************/
#define SANITY_CHECKS 1U
/*************** Buffer Memory Size from which to allocate *****************/
#define LWUSB_BUFF_M_S         1024u + 512u
#define LWUSB_DESCRIPTOR_M_S   256u
#define LWUSB_STACK_M_S        512u

/******************************************************* Device Descriptor Options ********************************************************/
/*****************************************************************************************************************************************/

#define LWUSB_OPTS_DEV_CTRL_MAX_PACKET_SZ    64u                                    /* Maximum Packet Size Allowed for EP0 */
#define LWUSB_OPTS_DEV_CLASS                 0u                                     /* Device Class */
#define LWUSB_OPTS_DEV_SUBCLASS              0u										/* Sub Class */
#define LWUSB_OPTS_DEV_PROTOCOL              0u                                     /* Protocol */
#define LWUSB_OPTS_DEV_VENDOR_ID 			0x1672		                            /* Vendor ID */
#define LWUSB_OPTS_DEV_PRODUCT_ID			0x0407			                        /* Product ID */
#define LWUSB_OPTS_DEV_BCD_VERSION          0x4003                                  /* Version of the Device , in binary coded decimal */
#define LWUSB_OPTS_DEV_VENDOR_CODE          0x32			                        /* Vendor Code in Microsoft Windows Registry */
#define LWUSB_OPTS_DEV_LANGID               USB_LANGID_ENGLISH,USB_LANGID_ENGLISH 	/* Array of Supported LANGID */
#define LWUSB_OPTS_DEV_SERIAL_NUMBER_STRINGD_EN 0x1                                 /* Enable Serial Number String */
#define LWUSB_OPTS_DEV_SERIAL_NUMBER_STRING LWUSB_NULL_STRINGD                      /* Serial Number String */
#define LWUSB_OPTS_DEV_SERIAL_NUMBER_STRINGD_I 1u                                   /* Serial Number String Index */
#define LWUSB_OPTS_DEV_MANUFACTURER_STRINGD_EN  0x1                                 /* Enable Manufacturer String */
#define LWUSB_OPTS_DEV_MANUFACTURER_STRING LWUSB_NULL_STRINGD                       /* Manufacturer String */
#define LWUSB_OPTS_DEV_MANUFACTURER_STRINGD_I 2u                                    /* Manufacturer String Index */
#define LWUSB_OPTS_DEV_PRODUCT_STRINGD_EN       0x1                                 /* Enable Product String */
#define LWUSB_OPTS_DEV_PRODUCT_STRING LWUSB_NULL_STRINGD                            /* Product String */
#define LWUSB_OPTS_DEV_PRODUCT_STRINGD_I 3u                                         /* Product String Index */


/*****************************************************************************************************************************************/
/*****************************************************************************************************************************************/

#define LWUSB_OPTS_

#endif /* LWUSB_OPTS_H_ */
