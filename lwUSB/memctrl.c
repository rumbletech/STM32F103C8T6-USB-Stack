/* Author : mrashada
 * Date   : 8/4/2023
 * ***********************************************************************
 * ***********************************************************************
 * This Defines the memory controller.
 * ***********************************************************************
 * ***********************************************************************
 */

#include "memctrl.h"
#include "Include/lwUSB_Opts.h"
#include "mem.h"

static uint8_t m_bu8[LWUSB_BUFF_M_S];
static uint8_t m_du8[LWUSB_DESCRIPTOR_M_S];
static uint8_t m_su8[LWUSB_STACK_M_S];

static struct mem_s bm;
static struct mem_s dm;
static struct mem_s sm;

void memctrl_init ( void  ){

	mem_init(&sm, &m_su8[0] , sizeof(m_su8));
	mem_init(&bm, &m_bu8[0] , sizeof(m_bu8));
	mem_init(&dm, &m_du8[0] , sizeof(m_du8));

	return;
}

void * memctrl_alloc( uint32_t bSize  , enum e_mem_type mt ){

	struct mem_s * msrc = NULL ;

	switch ( mt ){
	case e_mem_type_buffer_mem :
		msrc = &bm;
		break;
	case e_mem_type_discriptor_mem :
		msrc = &dm;
		break;
	case e_mem_type_var_mem :
		msrc = &sm;
		break;
	default:
		return NULL;
	}
	return mem_alloc(msrc , bSize);
}
