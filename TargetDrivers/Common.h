/*
 * Common.h
 *
 *  Created on: Dec 16, 2022
 *      Author: Garmoosh
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stm32f103xb.h>

/*********************************************************-- OPTIONS --****************************
 * ***********************************************************************************************
 */
#define TARGET_ASSERT_ENABLE 1u
#define TARGET_ASSERT_RETURN 0u
#define TARGET_PRINTF_ENABLE 1u
#define TARGET_INLINE_ENABLE 1u
#define TARGET_PRINTF_BUFFER_LEN 200u
#define TARGET_DEBUG_PORT USART2
#define TARGET_PRIO_GROUP 3UL
#define MEM_ALIGNNMENT 4u
#define MEM_HEAP_SIZE  8192u

#define MEM_DUMP_FULL_MEMORY 0u
#define MEM_DUMP_FULL_BK 0u

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

#if TARGET_INLINE_ENABLE == 0U
#define TARGET_INLINE
#else
#define TARGET_INLINE
#endif

typedef int32_t err_t ;

/*************************************** ERR CODES ********************************************/
#define ERR_OK INT32T_CAST(0U)
#define ERR_PARAM INT32T_CAST(-1)
#define ERR_FAULT INT32T_CAST(-2)
#define ERR_ASSERT INT32T_CAST(-3)
#define ERR_DEBUG INT32T_CAST(-4)
#define ERR_NULL  INT32T_CAST(-5)
/******************************************** DEBUG **********************************************/
#if TARGET_PRINTF_ENABLE == 0u
#define targetPrintf(msg,...)
#else
#define targetPrintf(...)   (commonPrintf( __VA_ARGS__))
#endif
/******************************************** ASSERT *********************************************/
#if TARGET_ASSERT_ENABLE == 0U
#define TARGET_ASSERT(CONDITION) ((void*)0UL)
#else

#if TARGET_ASSERT_RETURN == 0U
#define TARGET_ASSERT(CONDITION) ( (CONDITION) ? ((void)(0)) : (targetPrintf("\r\nAssert_Fail at %s at Line(%d)" , __FILE__ ,  __LINE__ ))  )
#else
#define TARGET_ASSERT(CONDITION) ((CONDITION)?((void*)(0)):((targetPrintf("\r\nAssert_Fail at %s at Line(%d)", __FILE__, __LINE__ ));return ERR_ASSERT;))
#endif


#endif

/******************************************* Externs ******************************************************/

extern struct heap_s gheap_s ;



/****************************************** WRAPPED CASTS ************************************************/
#define UINT32T_CAST(ARG) ((uint32_t)ARG)
#define INT32T_CAST(ARG)  ((int32_t)ARG)
#define USE(X) do{ (void)X; } while(0)
#define DONT_CARE 0u
#define IS_ALIGNED(X) (!(UINT32T_CAST(X)%4))
#define malloc(SIZE) (heap_malloc( &gheap_s , SIZE))
#define free(PTR)    (heap_free(&gheap_s , PTR))
#define dump_heap()  (heap_dump(&gheap_s))
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"

#define BYTE_TO_BINARY(byte)  \
  (byte & 0x01 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x80 ? '1' : '0')


/********************************** LOOP TIMEGEN *****************************/


#ifndef CPU_FREQ
#error "CPU_FREQ Not Defined in Common.h"
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

void __attribute__((naked)) _waitfor( uint32_t time_ns);

/* this function waits for some time , the least time you can get is
 *  4 * Instruction time + flash latency * instruction time
 */
void __attribute__((naked)) _waitfor_cycles ( uint32_t cycles );


/* Light weight Printf function %x , %d , %c */
int commonPrintf (char * str, ...);

/* Initializes CLOCK VALS */
void commonInit( uint32_t SYS_clk , uint32_t APB1_clk , uint32_t APB2_clk , uint32_t USB_clk );

uint32_t commonGetSYSClk( void );
uint32_t commonGetAPB1Clk( void );
uint32_t commonGetAPB2Clk( void );
uint32_t commonGetUSBClk( void );

err_t init_heap ( struct heap_s* mem_heap  , uint8_t * heap  , uint8_t* bk , size_t hp_sz , uint8_t align );
void* heap_malloc ( struct heap_s * mem_heap , size_t sz ) ;
err_t  heap_free ( struct heap_s * mem_heap , void* ptr );
int32_t heap_dump ( struct heap_s * mem_heap  ) ;



#define _waitfor_ns(timex) _waitfor_cycles(LOOP_TIME_NS*timex)
#define _waitfor_us(timex) _waitfor(timex*1000u)
#define _waitfor_ms(timex) _waitfor(timex*1000000u)
#define _waitfor_s(timex)  _waitfor(timex*1000000000u)

/* HEAP LOCK Mechanism for Allocation */
#define HEAP_LOCK()
#define HEAP_UNLOCK()




#endif /* COMMON_H_ */
