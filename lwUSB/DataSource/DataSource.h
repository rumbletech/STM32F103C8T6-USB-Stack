/*
 * DataSource.h
 *
 *  Created on: Jun 28, 2023
 *      Author: Garmoosh
 */

#ifndef DATASOURCE_DATASOURCE_H_
#define DATASOURCE_DATASOURCE_H_

/* STD */
#include "../DataUnit/DataUnit.h"

#define DATASOURCE_MIN_BUFF_SIZE 256u

/* naming convention */
/* _s stands for type struct struct_name_s     ---> struct_name_s
 * _p stands for struct struct_name_s *        ---> struct-name_p
 * struct_name is a typedef for struct_name_ s ---> struct_name
 */

typedef struct DataSource_s {

	/* Expose Internal */
	uint8_t * internBuffer_p ;
	uint32_t  internBufferSize_l ;
	uint8_t   internBuffer_b[DATASOURCE_MIN_BUFF_SIZE];
	/* (read/write) Pointers , Circular Implementation */
	uint8_t*  read_p  ;
	uint8_t*  write_p ;
	/* dataUnit Info */
	uint8_t  dataInfo_sz;
	uint8_t  dataUnit_sz;

} DataSource , *DataSource_p ;


#endif /* DATASOURCE_DATASOURCE_H_ */
