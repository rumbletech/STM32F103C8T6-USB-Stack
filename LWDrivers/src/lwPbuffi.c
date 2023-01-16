


#include "lwUSB.h"

/* Packet Buffer Interface for USB Communication USB_PMAADDR */

#define LWUSB_PMA_BTABLE_ADDRESS (LWUSB_PMA_START_ADDR/2u)
#define LWUSB_BTABLE_SIZE (LWUSB_EP_NUM*LWUSB_PMA_BTABLE_ENTRY_SIZE/LWUSB_PMA_BUFF_ALIGNMENT)
#define LWUSB_BUFF_SPACE_SIZE (LWUSB_BUFF_MEM_SIZE/LWUSB_PMA_BUFF_ALIGNMENT - LWUSB_BTABLE_SIZE)


struct lwUSB_btable_buff_entry_s {

	uint32_t addr ;    /* Address of Buffer */
	uint32_t count ;   /* Size */

};

struct lwUSB_btable_ep_entry_s {

	struct lwUSB_btable_buff_entry_s tx_buff ;		/* Single Buffered TX */
	struct lwUSB_btable_buff_entry_s rx_buff ;	  /* Single Buffered RX */

};

struct lwUSB_pma_s {

	struct lwUSB_btable_ep_entry_s btable[LWUSB_EP_NUM] ;
	uint32_t buff_mem[LWUSB_BUFF_SPACE_SIZE];



};

struct lwUSB_pmabuffer_s {


	uint32_t * start_addr ;
	uint32_t size ;
	uint32_t isalloced ;
	uint32_t ep_num ;

};


struct lwUSB_pmacontrol_s {

	/* Init Check */
	uint8_t isInitialized ;
	/* PMA Memory */
	struct lwUSB_pma_s * lwusb_pma ;
	/* EP Buffer Structs , a maximum of LWUSB_EP_NUM * LWUSB_BUFF_PER_EP buffers can be allocated*/
	struct lwUSB_pmabuffer_s lwusb_buffs[LWUSB_EP_NUM];
	/* Next Free Address */
	uint32_t * next_addr ;

};



volatile static struct lwUSB_pmacontrol_s lwusb_pma_control ;



/* retval : pointer to the allocated buffer.
 * Param :
 * Size : size in bytes
 * ep_num : end point number associated with the allocated buffer.
 */
void* lwUSB_Allocate( uint32_t ep_num , uint32_t size  )
{

	LW_ASSERT( size <= LWUSB_BUFF_SPACE_SIZE*LWUSB_PMA_BUFF_ALIGNMENT );
	LW_ASSERT( lwusb_pma_control.isInitialized != 0 );

	/* Align */
	if ( size%LWUSB_PMA_BUFF_ALIGNMENT != 0  )
	{
		size+= LWUSB_PMA_BUFF_ALIGNMENT ;
		size &= ~ (LWUSB_PMA_BUFF_ALIGNMENT-1u );
	}
	uint32_t free_mem = UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_BUFF_SPACE_SIZE-1] ) - UINT32T_CAST( lwusb_pma_control.next_addr ) ;
	uint32_t* ptr = lwusb_pma_control.next_addr  ;

	if ( size > free_mem ){
		return NULL ;
	}

	uint32_t status = 0 ;
	for ( int32_t i = 0 ; i < LWUSB_EP_NUM ; i++ )
	{
		if ( lwusb_pma_control.lwusb_buffs[i].isalloced == 0u )
		{
			lwusb_pma_control.lwusb_buffs[i].isalloced  = 1u  ;
			lwusb_pma_control.lwusb_buffs[i].start_addr = ptr ;
			lwusb_pma_control.lwusb_buffs[i].ep_num     = ep_num ;
			lwusb_pma_control.lwusb_buffs[i].size       = size ;
			status++;
			break ;
		}
	}

	if ( !status )
	{
		return NULL ;
	}

	lwusb_pma_control.next_addr +=  size/LWUSB_PMA_BUFF_ALIGNMENT ;
	return ptr ;
}

 err_t lwUSB_pmaWrite( uint8_t * ptr , uint32_t len , uint32_t* addr  )
{
	LW_ASSERT( (uint32_t)addr <= (uint32_t)&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_BUFF_SPACE_SIZE-1]);
	LW_ASSERT( (uint32_t)addr >= (uint32_t)&lwusb_pma_control.lwusb_pma->buff_mem[0]);
	LW_ASSERT( IS_ALIGNED(addr) );
	LW_ASSERT( len/LWUSB_PMA_BUFF_ALIGNMENT <= LWUSB_BUFF_SPACE_SIZE );
	LW_ASSERT( ptr );

	uint16_t* wptr = (uint16_t*) ptr ;

	for ( uint32_t i = 0 ; i < len/2 ; len-= 2 )
		*(addr++) = *(wptr++);

	if ( len )
		*(addr) = *(ptr+len-1) ;

	return ERR_OK ;


}

 err_t lwUSB_pmaRead( uint8_t * ptr , uint32_t len , uint32_t* addr  )
{
	LW_ASSERT( (uint32_t)addr <= (uint32_t)&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_BUFF_SPACE_SIZE-1]);
	LW_ASSERT( (uint32_t)addr >= (uint32_t)&lwusb_pma_control.lwusb_pma->buff_mem[0]);
	LW_ASSERT( IS_ALIGNED(addr) );
	LW_ASSERT( len/LWUSB_PMA_BUFF_ALIGNMENT <= LWUSB_BUFF_SPACE_SIZE );
	LW_ASSERT( ptr );

	uint16_t* wptr = (uint16_t*) ptr ;

	for ( uint32_t i = 0 ; i < len/2 ; len-= 2 )
		*(wptr++) = *(addr++);

	if ( len )
		*(ptr+len-1) = *(addr) ;

	return ERR_OK ;


}


 struct lwUSB_btable_ep_entry_s * lwUSB_pmaGetEntry ( uint32_t ep_num )
 {
	 LW_ASSERT( ep_num <= LWUSB_EP_NUM -1 ) ;
	 return &lwusb_pma_control.lwusb_pma->btable[ep_num];
 }
 err_t lwUSB_pmaWriteEntry( uint32_t ep_num , struct lwUSB_btable_ep_entry_s * entry )
{
	 LW_ASSERT(lwusb_pma_control.isInitialized != 0u );
	 LW_ASSERT( entry != NULL );
	 /* Assert Addresses Lie in the buffer memory */
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(entry->tx_buff.addr) ) >= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[0]) ) ;
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(entry->tx_buff.addr) ) <= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_BUFF_SPACE_SIZE-1]) ) ;
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(entry->rx_buff.addr) ) >= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[0]) ) ;
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(entry->rx_buff.addr) ) <= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_BUFF_SPACE_SIZE-1]) ) ;

	 lwusb_pma_control.lwusb_pma->btable[ep_num].tx_buff.addr = LWUSB_PMA_ADDR(entry->tx_buff.addr) ;
	 lwusb_pma_control.lwusb_pma->btable[ep_num].tx_buff.count = entry->tx_buff.count ;
	 lwusb_pma_control.lwusb_pma->btable[ep_num].rx_buff.addr = LWUSB_PMA_ADDR(entry->rx_buff.addr) ;
	 lwusb_pma_control.lwusb_pma->btable[ep_num].rx_buff.count = entry->rx_buff.count ;

	return ERR_OK ;

}



err_t lwUSB_pmaInit( void )
{

	lwusb_pma_control.lwusb_pma =  (struct lwUSB_pma_s*) LWUSB_PMA_START_ADDR ;
	memset((void*)lwusb_pma_control.lwusb_pma  , LWUSB_FILL_VALUE ,  sizeof(struct lwUSB_pma_s) * sizeof(uint32_t));

	for ( int32_t i = 0 ; i < LWUSB_EP_NUM ; i++  ){

		lwusb_pma_control.lwusb_buffs[i].isalloced = 0u ;
		lwusb_pma_control.lwusb_buffs[i].ep_num = 0u ;
		lwusb_pma_control.lwusb_buffs[i].size = 0u ;
		lwusb_pma_control.lwusb_buffs[i].start_addr = NULL ;
	}

	lwusb_pma_control.next_addr = (uint32_t*)&lwusb_pma_control.lwusb_pma->buff_mem[0];
	lwusb_pma_control.isInitialized = 1u ;

	return ERR_OK ;
}



