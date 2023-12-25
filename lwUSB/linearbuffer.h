/*
 * linearbuffer.h
 *
 *  Created on: Dec 22, 2023
 *      Author: Garmoosh
 */

#ifndef LINEARBUFFER_H_
#define LINEARBUFFER_H_

#include "Include/lwUSB_Std.h"

struct LinearBuffer_s {
	uint8_t * dp;
	uint32_t wrp;
	uint32_t rdp;
	uint32_t size;
};

void LinearBuffer_Init( struct LinearBuffer_s * lbs , uint8_t * dp , uint32_t size );
void LinearBuffer_Reset( struct LinearBuffer_s * lbs );
void LinearBuffer_Push( struct LinearBuffer_s * lbs , uint8_t * src , uint32_t len );
void LinearBuffer_Get( struct LinearBuffer_s * lbs , uint8_t * dest , uint32_t len );

#endif /* LINEARBUFFER_H_ */
