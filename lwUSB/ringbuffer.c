/* Author : mrashada
 * Date   : 6/28/2023
 * ***********************************************************************
 * ***********************************************************************
 * This Defines an Abstract Type *** Circular Buffer ****
 * ***********************************************************************
 * This Implements a Simple Ring/Circular Buffer.
 * ***********************************************************************
 */
#include "ringbuffer.h"

#include <stddef.h>
#include "Include/lwUSB_Debug.h"
#include "memctrl.h"


#if defined(DP_DEBUG_EN) && DP_DEBUG_EN != 0u
#define RB_PRINT(STR)
#else
#define RB_PRINT(STR) (void*)(STR)
#endif


static void increment_wrp( struct RingBuffer_s * rb ){
	rb->write_p++;
	rb->write_p %= rb->internBufferSize_l;
	rb->isEmpty = 0u ;
	if ( rb->write_p == rb->read_p ){
		rb->isFull = 1u;
	}
}

static void increment_rrb( struct RingBuffer_s * rb ){
	rb->read_p++;
	rb->read_p %= rb->internBufferSize_l;
	rb->isFull = 0u ;
	if ( rb->read_p == rb->write_p ){
		rb->isEmpty = 1u;
	}
}

uint8_t RingBuffer_isEmpty( struct RingBuffer_s * rb ){
	return rb->isEmpty ;
}

uint8_t RingBuffer_isFull( struct RingBuffer_s * rb ){
	return rb->isFull ;
}
uint32_t RingBuffer_GetWritePointer( struct RingBuffer_s * rb ){
	return rb->write_p;
}
static void write( struct RingBuffer_s * rb , uint8_t * src , uint32_t length ){
	uint32_t i = 0 ;
	for ( ; i < length && !RingBuffer_isFull(rb) ; i++ ){
		rb->internBuffer_p[rb->write_p] = src[i] ;
		increment_wrp(rb);
	}
	if ( i != length ){
		RB_PRINT("DP : WRF\r\n");
	}
	return ;
}

static void read( struct RingBuffer_s * rb , uint8_t *dst , uint32_t length ){
	uint32_t i = 0 ;
	for (; i < length && !RingBuffer_isEmpty(rb) ; i++ ){
		dst[i] = rb->internBuffer_p[rb->read_p];
		increment_rrb(rb);
	}
	if ( i != length ){
		RB_PRINT("DP : RDF\r\n");
	}
	return ;
}

uint32_t RingBuffer_GetFreeSpace( struct RingBuffer_s * rb){
	uint32_t writep = rb->write_p ;
	uint32_t rearb = rb->read_p ;
	uint32_t length = rb->internBufferSize_l ;
	if ( writep < rearb ){
		return rearb-writep ;
	}
	return length - writep + rearb ;
}

uint32_t RingBuffer_GetUsedSpace ( struct RingBuffer_s * rb  ){
	return rb->internBufferSize_l - RingBuffer_GetFreeSpace(rb);
}

void RingBuffer_Init( struct RingBuffer_s * rb , uint8_t * pool_b , uint32_t pool_sz ){

	if ( rb == NULL || pool_b == NULL ){
		RB_PRINT("DP: NULL d\r\n");
		return;
	}
	rb->internBuffer_p = pool_b;
	rb->internBufferSize_l = pool_sz;
	rb->read_p  = 0ul;
	rb->write_p = 0ul;
	rb->isFull  = 0ul;
	rb->isEmpty = 1ul;
	return ;
}

uint8_t RingBuffer_Push( struct RingBuffer_s * rb , uint8_t* du , uint32_t dl){

	uint32_t freeSpace = RingBuffer_GetFreeSpace(rb);
	if ( dl > freeSpace ){
		RB_PRINT("DP: > d\r\n");
		return 0u;
	}
	write( rb , du , dl);
	return 1u;
}

uint8_t RingBuffer_Get( struct RingBuffer_s * rb , uint8_t* du , uint32_t dl){

	uint32_t usedSpace = RingBuffer_GetUsedSpace(rb);
	if ( usedSpace < dl ){
		RB_PRINT("DP: < d\r\n");
		return 0u;
	}
	read( rb , du , dl );
	return 1u;
}
