/* Author : mrashada
 * Date   : 6/28/2023
 * ***********************************************************************
 * ***********************************************************************
 * This Defines an Abstract Type *** Data Pool ****
 * ***********************************************************************
 * This file Describes a generic Pool of Data used to interface Data to/from the stack
 * - a Data Pool is a POOL OF DATA .
 * - a Data Pool Implements a ring buffer.
 * - a Data Pool supports data Units as Push/Get.
 * ***********************************************************************
 */
#include <stddef.h>
#include "DataPool.h"

#include "../Include/lwUSB_Debug.h"


#if defined(DP_DEBUG_EN) && DP_DEBUG_EN != 0u
#define DP_PRINT(STR)
#else
#define DP_PRINT(STR) (void*)(STR)
#endif


static void increment_wrp( struct DataPool_s * dp ){
	dp->write_p++;
	dp->write_p %= dp->internBufferSize_l;
	dp->isEmpty = 0u ;
	if ( dp->write_p == dp->read_p ){
		dp->isFull = 1u;
	}
}

static void increment_rdp( struct DataPool_s * dp ){
	dp->read_p++;
	dp->read_p %= dp->internBufferSize_l;
	dp->isFull = 0u ;
	if ( dp->read_p == dp->write_p ){
		dp->isEmpty = 1u;
	}
}

uint8_t DataPool_isEmpty( struct DataPool_s * dp ){
	return dp->isEmpty ;
}

uint8_t DataPool_isFull( struct DataPool_s * dp ){
	return dp->isFull ;
}

static void write( struct DataPool_s * dp , uint8_t * src , uint32_t length ){
	uint32_t i = 0 ;
	for ( ; i < length && !DataPool_isFull(dp) ; i++ ){
		dp->internBuffer_p[dp->write_p] = src[i] ;
		increment_wrp(dp);
	}
	if ( i != length ){
		DP_PRINT("DP : WRF\r\n");
	}
	return ;
}

static void read( struct DataPool_s * dp , uint8_t *dst , uint32_t length ){
	uint32_t i = 0 ;
	for (; i < length && !DataPool_isEmpty(dp) ; i++ ){
		dst[i] = dp->internBuffer_p[dp->read_p];
		increment_rdp(dp);
	}
	if ( i != length ){
		DP_PRINT("DP : RDF\r\n");
	}
	return ;
}

uint32_t DataPool_GetFreeSpace( struct DataPool_s * dp){
	uint32_t writep = dp->write_p ;
	uint32_t readp = dp->read_p ;
	uint32_t length = dp->internBufferSize_l ;
	if ( writep < readp ){
		return readp-writep ;
	}
	return length - writep + readp ;
}

uint32_t DataPool_GetUsedSpace ( struct DataPool_s * dp  ){
	return dp->internBufferSize_l - DataPool_GetFreeSpace(dp);
}

void DataPool_Init( struct DataPool_s * dp , uint8_t * pool_b , uint32_t pool_sz ){

	if ( dp == NULL || pool_b == NULL ){
		DP_PRINT("DP: NULL d\r\n");
		return;
	}
	dp->internBuffer_p = pool_b;
	dp->internBufferSize_l = pool_sz;
	dp->read_p  = 0ul;
	dp->write_p = 0ul;
	dp->isFull  = 0ul;
	dp->isEmpty = 1ul;
	return ;
}

uint8_t DataPool_Push( struct DataPool_s * dp , struct DataUnit_s du ){

	uint32_t freeSpace = DataPool_GetFreeSpace(dp);
	uint32_t totalSize = du.info.length + sizeof( struct DataInfo_s );
	if ( totalSize > freeSpace ){
		DP_PRINT("DP: > d\r\n");
		return 0u;
	}
	write( dp , (uint8_t*)&du.info , sizeof(struct DataInfo_s) );
	write( dp , du.data , du.info.length );
	return 1u;
}

uint8_t DataPool_Get( struct DataPool_s * dp , struct DataUnit_s * du ){

	uint32_t usedSpace = DataPool_GetUsedSpace(dp);
	uint32_t totalSize = sizeof(struct DataInfo_s) ;

	if ( usedSpace < totalSize ){
		DP_PRINT("DP: < d\r\n");
		return 0u;
	}
	read( dp , (uint8_t*)&du->info , sizeof(struct DataInfo_s) );
	usedSpace = DataPool_GetUsedSpace(dp);
	totalSize = du->info.length ;

	if ( usedSpace < totalSize ){
		DP_PRINT("DP: < d\r\n");
		return 0u;
	}
	read( dp , du->data , du->info.length );

	return 1u;
}
