/*
 * USART.h
 *
 *  Created on: Dec 24, 2022
 *      Author: Garmoosh
 */

#ifndef SRC_USART_H_
#define SRC_USART_H_

#include <Common.h>

typedef enum {

	e_USART_usart_wordlen_8 = 0u ,
	e_USART_usart_wordlen_9 = 1u ,

} e_USART_uasrt_wordlen;

typedef enum {

	e_USART_usart_parity_nop  = 0b00 ,
	e_USART_usart_parity_even = 0b10 ,
	e_USART_usart_parity_odd  = 0b11


} e_USART_usart_parity;

typedef enum {
	e_USART_usart_mode_disabled       = 0b00 ,
	e_USART_usart_mode_half_duplex_tx = 0b10 ,
	e_USART_usart_mode_half_duplex_rx = 0b01 ,
	e_USART_usart_mode_full_duplex    = 0b11


} e_USART_usart_mode ;

typedef enum {

	e_USART_usart_stopbits_1sb    = 0b00 ,
	e_USART_usart_stopbits_0x5sb  = 0b01 ,
	e_USART_usart_stopbits_2sb    = 0b10 ,
	e_USART_usart_stopbits_1x5sb  = 0b11


} e_USART_usart_stopbits;

typedef enum {

	e_USART_usart_synch_synchronous    = 0b00 ,
	e_USART_usart_synch_asynchronous   = 0b01

} e_USART_usart_synch;

typedef enum {
	e_USART_usart_clock_fe_lowp  = 0b00 ,
	e_USART_usart_clock_se_lowp  = 0b01 ,
	e_USART_usart_clock_fe_highp = 0b10 ,
	e_USART_usart_clock_se_highp = 0b11

} e_USART_usart_clock ;

typedef struct str_USART_Config {

	uint32_t                  baudrate     ;
	e_USART_uasrt_wordlen   word_len     ;
	e_USART_usart_parity    parity       ;
	e_USART_usart_stopbits  stop_bits    ;
	e_USART_usart_mode      mode         ;
	e_USART_usart_synch     clock_en     ;
	e_USART_usart_clock     clock_config ;


} s_USART_Config  ;


err_t USART_EnableUSART ( USART_TypeDef* usart  );
err_t USART_Config( USART_TypeDef* usart , s_USART_Config* usart_cs );
err_t USART_AS_Send( USART_TypeDef* usart , uint8_t* dptr , int16_t len );
err_t USART_AS_Receieve( USART_TypeDef* usart , uint8_t* dptr , int16_t len );

#endif /* SRC_USART_H_ */
