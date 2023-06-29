/*
 * DataUnit.h
 *
 *  Created on: Jun 28, 2023
 *      Author: Garmoosh
 */

#ifndef DATAUNIT_DATAUNIT_H_
#define DATAUNIT_DATAUNIT_H_


/* STD */
#include <stdint.h>

typedef struct DataInfo_s {

	uint16_t meta    ;
	uint16_t length  ;

} DataInfo , *DataInfo_p ;


typedef struct DataUnit_s {

	struct DataInfo_s info ;
	uint8_t * data ;

} DataUnit , *DataUnit_p ;

struct DataInfo_s makeDataInfo ( uint16_t dataLength , uint16_t metaData );
struct DataUnit_s makeDataUnit ( uint8_t * dataPtr , struct DataInfo_s info);
void pushDataUnitToMemory( struct DataUnit_s du , uint8_t * mptr );
struct DataUnit_s getDataUnitFromMemory( uint8_t * dup , uint8_t * mptr );

#endif /* DATAUNIT_DATAUNIT_H_ */
