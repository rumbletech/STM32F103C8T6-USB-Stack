/*
 * memctrl.h
 *
 *  Created on: Aug 4, 2023
 *      Author: Garmoosh
 */

#ifndef MEMCTRL_H_
#define MEMCTRL_H_

#include "stdint.h"

typedef enum e_mem_type {

	e_mem_type_buffer_mem ,    /* Defines an Area of Memory that is used for Buffer Allocation */
	e_mem_type_discriptor_mem, /* Defines an Area of Memory that is used for USB Descriptor Allocation */
	e_mem_type_var_mem,        /* Defines an Area of Memory that is used for control structures Allocation */

};
void memctrl_init ( void );
void * memctrl_alloc( uint32_t bSize  , enum e_mem_type mt );

#endif /* MEMCTRL_H_ */
