/*
 * lwUSB_Opts.h
 *
 *  Created on: Jan 13, 2023
 *      Author: mrashada
 */

#ifndef LWUSB_OPTS_H_
#define LWUSB_OPTS_H_

#include "lwUSB_SPECS.h"


/******************************************************* HW Specific Defines *************************************************************/
/*****************************************************************************************************************************************/

#define LWUSB_OPTS_NUM_HW_EPS 8u						/* Maximum Number of EP's That the Hardware Supports */

/****************************************************** Control EP Configuration *********************************************************/
/*****************************************************************************************************************************************/

#define LWUSB_OPTS_CONTROL_EP_TX_B_SIZE 64u				/* Control EP Transfer Buffer Size */
#define LWUSB_OPTS_CONTROL_EP_RX_B_SIZE 32u				/* Control EP Receive  Buffer Size */

/****************************************************** Stack Speicifc Defines ***********************************************************/
/*****************************************************************************************************************************************/

#define LWUSB_OPTS_CONFIG_VALUE_START    0x40			/* First Configuration Descriptor Value */
#define LWUSB_OPTS_INTERFACE_VALUE_START 0x00           /* First Interface Descriptor Value */
#define LWUSB_OPTS_STRING_INDEX_START    0x20			/* First String Descriptor Value */

/******************************************************* Device Descriptor Options ********************************************************/
/*****************************************************************************************************************************************/

#define LWUSB_OPTS_VENDOR_ID 			0x1672		    /* Vendor ID */
#define LWUSB_OPTS_PRODUCT_ID			0x0407			/* Product ID */
#define LWUSB_OPTS_BCD_VERSION          0x4003          /* Version of the Device , in binary coded decimal */
#define LWUSB_OPTS_VENDOR_CODE          0x32			/* Vendor Code in Microsoft Windows Registry */

#define LWUSB_OPTS_LANGID               LANGID_ENGLISH,LANGID_ENGLISH 	/* Array of Supported LANGID */


#define LWUSB_OPTS_SERIAL_NUMBER_STRINGD_EN 0x1
#define LWUSB_OPTS_MANUFACTURER_STRINGD_EN  0x1
#define LWUSB_OPTS_PRODUCT_STRINGD_EN       0x1


#if defined(LWUSB_OPTS_SERIAL_NUMBER_STRINGD_EN) && LWUSB_OPTS_SERIAL_NUMBER_STRINGD_EN != 0u
/* Serial Number String */
#define LWUSB_OPTS_SERIAL_NUMBER_STRING "SerialNumber PlaceHolder"
#define LWUSB_OPTS_SERIAL_NUMBER_STRINGD_I 1u
#endif

#if defined(LWUSB_OPTS_MANUFACTURER_STRINGD_EN) && LWUSB_OPTS_MANUFACTURER_STRINGD_EN != 0u
/* Manufacturer String */
#define LWUSB_OPTS_MANUFACTURER_STRING "ManuFacturer PlaceHolder"
#define LWUSB_OPTS_MANUFACTURER_STRINGD_I 2u

#endif

#if defined(LWUSB_OPTS_PRODUCT_STRINGD_EN) && LWUSB_OPTS_PRODUCT_STRINGD_EN != 0u
/* Product String */
#define LWUSB_OPTS_PRODUCT_STRING "Product PlaceHolder"
#define LWUSB_OPTS_PRODUCT_STRINGD_I 3u
#endif

/*****************************************************************************************************************************************/
/*****************************************************************************************************************************************/





#define LWUSB_OPTS_

#endif /* LWUSB_OPTS_H_ */
