

#include "lwGPIO.h"




#define  UINT32_CAST_GPIOA  UINT32T_CAST(GPIOA)
#define	 UINT32_CAST_GPIOB  UINT32T_CAST(GPIOB)
#define	 UINT32_CAST_GPIOC  UINT32T_CAST(GPIOC)
#define	 UINT32_CAST_GPIOD  UINT32T_CAST(GPIOD)
#define	 UINT32_CAST_GPIOE  UINT32T_CAST(GPIOE)

#define ASSERT_GPIO_TYPDEF(GPIO_IN) LW_ASSERT( ( (UINT32T_CAST(GPIO_IN) == UINT32_CAST_GPIOA) || \
									  (UINT32T_CAST(GPIO_IN) == UINT32_CAST_GPIOB) || \
									  (UINT32T_CAST(GPIO_IN) == UINT32_CAST_GPIOC) || \
									  (UINT32T_CAST(GPIO_IN) == UINT32_CAST_GPIOD) || \
									  (UINT32T_CAST(GPIO_IN) == UINT32_CAST_GPIOE)  \
									  ) ) \

LW_INLINE err_t lwGPIO_EnableGPIO ( GPIO_TypeDef* lwgpio  ){
#if ( LW_ASSERT_ENABLE == 1U )
	ASSERT_GPIO_TYPDEF(lwgpio);
#endif
	uint32_t cast_gpio = (uint32_t)lwgpio ;
	switch (cast_gpio)
	{
	case UINT32_CAST_GPIOA :
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN_Msk ;
		break;
	case UINT32_CAST_GPIOB :
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN_Msk ;
		break;
	case UINT32_CAST_GPIOC :
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN_Msk ;
		break;
	case UINT32_CAST_GPIOD :
		RCC->APB2ENR |= RCC_APB2ENR_IOPDEN_Msk ;
		break;
	case UINT32_CAST_GPIOE :
		RCC->APB2ENR |= RCC_APB2ENR_IOPEEN_Msk ;
		break;
	default :
		LW_DEBUG( "LW_DEBUG: Invalid_Param" ) ;
		return ERR_PARAM ;
	}
	return ERR_OK ;
}


LW_INLINE err_t lwGPIO_WritePort( GPIO_TypeDef* lwgpio , uint16_t portv  ){
#if ( LW_ASSERT_ENABLE == 1U )
	LW_ASSERT( (portv&0xFFFF) == 0 );
	ASSERT_GPIO_TYPDEF(lwgpio);
#endif
	lwgpio->ODR = portv ;
	return ERR_OK ;
}

LW_INLINE int32_t lwGPIO_ReadPort( GPIO_TypeDef* lwgpio  ){
#if ( LW_ASSERT_ENABLE == 1U )
	ASSERT_GPIO_TYPDEF(lwgpio);
#endif
	return lwgpio->IDR&0xFFFF ;
}



LW_INLINE err_t lwGPIO_SetPin( GPIO_TypeDef* lwgpio , s_lwGPIO_Config* lwgpio_cs  ){
#if ( LW_ASSERT_ENABLE == 1U )
	LW_ASSERT( lwgpio_cs->pin_i <= 15u );
	ASSERT_GPIO_TYPDEF(lwgpio);
#endif
	lwgpio->BSRR |= ( 1u << lwgpio_cs->pin_i )  ;
	return ERR_OK ;
}

LW_INLINE err_t lwGPIO_ResetPin( GPIO_TypeDef* lwgpio , s_lwGPIO_Config* lwgpio_cs ){
#if ( LW_ASSERT_ENABLE == 1U )
	LW_ASSERT( lwgpio_cs->pin_i <= 15u );
	ASSERT_GPIO_TYPDEF(lwgpio);
#endif
	lwgpio->BRR |= ( 1u << lwgpio_cs->pin_i )  ;
	return ERR_OK ;
}

LW_INLINE int8_t lwGPIO_ReadPin( GPIO_TypeDef* lwgpio , s_lwGPIO_Config* lwgpio_cs  ){
#if ( LW_ASSERT_ENABLE == 1U )
	LW_ASSERT( lwgpio_cs->pin_i <= 15u );
	ASSERT_GPIO_TYPDEF(lwgpio);
#endif
	if( lwgpio->IDR & (1u << lwgpio_cs->pin_i )){
		return 1u ;
	}
	return 0u ;
}


err_t lwGPIO_Config( GPIO_TypeDef* lwgpio , s_lwGPIO_Config* lwgpio_cs ){
#if ( LW_ASSERT_ENABLE == 1U )
	LW_ASSERT( lwgpio_cs );
	LW_ASSERT( lwgpio_cs->mode <= E_LWGPIO_MODE_RNG);
	LW_ASSERT( lwgpio_cs->conf <= E_LWGPIO_CONF_RNG);
	LW_ASSERT( lwgpio_cs->pin_i <= 15u );
	LW_ASSERT( lwgpio_cs->lock <= 1u );
	ASSERT_GPIO_TYPDEF(lwgpio);
#endif

	uint8_t config_v = ((lwgpio_cs->mode) | (lwgpio_cs->conf << 2u));
	uint8_t shift_v = (lwgpio_cs->pin_i%8)*4u ;

	if ( lwgpio_cs->pin_i <= 7u ){
		lwgpio->CRL &= ~( 0xF <<  shift_v );
		lwgpio->CRL |= ( config_v << shift_v);
	}
	else{
		lwgpio->CRH &= ~( 0xF <<  shift_v );
		lwgpio->CRH |= ( config_v << shift_v );
	}

	lwgpio->LCKR |= (  lwgpio_cs->lock << lwgpio_cs->pin_i );
	return ERR_OK ;
}

err_t lwGPIO_Lock(  GPIO_TypeDef* lwgpio  ){

#if ( LW_ASSERT_ENABLE == 1U )
	ASSERT_GPIO_TYPDEF(lwgpio);
#endif
	if ( lwgpio->LCKR & GPIO_LCKR_LCKK_Msk ){
		return ERR_FAULT ;
	}
	/*Lock Sequence */
	uint32_t lockr = lwgpio->LCKR ;
	lwgpio->LCKR = (GPIO_LCKR_LCKK_Msk) | lockr ;
	lwgpio->LCKR = (~GPIO_LCKR_LCKK_Msk) & lockr ;
	lwgpio->LCKR = (GPIO_LCKR_LCKK_Msk) | lockr ;
	lockr = lwgpio->LCKR ;
	if ( lockr&GPIO_LCKR_LCKK_Msk ){
		return ERR_FAULT ;
	}
	lockr = lwgpio->LCKR ;
	if ( !(lockr&GPIO_LCKR_LCKK_Msk) ){
		return ERR_FAULT ;
	}

	return ERR_OK ;
}
