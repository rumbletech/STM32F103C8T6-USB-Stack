





#include <main.h>


/* RCC AND FLASH Configuration *************************************************************
 * *****************************************************************************************
 */
#define HSE_FREQ 8000000UL
#define PLL_MULL_FACTOR 9UL
#define PLL_MULL_V 7U
#define APB1_DIV2 4UL
#define APB2_DIV1 0UL
#define AHB_DIV1 0UL

#define PLL_SWITCH 2UL


/********************************************************************************************
 ********************************************************************************************/





s_lwGPIO_Config pc13 = { .conf  = e_lwGPIO_conf_PP ,
						 .mode  = e_lwGPIO_mode_output_10M ,
						 .pin_i = 13u ,
						 .lock  = 0u
};

s_lwGPIO_Config pa8 = {  .conf  = e_lwGPIO_conf_AF_PP ,
						 .mode  = e_lwGPIO_mode_output_50M ,
						 .pin_i = 8 ,
						 .lock  = 0u
};

s_lwGPIO_Config pa2 = {  .conf  = e_lwGPIO_conf_AF_PP ,
						 .mode  = e_lwGPIO_mode_output_10M ,
						 .pin_i = 8 ,
						 .lock  = 0u
};

s_lwGPIO_Config pa3 = {  .conf  = e_lwGPIO_conf_floating ,
						 .mode  = e_lwGPIO_mode_input ,
						 .pin_i = 8 ,
						 .lock  = 0u
};

s_lwUSART_Config usart2 = {
						     .baudrate     = 9600 ,
							 .clock_config = DONT_CARE ,
							 .clock_en     = DONT_CARE ,
							 .mode         = e_lwUSART_usart_mode_full_duplex ,
							 .parity       = e_lwUSART_usart_parity_nop ,
							 .stop_bits    = e_lwUSART_usart_stopbits_1sb ,
							 .word_len     = e_lwUSART_usart_wordlen_8
};

static void FLASH_Init( void )
{

	/*Insert two wait states and enable Prefetch buffer */
	FLASH->ACR = ( 2 << FLASH_ACR_LATENCY_Pos )|( FLASH_ACR_PRFTBE_Msk ) ;

}

static void RCC_Init( void )
{


	RCC->CR |= RCC_CR_HSEON_Msk ;
	/* Wait for HSE Flag */
	while( !(RCC_CR_HSERDY_Msk & RCC->CR  ) ) ;

	/* Configure PreScalers */

	RCC->CFGR = RCC_CFGR_PLLSRC_Msk | ( PLL_MULL_V << RCC_CFGR_PLLMULL_Pos ) |
			 	( AHB_DIV1 << RCC_CFGR_HPRE_Pos ) | ( APB1_DIV2 << RCC_CFGR_PPRE1_Pos ) |
				( APB2_DIV1 << RCC_CFGR_PPRE2_Pos )|( RCC_CFGR_MCOSEL_SYSCLK ) ;


	RCC->CR |= RCC_CR_PLLON_Msk  ;
	/* Wait for PLL Flag */
	while( !(RCC_CR_PLLRDY_Msk & RCC->CR  ) ) ;

	/*Switch to PLL */
	RCC->CFGR |= PLL_SWITCH  ; // SWITCH TO PLL

	// todo
	/* lw_init should be improved , USING PARAMS TO CALCULATE FREQ AT RUNTIME*/
	uint32_t sys_clk = PLL_MULL_FACTOR * HSE_FREQ ;
	lw_Init( PLL_MULL_FACTOR * HSE_FREQ  ,sys_clk/2 , sys_clk , ( sys_clk * 2 )/3 );

}



static void DebugUSART_Init( void )
{
	/* Configure PINS */
	lwGPIO_EnableGPIO(GPIOA);
	lwGPIO_Config( GPIOA , &pa2);
	lwGPIO_Config( GPIOA , &pa3 );
	/* Init Debug Port */
	lwUSART_Config(USART2, &usart2);


}
static void USB_Init( void )
{
	 lwUSB_Init();

}
static void GPIO_Init( void )
{


	lwGPIO_EnableGPIO(GPIOC);
	lwGPIO_EnableGPIO(GPIOA);

	lwGPIO_Config( GPIOC , &pc13 );
	lwGPIO_ResetPin(GPIOC, &pc13 );
	lwGPIO_Config( GPIOA , &pa8 );

}

void SysInit( void )
{

	FLASH_Init();
	RCC_Init();
	DebugUSART_Init();
	GPIO_Init();
	USB_Init();


}
