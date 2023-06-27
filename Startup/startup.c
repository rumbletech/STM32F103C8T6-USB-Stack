/*
 * SysConfig.c
 *
 *  Created on: Jun 6, 2022
 *      Author: Garmoosh
 *  Configuration File for the Project
 *
 *      */


#include <Common.h>

#define STARTUP_INST_TICKS 100

/* Avoid Compiler warnings */

extern void main( void );

// prevent compiler warnings
extern void SysInit( void );

static void StartUp_Debounce ( void )
{
	for ( int32_t i = 0 ; i < STARTUP_INST_TICKS ; i++ );
}

/* Linker Variables , should always be acessed as &(var) */

extern uint32_t _estack       ;
extern uint32_t _etext        ;
extern uint32_t _sbss         ;
extern uint32_t _ebss         ;
extern uint32_t _sidata       ;
extern uint32_t _edata        ;
extern uint32_t _sdata        ;
extern uint32_t __bss_start__ ;
extern uint32_t __bss_end__   ;
extern uint32_t _srodata ;
extern uint32_t _erodata ;
/* Copy Data from LOADADDR(.data) to VIRTUALADDR(.data) */
void Copy_Data ( void ) {
	uint32_t * src = &_sidata ;
	uint32_t * dst = &_sdata ;

	while ( dst != &_edata ){
		*dst++ = *src++;
	}

}
/* Fill Bss Action starting from VIRTUALADDR(.bss) */
void Fill_Bss ( void ){
	uint32_t * strt = &__bss_start__ ;
	while ( strt != &__bss_end__ ) {
		 *strt++ = (int32_t)(0UL);
	}
}

/* Reset Handler */
void Reset_Handler( void ) {

	/* Copy Static Initialized Data to RAM */

	Copy_Data();
	/* Fill BSS with Zero */
	Fill_Bss();
	/* Enable FPU ACCESS Since (Privileged mode) */
	/* NO FPU On this MICRO */
	//__fpu_s();
	/* Debounce Hardware Reset */
	StartUp_Debounce();
	/*SysInit*/
	SysInit();
	/*Jump to main() */
	main();




}

/* Default_Handler */

void Default_Handler ( void ) {
	while(1);
}




/* Weak IRQ Names */

void __attribute__((weak , alias("Default_Handler"))) NMI_Handler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) HardFault_Handler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) MemManage_Handler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) BusFault_Handler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) UsageFault_Handler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) SVC_Handler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) DebugMon_Handler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) PendSV_Handler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) SysTick_Handler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) WWDG_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) PVD_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) TAMPER_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) RTC_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) FLASH_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) RCC_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) EXTI0_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) EXTI1_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) EXTI2_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) EXTI3_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) EXTI4_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) DMA1_Channel1_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) DMA1_Channel2_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) DMA1_Channel3_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) DMA1_Channel4_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) DMA1_Channel5_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) DMA1_Channel6_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) DMA1_Channel7_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) ADC1_2_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) USB_HP_CAN_TX_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) USB_LP_CAN_RX0_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) CAN_RX1_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) CAN_SCE_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) EXTI9_5_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) TIM1_BRK_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) TIM1_UP_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) TIM1_TRG_COM_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) TIM1_CC_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) TIM2_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) TIM3_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) TIM4_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) I2C1_EV_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) I2C1_ER_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) I2C2_EV_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) I2C2_ER_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) SPI1_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) SPI2_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) USART1_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) USART2_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) USART3_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) EXTI15_10_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) RTCAlarm_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) USBWakeUp_IRQHandler ( void ) ;
void __attribute__((weak , alias("Default_Handler"))) BootRAM ( void ) ;
/* Vector table with Vector table section */

void (*isr_vectors[]) ( void ) __attribute__((section(".isr_vector_table")))    = {

 (void (*)( void ) )&_estack ,
 Reset_Handler,
 NMI_Handler,
 HardFault_Handler,
 MemManage_Handler,
 BusFault_Handler,
 UsageFault_Handler,
 (void (*)( void ) ) 0,
 (void (*)( void ) )0,
 (void (*)( void ) )0,
 (void (*)( void ) )0,
 SVC_Handler,
 DebugMon_Handler,
 (void (*)( void ) )0,
 PendSV_Handler,
 SysTick_Handler,
 WWDG_IRQHandler,            /* Window Watchdog interrupt                        */
 PVD_IRQHandler,             /* PVD through EXTI line detection interrupt        */
 TAMPER_IRQHandler,          /* Tamper interrupt                                 */
 RTC_IRQHandler,             /* RTC global interrupt                             */
 FLASH_IRQHandler,           /* Flash global interrupt                           */
 RCC_IRQHandler,             /* RCC global interrupt                             */
 EXTI0_IRQHandler,           /* EXTI Line0 interrupt                             */
 EXTI1_IRQHandler,           /* EXTI Line1 interrupt                             */
 EXTI2_IRQHandler,           /* EXTI Line2 interrupt                             */
 EXTI3_IRQHandler,           /* EXTI Line3 interrupt                             */
 EXTI4_IRQHandler,           /* EXTI Line4 interrupt                             */
 DMA1_Channel1_IRQHandler,   /* DMA1 Channel1 global interrupt                   */
 DMA1_Channel2_IRQHandler,   /* DMA1 Channel2 global interrupt                   */
 DMA1_Channel3_IRQHandler,   /* DMA1 Channel3 global interrupt                   */
 DMA1_Channel4_IRQHandler,   /* DMA1 Channel4 global interrupt                   */
 DMA1_Channel5_IRQHandler,   /* DMA1 Channel5 global interrupt                   */
 DMA1_Channel6_IRQHandler,   /* DMA1 Channel6 global interrupt                   */
 DMA1_Channel7_IRQHandler,   /* DMA1 Channel7 global interrupt                   */
 ADC1_2_IRQHandler,          /* ADC1 and ADC2 global interrupt                   */
 USB_HP_CAN_TX_IRQHandler,   /* USB High Priority or CAN TX interrupts           */
 USB_LP_CAN_RX0_IRQHandler,  /* USB Low Priority or CAN RX0 interrupts           */
 CAN_RX1_IRQHandler,         /* CAN RX1 interrupt                                */
 CAN_SCE_IRQHandler,         /* CAN SCE interrupt                                */
 EXTI9_5_IRQHandler,         /* EXTI Line[9:5] interrupts                        */
 TIM1_BRK_IRQHandler,        /* TIM1 Break interrupt                             */
 TIM1_UP_IRQHandler,         /* TIM1 Update interrupt                            */
 TIM1_TRG_COM_IRQHandler,    /* TIM1 Trigger and Commutation interrupts          */
 TIM1_CC_IRQHandler,         /* TIM1 Capture Compare interrupt                   */
 TIM2_IRQHandler,            /* TIM2 global interrupt                            */
 TIM3_IRQHandler,            /* TIM3 global interrupt                            */
 TIM4_IRQHandler,            /* TIM4 global interrupt                            */
 I2C1_EV_IRQHandler,         /* I2C1 event interrupt                             */
 I2C1_ER_IRQHandler,         /* I2C1 error interrupt                             */
 I2C2_EV_IRQHandler,         /* I2C2 event interrupt                             */
 I2C2_ER_IRQHandler,         /* I2C2 error interrupt                             */
 SPI1_IRQHandler,            /* SPI1 global interrupt                            */
 SPI2_IRQHandler,            /* SPI2 global interrupt                            */
 USART1_IRQHandler,          /* USART1 global interrupt                          */
 USART2_IRQHandler,          /* USART2 global interrupt                          */
 USART3_IRQHandler,          /* USART3 global interrupt                          */
 EXTI15_10_IRQHandler,       /* EXTI Line[15:10] interrupts                      */
 RTCAlarm_IRQHandler,        /* RTC Alarms through EXTI line interrupt           */
 USBWakeUp_IRQHandler ,      /* USB Wakeup Handler */
 (void (*)( void ) )0,
 (void (*)( void ) )0,
 (void (*)( void ) )0,
 (void (*)( void ) )0,
 (void (*)( void ) )0,
 (void (*)( void ) )0,
 (void (*)( void ) )0,
 BootRAM
};




