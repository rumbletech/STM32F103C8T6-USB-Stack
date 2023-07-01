/*
 * GPIO.h
 *
 *  Created on: Dec 16, 2022
 *      Author: Garmoosh
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#include <Common.h>

typedef enum {

	e_GPIO_conf_analog     = 0b00 ,
	e_GPIO_conf_floating   = 0b01 ,
	e_GPIO_conf_input_pupd = 0b10 ,

	e_GPIO_conf_PP         = 0b00 ,
	e_GPIO_conf_OD         = 0b01 ,
	e_GPIO_conf_AF_PP      = 0B10 ,
	e_GPIO_conf_AF_OD      = 0B11 ,

} e_GPIO_conf ;


typedef enum {

	e_GPIO_mode_input       = 0b00 ,
	e_GPIO_mode_output_10M  = 0b01 ,
	e_GPIO_mode_output_2M   = 0b10 ,
	e_GPIO_mode_output_50M  = 0b11 ,

} e_GPIO_mode ;

typedef struct str_GPIO_Config {

	e_GPIO_mode mode ;
	e_GPIO_conf conf ;
	uint8_t pin_i ;
	uint8_t lock ;

} s_GPIO_Config  ;

err_t GPIO_EnableGPIO ( GPIO_TypeDef* gpio  );
err_t GPIO_WritePort( GPIO_TypeDef* gpio , uint16_t portv  );
int32_t GPIO_ReadPort( GPIO_TypeDef* gpio  );
err_t GPIO_SetPin( GPIO_TypeDef* gpio , s_GPIO_Config* gpio_cs );
err_t GPIO_ResetPin( GPIO_TypeDef* gpio , s_GPIO_Config* gpio_cs  );
int8_t  GPIO_ReadPin( GPIO_TypeDef* gpio , s_GPIO_Config* gpio_cs );
err_t GPIO_Config( GPIO_TypeDef* gpio , s_GPIO_Config* gpio_cs );
err_t GPIO_Lock(  GPIO_TypeDef* gpio  );

#endif /* SRC_GPIO_H_ */
