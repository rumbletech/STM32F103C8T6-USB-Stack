/*
 * DataPool.h
 *
 *  Created on: Jun 28, 2023
 *      Author: Garmoosh
 */

#ifndef DATAPOOL_DATAPOOL_H_
#define DATAPOOL_DATAPOOL_H_

/* STD */
#include "../DataUnit/DataUnit.h"
/* naming convention */
/* _s stands for type struct struct_name_s     ---> struct_name_s
 * _p stands for struct struct_name_s *        ---> struct-name_p
 * struct_name is a typedef for struct_name_ s ---> struct_name
 */

typedef struct DataPool_s {

	/* Expose Internal */
	uint8_t * internBuffer_p ;
	uint32_t  internBufferSize_l ;
	/* (read/write) Pointers , Circular Implementation */
	uint32_t  read_p  ;
	uint32_t  write_p ;
	/* flags */
	uint8_t isFull ;
	uint8_t isEmpty ;

} DataPool , *DataPool_p ;

uint8_t DataPool_isFull( struct DataPool_s * dp );
uint8_t DataPool_isEmpty( struct DataPool_s * dp );
uint32_t DataPool_GetUsedSpace ( struct DataPool_s * dp  );
uint32_t DataPool_GetFreeSpace( struct DataPool_s * dp);
void DataPool_Init( struct DataPool_s * dp , uint8_t * pool_b , size_t pool_sz );
void DataPool_Push( struct DataPool_s * dp , struct DataUnit_s du );
void DataPool_Get( struct DataPool_s * dp , struct DataUnit_s * du );

#endif /* DATAPOOL_DATAPOOL_H_ */
