/*
 * RingBuffer.h
 *
 *  Created on: Jun 28, 2023
 *      Author: Garmoosh
 */

#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

/* naming convention */
/* _s stands for type struct struct_name_s     ---> struct_name_s
 * _p stands for struct struct_name_s *        ---> struct-name_p
 * struct_name is a typedef for struct_name_ s ---> struct_name
 */

typedef struct RingBuffer_s {

	/* Expose Internal */
	uint8_t * internBuffer_p ;
	uint32_t  internBufferSize_l ;
	/* (read/write) Pointers , Circular Implementation */
	uint32_t  read_p  ;
	uint32_t  write_p ;
	/* flags */
	uint8_t isFull ;
	uint8_t isEmpty ;

} RingBuffer , *RingBuffer_p ;

uint8_t RingBuffer_isFull( struct RingBuffer_s * dp );
uint8_t RingBuffer_isEmpty( struct RingBuffer_s * dp );
uint32_t RingBuffer_GetUsedSpace ( struct RingBuffer_s * dp  );
uint32_t RingBuffer_GetFreeSpace( struct RingBuffer_s * dp);
void RingBuffer_Init( struct RingBuffer_s * dp , uint8_t * pool_b , uint32_t pool_sz );
uint8_t RingBuffer_Push( struct RingBuffer_s * dp , uint8_t* du , uint32_t dl);
uint8_t RingBuffer_Get( struct RingBuffer_s * dp , uint8_t* du , uint32_t dl);

#endif /* RING_BUFFER_H_ */
