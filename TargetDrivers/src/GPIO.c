

#include <src/GPIO.h>

#define  UINT32_CAST_GPIOA  UINT32T_CAST(GPIOA)
#define	 UINT32_CAST_GPIOB  UINT32T_CAST(GPIOB)
#define	 UINT32_CAST_GPIOC  UINT32T_CAST(GPIOC)
#define	 UINT32_CAST_GPIOD  UINT32T_CAST(GPIOD)
#define	 UINT32_CAST_GPIOE  UINT32T_CAST(GPIOE)

err_t GPIO_EnableGPIO ( GPIO_TypeDef* gpio  ){

	uint32_t cast_gpio = (uint32_t)gpio ;
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
		targetPrintf( "LW_DEBUG: Invalid_Param" ) ;
		return ERR_PARAM ;
	}
	return ERR_OK ;
}


err_t GPIO_WritePort( GPIO_TypeDef* gpio , uint16_t portv  ){

	gpio->ODR = portv ;
	return ERR_OK ;
}

int32_t GPIO_ReadPort( GPIO_TypeDef* gpio  ){

	return gpio->IDR&0xFFFF ;
}



err_t GPIO_SetPin( GPIO_TypeDef* gpio , s_GPIO_Config* gpio_cs  ){

	gpio->BSRR |= ( 1u << gpio_cs->pin_i )  ;
	return ERR_OK ;
}

err_t GPIO_ResetPin( GPIO_TypeDef* gpio , s_GPIO_Config* gpio_cs ){

	gpio->BRR |= ( 1u << gpio_cs->pin_i )  ;
	return ERR_OK ;
}

int8_t GPIO_ReadPin( GPIO_TypeDef* gpio , s_GPIO_Config* gpio_cs  ){

	if( gpio->IDR & (1u << gpio_cs->pin_i )){
		return 1u ;
	}
	return 0u ;
}


err_t GPIO_Config( GPIO_TypeDef* gpio , s_GPIO_Config* gpio_cs ){

	uint8_t config_v = ((gpio_cs->mode) | (gpio_cs->conf << 2u));
	uint8_t shift_v = (gpio_cs->pin_i%8)*4u ;

	if ( gpio_cs->pin_i <= 7u ){
		gpio->CRL &= ~( 0xF <<  shift_v );
		gpio->CRL |= ( config_v << shift_v);
	}
	else{
		gpio->CRH &= ~( 0xF <<  shift_v );
		gpio->CRH |= ( config_v << shift_v );
	}

	gpio->LCKR |= (  gpio_cs->lock << gpio_cs->pin_i );
	return ERR_OK ;
}

err_t GPIO_Lock(  GPIO_TypeDef* gpio  ){

	if ( gpio->LCKR & GPIO_LCKR_LCKK_Msk ){
		return ERR_FAULT ;
	}
	/*Lock Sequence */
	uint32_t lockr = gpio->LCKR ;
	gpio->LCKR = (GPIO_LCKR_LCKK_Msk) | lockr ;
	gpio->LCKR = (~GPIO_LCKR_LCKK_Msk) & lockr ;
	gpio->LCKR = (GPIO_LCKR_LCKK_Msk) | lockr ;
	lockr = gpio->LCKR ;
	if ( lockr&GPIO_LCKR_LCKK_Msk ){
		return ERR_FAULT ;
	}
	lockr = gpio->LCKR ;
	if ( !(lockr&GPIO_LCKR_LCKK_Msk) ){
		return ERR_FAULT ;
	}

	return ERR_OK ;
}
