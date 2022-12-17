/*
 * lwCommon.h
 *
 *  Created on: Dec 16, 2022
 *      Author: Garmoosh
 */

#ifndef LWCOMMON_H_
#define LWCOMMON_H_

#include <stdint.h>
#include <stdlib.h>
#include <stm32f103xb.h>

/*********************************************************-- OPTIONS --****************************
 * ***********************************************************************************************
 */
#define LW_ASSERT_ENABLE 0u
#define LW_ASSERT_RETURN 0u
#define LW_DEBUG_ENABLE 0u
#define LW_ERR_RETURN 0u
#define LW_INLINE_ENABLE 1u





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

#else
#define ERR_OK INT32T_CAST(0U)
#define ERR_PARAM INT32T_CAST(-1)
#define ERR_FAULT INT32T_CAST(-2)
#define ERR_ASSERT INT32T_CAST(-3)

#endif
/******************************************** DEBUG **********************************************/
#if LW_DEBUG_ENABLE == 0u
#define LW_DEBUG(msg,err_code)
#else
#define LW_DEBUG(msg,err_code)       //TODO
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

/****************************************** WRAPPED CASTS ************************************************/
#define UINT32T_CAST(ARG) ((uint32_t)ARG)
#define INT32T_CAST(ARG)  ((int32_t)ARG)
#endif /* LWCOMMON_H_ */
