/*
 * lwCommon.h
 *
 *  Created on: Dec 16, 2022
 *      Author: Garmoosh
 */

#ifndef LWCOMMON_H_
#define LWCOMMON_H_

#include <stdint.h>
#include <stm32f103xb.h>

/*********************************************************-- OPTIONS --****************************
 * ***********************************************************************************************
 */
#define LW_ASSERT_ENABLE 0u
#define LW_ASSERT_RETURN 0u
#define LW_DEBUG_ENABLE 0u
#define LW_ERR_RETURN 0u
#define LW_INLINE_ENABLE 1u
#define LW_PRINTF_BUFFER_LEN 200u
/* Processor Options */

#define CPU_FREQ 72000000UL
#define FLASH_LATENCY 2u


/****************************************** TYPE DEFINITIONS ****************************************************
 *****************************************************************************************************************
 */


struct heap_s{

    uint8_t* heap ;
    uint8_t* bk ;
    uint32_t free_mem_sz ;
    uint32_t mem_sz ;
    uint8_t heap_align ;

} ;


/*************************************************************************************************************
 ***********************************************************************************************************
 */

#if LW_INLINE_ENABLE == 0U
#define LW_INLINE
#else
#define LW_INLINE
#endif


#if LW_ERR_RETURN == 0U
typedef void err_t ;
#else
typedef int32_t err_t ;
#endif
/*************************************** ERR CODES ********************************************/
#if LW_ERR_RETURN == 0U
#define ERR_OK
#define ERR_PARAM
#define ERR_FAULT
#define ERR_ASSERT
#define ERR_DEBUG

#else
#define ERR_OK INT32T_CAST(0U)
#define ERR_PARAM INT32T_CAST(-1)
#define ERR_FAULT INT32T_CAST(-2)
#define ERR_ASSERT INT32T_CAST(-3)

#endif
/******************************************** DEBUG **********************************************/
#if LW_DEBUG_ENABLE == 0u
#define LW_DEBUG(msg,...)
#else
#define LW_DEBUG(msg,...)   lw_printf( "Error Code %d \r\n" msg  , ##__VA_ARGS__)
#endif
/******************************************** ASSERT *********************************************/
#if LW_ASSERT_ENABLE == 0U
#define LW_ASSERT(CONDITION) ((void*)0UL)
#else

#if LW_ASSERT_RETURN == 0U
#define LW_ASSERT(CONDITION) ((CONDITION)?((void*)(0)):(LW_DEBUG("Assert_Fail" ,ERR_ASSERT )) )
#else
#define LW_ASSERT(CONDITION) ((CONDITION)?((void*)(0)):((LW_DEBUG("Assert_Fail" ,ERR_ASSERT ));return ERR_ASSERT;))
#endif


#endif

/******************************************* Externs ******************************************************/





/****************************************** WRAPPED CASTS ************************************************/
#define UINT32T_CAST(ARG) ((uint32_t)ARG)
#define INT32T_CAST(ARG)  ((int32_t)ARG)
#define USE(X) do{ (void)X; } while(0)
#define DONT_CARE 0u




/********************************** LOOP TIMEGEN *****************************/


#ifndef CPU_FREQ
#error "CPU_FREQ Not Defined in lwCommon.h"
#endif

#ifndef FLASH_LATENCY

#if (CPU_FREQ > 48000000UL && CPU_FREQ <= 72000000UL )
#define FLASH_LATENCY 2
#elif (CPU_FREQ > 24000000UL && CPU_FREQ <= 48000000UL )
#define FLASH_LATENCY 1
#elif CPU_FREQ <= 24000000UL
#define FLASH_LATENCY 0
#endif

#endif

#define INST_TIME  1000000000.00/CPU_FREQ
#define LOOP_TIME_NS  (uint32_t)( INST_TIME*4 +  FLASH_LATENCY * INST_TIME )

/* Note : These Functions use instructions to generate delays ,
 * these functions are heavily dependent on the cpu frequency as well as flash access time and wait states.
 * LOOP is 3 instructions per iteration
 */

void __attribute__((naked)) lw_waitfor( uint32_t time_ns);

/* this function waits for some time , the least time you can get is
 *  4 * Instruction time + flash latency * instruction time
 */
void __attribute__((naked)) lw_waitfor_cycles ( uint32_t cycles );


/* Light weight Printf function %x , %d , %c */
int lw_printf (char * str, ...);

/* Initializes CLOCK VALS */
void lw_Init( uint32_t SYS_clk , uint32_t APB1_clk , uint32_t APB2_clk , uint32_t USB_clk );
uint32_t lw_getSYSClk( void );
uint32_t lw_getAPB1Clk( void );
uint32_t lw_getAPB2Clk( void );
uint32_t lw_getUSBClk( void );


#define lw_waitfor_ns(timex) lw_waitfor_cycles(LOOP_TIME_NS*timex)
#define lw_waitfor_us(timex) lw_waitfor(timex*1000u)
#define lw_waitfor_ms(timex) lw_waitfor(timex*1000000u)
#define lw_waitfor_s(timex)  lw_waitfor(timex*1000000000u)

/* HEAP LOCK Mechanism for Allocation */
#define HEAP_LOCK()
#define HEAP_UNLOCK()




#endif /* LWCOMMON_H_ */
