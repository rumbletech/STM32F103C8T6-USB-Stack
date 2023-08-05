/*
 * mem.h
 *
 *  Created on: Jul 29, 2023
 *      Author: Garmoosh
 */

#ifndef MEM_H_
#define MEM_H_

#include "Include/lwUSB_Std.h"

typedef struct mem_s {

	uint8_t * mp;  // pointer to block
	uint32_t  ms;  // block size
	uint32_t  ap;  // allocation pointer

} mem , *mem_p;

void mem_init( struct mem_s * mem_p , uint8_t * bp , uint32_t bs );
BOOL mem_peek( struct mem_s * mem_p , uint32 peek_sz );
uint8_t * mem_alloc( struct mem_s * mem_p , uint32_t alloc_sz );

#endif /* MEM_H_ */
