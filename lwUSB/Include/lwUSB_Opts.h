/*
 * lwUSB_Opts.h
 *
 *  Created on: Jan 13, 2023
 *      Author: mrashada
 */

#ifndef LWUSB_OPTS_H_
#define LWUSB_OPTS_H_

#include "lwUSB_SPECS.h"
#include "lwUSB_Std.h"

/******************************************************* HW Specific Defines *************************************************************/
/*****************************************************************************************************************************************/

#define LWUSB_OPTS_NUM_EPS     2     /* Number of EndPoints */
#define LWUSB_OPTS_NUM_CONFIGS 1u    /* Number of Configurations */
#define LWUSB_OPTS_NUM_INTFS   1u    /* Number of Interfaces */

#if LWUSB_OPTS_NUM_EPS > 0u

#define LWUSB_OPTS_EP0_TX_B_SIZE LWUSB_DEFAULT_EP_TX_BS
#define LWUSB_OPTS_EP0_RX_B_SIZE LWUSB_DEFAULT_EP_RX_BS

#endif

#if LWUSB_OPTS_NUM_EPS > 1u

#define LWUSB_OPTS_EP1_TX_B_SIZE LWUSB_DEFAULT_EP_TX_BS
#define LWUSB_OPTS_EP1_RX_B_SIZE LWUSB_DEFAULT_EP_RX_BS

#endif

#if LWUSB_OPTS_NUM_EPS > 2u

#define LWUSB_OPTS_EP2_TX_B_SIZE LWUSB_DEFAULT_EP_TX_BS
#define LWUSB_OPTS_EP2_RX_B_SIZE LWUSB_DEFAULT_EP_RX_BS

#endif

#if LWUSB_OPTS_NUM_EPS > 3u

#define LWUSB_OPTS_EP3_TX_B_SIZE LWUSB_DEFAULT_EP_TX_BS
#define LWUSB_OPTS_EP3_RX_B_SIZE LWUSB_DEFAULT_EP_RX_BS

#endif

#if LWUSB_OPTS_NUM_EPS > 4u

#define LWUSB_OPTS_EP4_TX_B_SIZE LWUSB_DEFAULT_EP_TX_BS
#define LWUSB_OPTS_EP4_RX_B_SIZE LWUSB_DEFAULT_EP_RX_BS

#endif

#if LWUSB_OPTS_NUM_EPS > 5u

#define LWUSB_OPTS_EP5_TX_B_SIZE LWUSB_DEFAULT_EP_TX_BS
#define LWUSB_OPTS_EP5_RX_B_SIZE LWUSB_DEFAULT_EP_RX_BS

#endif

#if LWUSB_OPTS_NUM_EPS > 6u

#define LWUSB_OPTS_EP6_TX_B_SIZE LWUSB_DEFAULT_EP_TX_BS
#define LWUSB_OPTS_EP6_RX_B_SIZE LWUSB_DEFAULT_EP_RX_BS

#endif

#if LWUSB_OPTS_NUM_EPS > 7u

#define LWUSB_OPTS_EP7_TX_B_SIZE LWUSB_DEFAULT_EP_TX_BS
#define LWUSB_OPTS_EP7_RX_B_SIZE LWUSB_DEFAULT_EP_RX_BS

#endif


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


#if LWUSB_OPTS_NUM_EPS < LWUSB_MIN_EP_N
#error "S-C : P(LWUSB_OPTS_NUM_EPS) , Minimum of one EndPoint is needed !!"
#endif

#if LWUSB_OPTS_NUM_CONFIGS < LWUSB_MIN_CONFIG_N
#error "S-C : P(LWUSB_OPTS_NUM_CONFIGS) , Minimum of one Configuration is needed !!"
#endif

#if LWUSB_OPTS_NUM_INTFS < LWUSB_MIN_INTF_N
#error "S-C : P(LWUSB_OPTS_NUM_INTFS) , Minimum of one Interface is needed !!"
#endif

#if LWUSB_OPTS_NUM_EPS > LWUSB_MAX_EP_N
#error "S-C : P(LWUSB_OPTS_NUM_EPS) , Unsupported Number of EndPoints !!"
#endif

#if LWUSB_OPTS_NUM_CONFIGS > LWUSB_MAX_CONFIG_N
#error "S-C : P(LWUSB_OPTS_NUM_CONFIGS) , Unsupported Number of Configurations !!"
#endif

#if LWUSB_OPTS_NUM_INTFS > LWUSB_MAX_INTF_N
#error "S-C : P(LWUSB_OPTS_NUM_INTFS) , Unsupported Number of Interfaces !!"
#endif

#define LWUSB_OPTS_

#endif /* LWUSB_OPTS_H_ */
