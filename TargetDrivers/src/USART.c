


#include <math.h>
#include <src/USART.h>

#define  UINT32_CAST_USART1  UINT32T_CAST(USART1)
#define	 UINT32_CAST_USART2  UINT32T_CAST(USART2)
#define	 UINT32_CAST_USART3  UINT32T_CAST(USART3)

err_t USART_EnableUSART ( USART_TypeDef* usart  ){

	uint32_t cast_usart = (uint32_t)usart ;
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
		targetPrintf("LW_DEBUG: Invalid_Param" ) ;
		return ERR_PARAM ;
	}
	return ERR_OK ;
}



static uint16_t USART_CalculateBaud( uint32_t p_freq ,  uint32_t baud  )
{
	uint32_t mantissa = 0 ;
	uint32_t fraction = 0 ;
	float fdiv = (float)p_freq / ( 16u * baud ) ;
	mantissa = (uint32_t) fdiv ;
	fraction = round ( (fdiv-mantissa ));
	return ( (mantissa << 4UL )|fraction);

}


err_t USART_Config( USART_TypeDef* usart , s_USART_Config* usart_cs ){

	USART_EnableUSART(usart);
	uint32_t pclk = 0 ;

	if ( UINT32T_CAST(usart) == UINT32_CAST_USART1 )
	{
		pclk = commonGetAPB2Clk() ;
	}
	else{
		pclk = commonGetAPB1Clk() ;
	}

	uint16_t baud = USART_CalculateBaud( pclk , usart_cs->baudrate );

	usart->BRR &= ~( USART_BRR_DIV_Fraction_Msk | USART_BRR_DIV_Mantissa_Msk );

	usart->BRR |= baud ;

	usart->CR2 &= ~( USART_CR2_CPHA_Msk | USART_CR2_CLKEN_Msk | USART_CR2_STOP_Msk ) ;

	usart->CR2 |= ((usart_cs->clock_config << USART_CR2_CPHA_Pos)  | (usart_cs->clock_en << USART_CR2_CLKEN_Pos)|
				   (usart_cs->stop_bits << USART_CR2_STOP_Pos ) );

	usart->CR1 &= ~( USART_CR1_UE_Msk | USART_CR1_TE_Msk | USART_CR1_RE_Msk | USART_CR1_M_Msk | USART_CR1_PS_Msk |
					   USART_CR1_PCE_Msk ) ;


	usart->CR1 |= ( USART_CR1_UE_Msk | ( usart_cs->mode << USART_CR1_RE_Pos ) | ( usart_cs->word_len << USART_CR1_M_Pos )|
					 ( usart_cs->parity << USART_CR1_PS_Pos ) );


	return ERR_OK ;
}


err_t USART_AS_Send( USART_TypeDef* usart , uint8_t* dptr , int16_t len   )
{

	for ( ; len-- ; )
	{
		while( !(usart->SR & (USART_SR_TXE_Msk)) );
		usart->DR = *(dptr++);
	}

	return ERR_OK ;

}

err_t USART_AS_Receieve( USART_TypeDef* usart , uint8_t* dptr , int16_t len    )
{

	for ( ; len-- ; )
	{
		while( !(usart->SR & (USART_SR_RXNE_Msk)) );
		*(dptr++) = usart->DR ;

	}

	return ERR_OK  ;

}


