


#include <main.h>


extern s_lwGPIO_Config pc13 ;
extern s_lwUSART_Config usart2 ;
extern s_lwGPIO_Config pa8 ;
extern s_lwGPIO_Config pa2 ;
extern s_lwGPIO_Config pa3 ;


volatile int32_t usb_state = USB_STATE_IDLE ;
/*
 * todo refactor the code . without bricking it
 * todo seperate in/out/setup handlers into seperate functions
 * todo implement unidirictional only for data endpoints and bidirctional only for control eps
 * todo implement ring buffers for rx/tx
 * todo figure out correct handling for todos in the code .
 * todo REFACTOR THE UGLY CODE -- SIMPLIFY
 * todo check status table in the specifications
 */

int main(void)
{
	//todo replace infinite loop with kernel




	while(1)
	{

//		GPIOC->BSRR = GPIO_BSRR_BS13_Msk ;
//		lw_waitfor_us(100000);
//		GPIOC->BSRR = GPIO_BSRR_BR13_Msk ;
//		lw_waitfor_us(100000);

		if ( usb_state == USB_STATE_RESET )
		{
			/* Reset Task */
			//lwUSB_Init();
	//		lwUSB_Reset();


			usb_state = USB_STATE_IDLE ;

		}
		else if ( usb_state == USB_STATE_SUSP )
		{


			usb_state = USB_STATE_IDLE ;
		}
		else if ( usb_state == USB_STATE_SOF )
		{

			usb_state = USB_STATE_IDLE ;

		}
		else if ( usb_state == USB_STATE_ESOF )
		{

			usb_state = USB_STATE_IDLE ;

		}
		else if ( usb_state == USB_STATE_ERR )
		{

			usb_state = USB_STATE_IDLE ;

		}
		else if ( usb_state == USB_STATE_WKUP )
		{

			usb_state = USB_STATE_IDLE ;

		}



	}




}
