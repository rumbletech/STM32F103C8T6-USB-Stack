/*
 * lwDebug.h
 *
 *  Created on: Jun 30, 2023
 *      Author: Garmoosh
 */

#ifndef LW_DEBUG_H_
#define LW_DEBUG_H_

/************************************* USER INCLUDES *************************/
#include "../TargetDrivers/Common.h"
/*****************************************************************************/
/**************************** Switches ***************************************/
#define DP_DEBUG_EN 1u         /* DataPool Debug Enable */

/*****************************************************************************/
#define LW_NO_PRINTF(...)
#define LW_NO_ASSERT(CONDITION)
/***************** Printf/ASSERT used for debugging purposes *****************/
// Define as Follows : LW_PRINTF(...) TargetPrintf(__VA_ARGS__)
#define LW_PRINTF(...) LW_NO_PRINTF(__VA_ARGS__)

/***************** LW_DEBUG **************************************************/
#if defined(LW_DEBUG_EN) && LW_DEBUG_EN != 0
#define LW_DEBUG(...) LW_PRINTF(__VA_ARGS__)
#else
#define LW_DEBUG(...) LW_NO_PRINTF(__VA_ARGS__)
#endif
/**************** LW_ASSERT  **************************************************/
#if defined(LW_ASSERT_EN) && LW_ASSERT_EN != 0
#define LW_ASSERT(CONDITION)  ( (CONDITION) ? ((void)(0)) : (LW_PRINTF("\r\nAssert_Fail at %s at Line(%d)" , __FILE__ ,  __LINE__ ))  )
#else
#define LW_ASSERT(CONDITION) LW_NO_ASSERT(CONDITION)
#endif


#endif /* LW_DEBUG_H_ */
