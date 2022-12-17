





#include <main.h>


/* RCC AND FLASH Configuration *************************************************************
 * *****************************************************************************************
 */
#define HSE_FREQ 8000000UL
#define PLL_MULL_FACTOR 9UL
#define APB1_DIV2 4UL
#define APB2_DIV1 0UL
#define AHB_DIV1 0UL

#define PLL_SWITCH 2UL


/********************************************************************************************
 ********************************************************************************************/



static uint32_t sys_clk ;
static uint32_t apb1_clk ;
static uint32_t apb2_clk ;
static uint32_t usb_clk ;



s_lwGPIO_Config pc13 = { .conf  = e_lwGPIO_conf_PP ,
						 .mode  = e_lwGPIO_mode_output_10M ,
						 .pin_i = 13u ,
						 .lock  = 0u
};

static void FLASH_Init( void )
{

	/*Insert two wait states and enable Prefetch buffer */
	FLASH->ACR = ( 2 << FLASH_ACR_LATENCY_Pos )|( FLASH_ACR_PRFTBE_Msk )| ( FLASH_ACR_HLFCYA_Msk ) ;
}

static void RCC_Init( void )
{

	/* Configure PreScalers */

	RCC->CFGR = RCC_CFGR_PLLSRC_Msk | ( (PLL_MULL_FACTOR) << RCC_CFGR_PLLMULL_Pos ) |
			 	( AHB_DIV1 << RCC_CFGR_HPRE_Pos ) | ( APB1_DIV2 << RCC_CFGR_PPRE1_Pos ) |
				( APB2_DIV1 << RCC_CFGR_PPRE2_Pos ) ;

	RCC->CR = RCC_CR_HSEON_Msk | RCC_CR_HSION_Msk | RCC_CR_PLLON_Msk ;


	/* Wait for HSE Flag */
	while( !(RCC_CR_HSERDY_Msk & RCC->CR  ) ) ;
	/* Wait for PLL Flag */
	while( !(RCC_CR_PLLRDY_Msk & RCC->CR  ) ) ;


	RCC->CR |= PLL_SWITCH  ; // SWITCH TO PLL


	sys_clk = PLL_MULL_FACTOR * HSE_FREQ ;
	apb1_clk = sys_clk/2 ;
	apb2_clk = sys_clk/1 ;
	usb_clk = ( sys_clk * 2 )/3 ;



}


void GPIO_Init( void )
{


	lwGPIO_EnableGPIO(GPIOC);
	lwGPIO_Config( GPIOC , &pc13 );
	lwGPIO_ResetPin(GPIOC, &pc13 );
}

void SysInit( void )
{

	FLASH_Init();
	RCC_Init();
	GPIO_Init();



}
