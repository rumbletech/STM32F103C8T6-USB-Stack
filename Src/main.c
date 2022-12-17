


#include <main.h>

extern void SysInit( void ) ;
extern s_lwGPIO_Config pc13 ;



int main(void)
{




	SysInit();


	while(1)
	{
		lwGPIO_SetPin(GPIOC , &pc13) ;
		for(int i = 0 ; i < 10000 ; i++ );
		lwGPIO_ResetPin(GPIOC , &pc13) ;
		for(int i = 0 ; i < 10000 ; i++ );


	}















}
