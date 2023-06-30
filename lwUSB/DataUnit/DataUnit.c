/* Author : mrashada
 * ********************************
 * Abstract Type DataUnit .
 * ********************************
 */

#include "DataUnit.h"

#include <string.h>
#include <stddef.h>

#define DU_MEMCPY(DST,SRC,LEN) memcpy((void*)DST,(void*)SRC,(size_t)LEN)

/******************************  Maker functions  *************************/

struct DataInfo_s makeDataInfo ( uint16_t dataLength , uint16_t metaData ){
	struct DataInfo_s info ;
	info.length = dataLength ;
	info.meta = metaData ;
	return info ;
}

struct DataUnit_s makeDataUnit ( uint8_t * dataPtr ,
		                         struct DataInfo_s info){
	struct DataUnit_s unit ;
	unit.info = info ;
	unit.data = dataPtr ;
	return unit;
}

/******************************  PUSH/GET/COPY  *************************/

void pushDU( struct DataUnit_s du , uint8_t * mptr ){

	uint8_t * src = du.data ;
	uint8_t * dst = mptr ;
	uint16_t  datLen = du.info.length ;
	struct DataInfo_s * infoDst = (struct DataInfo_s *)mptr;

	mptr += sizeof( struct DataInfo_s );
	*(infoDst)  = du.info ;

	DU_MEMCPY(dst , src , datLen);
	return;
}

struct DataUnit_s getDU( uint8_t * dup , uint8_t * mptr ){

	uint8_t * src = mptr ;
	uint8_t * dst = dup ;
	struct DataInfo_s * infoSrc = (struct DataInfo_s *)mptr;
	struct DataInfo_s info ;

	mptr+= sizeof( struct DataInfo_s );
	info = *infoSrc ;

	DU_MEMCPY(dst , src , info.length);

	return makeDataUnit(dup,info);
}
