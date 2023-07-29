/* Author : mrashada
 * Date   : 7/29/2023
 * ***********************************************************************
 * ***********************************************************************
 * This Defines the Method of static Initialization in the lwUSB Stack.
 * ***********************************************************************
 * ***********************************************************************
 */

#include "mem.h"
#include "Include/lwUSB_Opts.h"

static uint8_t  buffm[LWUSB_BUFF_M_S];

void mem_Init( struct mem_s * mem_p , uint8_t * bp , uint32_t bs ){

	mem_p->mp = bp ;
	mem_p->ms = bs ;
	mem_p->ap = 0ul;
}


uint8_t * mem_alloc( struct mem_s * mem_p , uint32_t alloc_sz ){

	int32_t free = mem_p->ms - mem_p->ap ;
	if ( !mem_p->mp ||
		 alloc_sz > free ){
		return NULL ;
	}
	uint32_t retp = mem_p->ap;
	mem_p->ap += alloc_sz ;

	return &mem_p->mp[retp];
}

