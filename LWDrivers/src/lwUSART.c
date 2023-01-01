


#include "lwUSART.h"
#include <math.h>



#define  UINT32_CAST_USART1  UINT32T_CAST(USART1)
#define	 UINT32_CAST_USART2  UINT32T_CAST(USART2)
#define	 UINT32_CAST_USART3  UINT32T_CAST(USART3)



#define ASSERT_USART_TYPDEF(USART_IN) LW_ASSERT(( (UINT32T_CAST(USART_IN) == UINT32_CAST_USART1) || \
									  (UINT32T_CAST(USART_IN) == UINT32_CAST_USART2) || \
									  (UINT32T_CAST(USART_IN) == UINT32_CAST_USART3)  \
									  )) \



LW_INLINE err_t lwUSART_EnableUSART ( USART_TypeDef* lwusart  ){
#if ( LW_ASSERT_ENABLE == 1U )
	ASSERT_USART_TYPDEF(lwusart);
#endif
	uint32_t cast_usart = (uint32_t)lwusart ;
	switch (cast_usart)
	{
	case UINT32_CAST_USART1 :
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN_Msk ;
		break;
	case UINT32_CAST_USART2 :
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN_Msk ;
		break;
	case UINT32_CAST_USART3 :
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN_Msk ;
		break;
	default :
		LW_DEBUG("LW_DEBUG: Invalid_Param" ) ;
		return ERR_PARAM ;
	}
	return ERR_OK ;
}



static uint16_t lwUSART_CalculateBaud( uint32_t p_freq ,  uint32_t baud  )
{
	uint32_t mantissa = 0 ;
	uint32_t fraction = 0 ;
	float fdiv = (float)p_freq / ( 16u * baud ) ;
	mantissa = (uint32_t) fdiv ;
	fraction = round ( (fdiv-mantissa ));
	return ( (mantissa << 4UL )|fraction);

}


err_t lwUSART_Config( USART_TypeDef* lwusart , s_lwUSART_Config* lwusart_cs ){
#if ( LW_ASSERT_ENABLE == 1U )
	LW_ASSERT( lwusart_cs );
	LW_ASSERT( lwusart_cs->mode <= LWUSART_USART_MODE_RANGE);
	LW_ASSERT( lwusart_cs->word_len <= LWUSART_USART_WORDLEN_RANGE);
	LW_ASSERT( lwusart_cs->parity <= LWUSART_USART_PARITY_RANGE );
	LW_ASSERT( lwusart_cs->stop_bits <= LWUSART_USART_STOPBITS_RANGE );
	LW_ASSERT( lwusart_cs->baudrate <= LWUSART_USART_BAUDRATE_RANGE );
	LW_ASSERT( lwusart_cs->clock_en <= LWUSART_USART_CLOCK_EN_RANGE );
	LW_ASSERT( lwusart_cs->clock_config <= LWUSART_USART_CLOCK_CONFIG_RANGE );
	ASSERT_USART_TYPDEF(lwusart);
#endif

	lwUSART_EnableUSART(lwusart);
	uint32_t pclk = 0 ;

	if ( UINT32T_CAST(lwusart) == UINT32_CAST_USART1 )
	{
		pclk = lw_getAPB2Clk() ;
	}
	else{
		pclk = lw_getAPB1Clk() ;
	}

	uint16_t baud = lwUSART_CalculateBaud( pclk , lwusart_cs->baudrate );

	lwusart->BRR &= ~( USART_BRR_DIV_Fraction_Msk | USART_BRR_DIV_Mantissa_Msk );

	lwusart->BRR |= baud ;

	lwusart->CR2 &= ~( USART_CR2_CPHA_Msk | USART_CR2_CLKEN_Msk | USART_CR2_STOP_Msk ) ;

	lwusart->CR2 |= ((lwusart_cs->clock_config << USART_CR2_CPHA_Pos)  | (lwusart_cs->clock_en << USART_CR2_CLKEN_Pos)|
				   (lwusart_cs->stop_bits << USART_CR2_STOP_Pos ) );

	lwusart->CR1 &= ~( USART_CR1_UE_Msk | USART_CR1_TE_Msk | USART_CR1_RE_Msk | USART_CR1_M_Msk | USART_CR1_PS_Msk |
					   USART_CR1_PCE_Msk ) ;


	lwusart->CR1 |= ( USART_CR1_UE_Msk | ( lwusart_cs->mode << USART_CR1_RE_Pos ) | ( lwusart_cs->word_len << USART_CR1_M_Pos )|
					 ( lwusart_cs->parity << USART_CR1_PS_Pos ) );


	return ERR_OK ;
}


err_t lwUSART_AS_Send( USART_TypeDef* lwusart , uint8_t* dptr , int16_t len   )
{

	for ( ; len-- ; )
	{
		while( !(lwusart->SR & (USART_SR_TXE_Msk)) );
		lwusart->DR = *(dptr++);
	}

	return ERR_OK ;

}

err_t lwUSART_AS_Receieve( USART_TypeDef* lwusart , uint8_t* dptr , int16_t len    )
{

	for ( ; len-- ; )
	{
		while( !(lwusart->SR & (USART_SR_RXNE_Msk)) );
		*(dptr++) = lwusart->DR ;

	}

	return ERR_OK  ;

}


