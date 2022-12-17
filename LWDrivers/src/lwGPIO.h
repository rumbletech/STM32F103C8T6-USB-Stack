/*
 * lwGPIO.h
 *
 *  Created on: Dec 16, 2022
 *      Author: Garmoosh
 */

#ifndef SRC_LWGPIO_H_
#define SRC_LWGPIO_H_


#include "lwCommon.h"


#define E_LWGPIO_CONF_RNG 3U
#define E_LWGPIO_MODE_RNG 3U

typedef enum {

	e_lwGPIO_conf_analog    = 0b00 ,
	e_lwGPIO_conf_floating  = 0b01 ,
	e_lwGPIO_conf_input     = 0b10 ,
	e_lwGPIO_conf_PP        = 0b00 ,
	e_lwGPIO_conf_OD        = 0b01 ,
	e_lwGPIO_conf_AF_PP     = 0B10 ,
	e_lwGPIO_conf_AF_OD     = 0B11 ,

} e_lwGPIO_conf ;


typedef enum {

	e_lwGPIO_mode_input       = 0b00 ,
	e_lwGPIO_mode_output_10M  = 0b01 ,
	e_lwGPIO_mode_output_2M   = 0b10 ,
	e_lwGPIO_mode_output_50M  = 0b00 ,

} e_lwGPIO_mode ;

typedef struct str_lwGPIO_Config {

	e_lwGPIO_mode mode ;
	e_lwGPIO_conf conf ;
	uint8_t pin_i ;
	uint8_t lock ;

} s_lwGPIO_Config  ;



err_t lwGPIO_EnableGPIO ( GPIO_TypeDef* lwgpio  );
LW_INLINE err_t lwGPIO_WritePort( GPIO_TypeDef* lwgpio , uint16_t portv  );
LW_INLINE int32_t lwGPIO_ReadPort( GPIO_TypeDef* lwgpio  );
LW_INLINE err_t lwGPIO_SetPin( GPIO_TypeDef* lwgpio , s_lwGPIO_Config* lwgpio_cs );
LW_INLINE err_t lwGPIO_ResetPin( GPIO_TypeDef* lwgpio , s_lwGPIO_Config* lwgpio_cs  );
LW_INLINE int8_t  lwGPIO_ReadPin( GPIO_TypeDef* lwgpio , s_lwGPIO_Config* lwgpio_cs );
err_t lwGPIO_Config( GPIO_TypeDef* lwgpio , s_lwGPIO_Config* lwgpio_cs );
err_t lwGPIO_Lock(  GPIO_TypeDef* lwgpio  );
#endif /* SRC_LWGPIO_H_ */
