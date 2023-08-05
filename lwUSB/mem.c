/* Author : mrashada
 * Date   : 7/29/2023
 * ***********************************************************************
 * ***********************************************************************
 * This Defines the Method of static Initialization in the lwUSB Stack.
 * ***********************************************************************
 * ***********************************************************************
 */

#include "mem.h"

void mem_init( struct mem_s * mem_p , uint8_t * bp , uint32_t bs ){

	mem_p->mp = bp ;
	mem_p->ms = bs ;
	mem_p->ap = 0ul;
}

BOOL mem_peek( struct mem_s * mem_p , uint32 peek_sz ){

	int32_t free = mem_p->ms - mem_p->ap ;
	if ( peek_sz > free ){
		return FALSE;
	}
	return TRUE;
}

uint8_t * mem_alloc( struct mem_s * mem_p , uint32_t alloc_sz ){

	if ( !mem_p->mp  ){
		return NULL ;
	}
	if ( !mem_peek(alloc_sz)){
		return NULL;
	}
	uint32_t retp = mem_p->ap;
	mem_p->ap += alloc_sz ;

	return &mem_p->mp[retp];
}
