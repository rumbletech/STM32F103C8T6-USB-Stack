/* Author : mrashada
 * Date   : 22/12/2023
 * ***********************************************************************
 * ***********************************************************************
 * This defines a linear buffer element with lock/unlock bits.
 * ***********************************************************************
 * ***********************************************************************
 */


#include "linearbuffer.h"
#include "string.h"

void LinearBuffer_Init( struct LinearBuffer_s * lbs , uint8_t * dp , uint32_t size ){
	lbs->dp = dp;
	lbs->size = size;
	lbs->wrp = 0u;
	lbs->rdp = 0u;
}


void LinearBuffer_Reset( struct LinearBuffer_s * lbs ){
	lbs->wrp = 0u;
	lbs->rdp = 0u;
}

void LinearBuffer_Push( struct LinearBuffer_s * lbs , uint8_t * src , uint32_t len ){
	memcpy(lbs->dp , src , len);
	lbs->wrp += len;
	return;
}

void LinearBuffer_Get( struct LinearBuffer_s * lbs , uint8_t * dest , uint32_t len ){
	memcpy(dest , lbs->dp , len);
	return;
}
