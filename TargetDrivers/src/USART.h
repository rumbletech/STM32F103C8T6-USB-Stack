/*
 * lwUSART.h
 *
 *  Created on: Dec 24, 2022
 *      Author: Garmoosh
 */

#ifndef SRC_USART_H_
#define SRC_USART_H_

#include <Common.h>



#define LWUSART_USART_WORDLEN_RANGE 1u
#define LWUSART_USART_PARITY_RANGE 3u
#define LWUSART_USART_MODE_RANGE 3u
#define LWUSART_USART_STOPBITS_RANGE 3u
#define LWUSART_USART_CLOCK_CONFIG_RANGE 3u
#define LWUSART_USART_CLOCK_EN_RANGE 1u
#define LWUSART_USART_SYNCH_RANGE 1u
#define LWUSART_USART_BAUDRATE_RANGE 4500000u



typedef enum {

	e_lwUSART_usart_wordlen_8 = 0u ,
	e_lwUSART_usart_wordlen_9 = 1u ,

} e_lwUSART_uasrt_wordlen;

typedef enum {

	e_lwUSART_usart_parity_nop  = 0b00 ,
	e_lwUSART_usart_parity_even = 0b10 ,
	e_lwUSART_usart_parity_odd  = 0b11


} e_lwUSART_usart_parity;

typedef enum {
	e_lwUSART_usart_mode_disabled       = 0b00 ,
	e_lwUSART_usart_mode_half_duplex_tx = 0b10 ,
	e_lwUSART_usart_mode_half_duplex_rx = 0b01 ,
	e_lwUSART_usart_mode_full_duplex    = 0b11


} e_lwUSART_usart_mode ;

typedef enum {

	e_lwUSART_usart_stopbits_1sb    = 0b00 ,
	e_lwUSART_usart_stopbits_0x5sb  = 0b01 ,
	e_lwUSART_usart_stopbits_2sb    = 0b10 ,
	e_lwUSART_usart_stopbits_1x5sb  = 0b11


} e_lwUSART_usart_stopbits;

typedef enum {

	e_lwUSART_usart_synch_synchronous    = 0b00 ,
	e_lwUSART_usart_synch_asynchronous   = 0b01

} e_lwUSART_usart_synch;

typedef enum {
	e_lwUSART_usart_clock_fe_lowp  = 0b00 ,
	e_lwUSART_usart_clock_se_lowp  = 0b01 ,
	e_lwUSART_usart_clock_fe_highp = 0b10 ,
	e_lwUSART_usart_clock_se_highp = 0b11

} e_lwUSART_usart_clock ;

typedef struct str_lwUSART_Config {

	uint32_t                  baudrate     ;
	e_lwUSART_uasrt_wordlen   word_len     ;
	e_lwUSART_usart_parity    parity       ;
	e_lwUSART_usart_stopbits  stop_bits    ;
	e_lwUSART_usart_mode      mode         ;
	e_lwUSART_usart_synch     clock_en     ;
	e_lwUSART_usart_clock     clock_config ;


} s_lwUSART_Config  ;


LW_INLINE err_t lwUSART_EnableUSART ( USART_TypeDef* lwusart  );
err_t lwUSART_Config( USART_TypeDef* lwusart , s_lwUSART_Config* lwusart_cs );
err_t lwUSART_AS_Send( USART_TypeDef* lwusart , uint8_t* dptr , int16_t len );
err_t lwUSART_AS_Receieve( USART_TypeDef* lwusart , uint8_t* dptr , int16_t len );



#endif /* SRC_USART_H_ */
