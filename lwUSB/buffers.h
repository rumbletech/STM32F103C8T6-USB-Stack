/*
 * buffers.h
 *
 *  Created on: Jul 29, 2023
 *      Author: Garmoosh
 */

#ifndef BUFFERS_H_
#define BUFFERS_H_

#if LWUSB_OPTS_NUM_EPS > 0u

static uint8_t ep0_tb[LWUSB_OPTS_EP0_TX_B_SIZE];
static uint8_t ep0_rb[LWUSB_OPTS_EP0_RX_B_SIZE];

#endif

#if LWUSB_OPTS_NUM_EPS > 1u

static uint8_t ep1_tb[LWUSB_OPTS_EP1_TX_B_SIZE];
static uint8_t ep1_rb[LWUSB_OPTS_EP1_RX_B_SIZE];

#endif

#if LWUSB_OPTS_NUM_EPS > 2u

static uint8_t ep2_tb[LWUSB_OPTS_EP2_TX_B_SIZE];
static uint8_t ep2_rb[LWUSB_OPTS_EP2_RX_B_SIZE];

#endif

#if LWUSB_OPTS_NUM_EPS > 3u

static uint8_t ep3_tb[LWUSB_OPTS_EP3_TX_B_SIZE];
static uint8_t ep3_rb[LWUSB_OPTS_EP3_RX_B_SIZE];

#endif

#if LWUSB_OPTS_NUM_EPS > 4u

static uint8_t ep4_tb[LWUSB_OPTS_EP4_TX_B_SIZE];
static uint8_t ep4_rb[LWUSB_OPTS_EP4_RX_B_SIZE];

#endif

#if LWUSB_OPTS_NUM_EPS > 5u

static uint8_t ep5_tb[LWUSB_OPTS_EP5_TX_B_SIZE];
static uint8_t ep5_rb[LWUSB_OPTS_EP5_RX_B_SIZE];

#endif

#if LWUSB_OPTS_NUM_EPS > 6u

static uint8_t ep6_tb[LWUSB_OPTS_EP6_TX_B_SIZE];
static uint8_t ep6_rb[LWUSB_OPTS_EP6_RX_B_SIZE];

#endif

#if LWUSB_OPTS_NUM_EPS > 7u

static uint8_t ep7_tb[LWUSB_OPTS_EP7_TX_B_SIZE];
static uint8_t ep7_rb[LWUSB_OPTS_EP7_RX_B_SIZE];

#endif

static uint8_t * bufp_r[LWUSB_MAX_EP_N] = {

#if LWUSB_OPTS_NUM_EPS > 0u
	ep0_rb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 1u
	ep1_rb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 2u
	ep2_rb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 3u
	ep3_rb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 4u
	ep4_rb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 5u
	ep5_rb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 6u
	ep6_rb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 7u
	ep7_rb
#else
	NULL
#endif

};

static uint8_t * bufp_t[LWUSB_MAX_EP_N] = {

#if LWUSB_OPTS_NUM_EPS > 0u
	ep0_tb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 1u
	ep1_tb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 2u
	ep2_tb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 3u
	ep3_tb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 4u
	ep4_tb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 5u
	ep5_tb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 6u
	ep6_tb
#else
	NULL
#endif
	,
#if LWUSB_OPTS_NUM_EPS > 7u
	ep7_tb
#else
	NULL
#endif

};


#endif /* BUFFERS_H_ */
