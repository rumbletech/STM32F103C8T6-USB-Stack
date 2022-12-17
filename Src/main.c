


#include <main.h>


extern s_lwGPIO_Config pc13 ;
extern s_lwUSART_Config usart2 ;
extern s_lwGPIO_Config pa8 ;
extern s_lwGPIO_Config pa2 ;
extern s_lwGPIO_Config pa3 ;



int main(void)
{





	while(1)
	{
		//todo figure out the frequency of the running clock as it doesn't make sense if it 72 Mhz
		// with the current called delay and what we are seeing
	//	lwGPIO_SetPin(GPIOC , &pc13) ;
		GPIOC->BSRR = GPIO_BSRR_BS13_Msk ;
		lw_waitfor_us(1000);
	//	lwGPIO_ResetPin(GPIOC , &pc13) ;
		GPIOC->BSRR = GPIO_BSRR_BR13_Msk ;
		lw_waitfor_us(1000);



	}















}
