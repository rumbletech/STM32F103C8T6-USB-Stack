/*
 * tree.h
 *
 *  Created on: Sep 3, 2023
 *      Author: mrashada
 */


typedef enum {
	e_handle_type_endpoint = 0x01,
	e_handle_type_phy_endpoint = 0x02,
	e_handle_type_interface = 0x03,
	e_handle_type_configuration = 0x04,
	e_handle_type_string = 0x05,
	e_handle_type_device = 0x06
} htype ;

typedef struct {
	htype ht;
	void * hc;
}*Handle;

typedef struct Configuration_Inf_s {

	uint8_t cfg_value;
	uint8_t cfg_maxPower;
	boolean cfg_selfPowered;
	boolean cfg_remoteWakeUp;

}Configuration_Inf ;

typedef struct Interface_Inf_s {

	uint8_t itf_class;
	uint8_t itf_subClass;
	uint8_t itf_protocol;

}Interface_Inf ;

typedef struct String_Inf_s {

	uint8_t * str_content;
	uint8_t str_id;
	uint8_t str_encoding;
	uint8_t str_length;

}String_Inf ;

typedef struct EndPoint_Inf_s {


}EndPoint_Inf;

typedef struct Phy_Inf_s {


}Phy_Inf;


Handle createHandle( htype t , void* c );
