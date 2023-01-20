


#include "lwPbuffi.h"

/* Packet Buffer Interface for USB Communication USB_PMAADDR */


struct lwUSB_btable_buff_entry_s {

	uint32_t addr ;    /* Address of Buffer */
	uint32_t count ;   /* Size */

};

/* Defines an Entry in the BTable */
struct lwUSB_btable_ep_entry_s {

	struct lwUSB_btable_buff_entry_s tx_buff ;	  /* Single Buffered TX */
	struct lwUSB_btable_buff_entry_s rx_buff ;	  /* Single Buffered RX */

};


struct lwUSB_pma_s {

	struct lwUSB_btable_ep_entry_s btable[LWUSB_PMA_EP_NUM] ;
	uint32_t buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE];



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
	//todo maybe deprecate the below line
	/* EP Buffer Structs , a maximum of LWUSB_EP_NUM * LWUSB_BUFF_PER_EP buffers can be allocated*/
	struct lwUSB_pmabuffer_s lwusb_buffs[LWUSB_PMA_EP_NUM];
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

	LW_ASSERT( size <= LWUSB_PMA_BUFF_SPACE_SIZE*LWUSB_PMA_BUFF_ALIGNMENT );
	LW_ASSERT( lwusb_pma_control.isInitialized != 0 );

	/* Align */
	if ( size%LWUSB_PMA_BUFF_ALIGNMENT != 0  )
	{
		size+= LWUSB_PMA_BUFF_ALIGNMENT ;
		size &= ~ (LWUSB_PMA_BUFF_ALIGNMENT-1u );
	}
	uint32_t free_mem = UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1] ) - UINT32T_CAST( lwusb_pma_control.next_addr ) ;
	uint32_t* ptr = lwusb_pma_control.next_addr  ;

	if ( size > free_mem ){
		return NULL ;
	}

	uint32_t status = 0 ;
	for ( int32_t i = 0 ; i < LWUSB_PMA_EP_NUM ; i++ )
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


 err_t lwUSB_pmaWrite( uint8_t EPNum , uint8_t * DataPtr , uint32_t len  ){

	uint32_t* dest = LWUSB_PMA_GET_SYS_ADDR(lwusb_pma_control.lwusb_pma->btable[EPNum].tx_buff.addr);
	uint16_t* src = (uint16_t*) DataPtr ;
#ifdef LW_ASSERT_ENABLE
	LW_ASSERT( lwusb_pma_control.isInitialized != 0 );
	LW_ASSERT( (uint32_t)dest <= (uint32_t)&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1]);
	LW_ASSERT( (uint32_t)dest >= (uint32_t)&lwusb_pma_control.lwusb_pma->buff_mem[0]);
	LW_ASSERT( IS_ALIGNED(dest) );
	LW_ASSERT( len/LWUSB_PMA_BUFF_ALIGNMENT <= LWUSB_PMA_BUFF_SPACE_SIZE );
	LW_ASSERT( DataPtr );
#endif
	for ( uint32_t i = 0 ; i < len/2 ; len-= 2 ){
		*(dest++) = *(src++);
	}
	if ( len ){
		*(dest) = *(DataPtr+len-1) ;
	}
	return ERR_OK ;

}

err_t lwUSB_pmaRead( uint8_t EPNum , uint8_t * DataPtr , uint32_t len  ){

	uint32_t* src  = LWUSB_PMA_GET_SYS_ADDR(lwusb_pma_control.lwusb_pma->btable[EPNum].rx_buff.addr);
	uint16_t* dest = (uint16_t*)DataPtr ;
#ifdef LW_ASSERT_ENABLE
	LW_ASSERT( lwusb_pma_control.isInitialized != 0 );
	LW_ASSERT( (uint32_t)src <= (uint32_t)&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1]);
	LW_ASSERT( (uint32_t)src >= (uint32_t)&lwusb_pma_control.lwusb_pma->buff_mem[0]);
	LW_ASSERT( IS_ALIGNED(src) );
	LW_ASSERT( len/LWUSB_PMA_BUFF_ALIGNMENT <= LWUSB_PMA_BUFF_SPACE_SIZE );
	LW_ASSERT( DataPtr );
#endif

	for ( uint32_t i = 0 ; i < len/2 ; len-= 2 ){
		*(dest++) = *(src++);
	}
	if ( len ){
		*(DataPtr+len-1) = *(src) ;
	}
	return ERR_OK ;
}

int32_t lwUSB_pmaGetNumBytes( uint8_t EPNum  ) {

	if ( lwusb_pma_control.isInitialized == 0 ){
		return ERR_FAULT ;
	}
	return lwusb_pma_control.lwusb_pma->btable[EPNum].rx_buff.count ;
}

int32_t lwUSB_pmaWriteNumBytes( uint8_t EPNum , uint32_t Length ){

	if ( lwusb_pma_control.isInitialized == 0 ){
		return ERR_FAULT ;
	}
	lwusb_pma_control.lwusb_pma->btable[EPNum].tx_buff.count = Length ;
	return ERR_OK ;
}

 struct lwUSB_btable_ep_entry_s * lwUSB_pmaGetEntry ( uint32_t ep_num )
 {
	 LW_ASSERT( ep_num <= LWUSB_PMA_EP_NUM -1 ) ;
	 return &lwusb_pma_control.lwusb_pma->btable[ep_num];
 }
 static err_t lwUSB_pmaWriteEntry( uint32_t ep_num , struct lwUSB_btable_ep_entry_s * entry )
{
	 LW_ASSERT(lwusb_pma_control.isInitialized != 0u );
	 LW_ASSERT( entry != NULL );
	 /* Assert Addresses Lie in the buffer memory */
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(entry->tx_buff.addr) ) >= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[0]) ) ;
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(entry->tx_buff.addr) ) <= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1]) ) ;
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(entry->rx_buff.addr) ) >= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[0]) ) ;
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(entry->rx_buff.addr) ) <= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1]) ) ;

	 lwusb_pma_control.lwusb_pma->btable[ep_num].tx_buff.addr = LWUSB_PMA_GET_PMA_ADDR(entry->tx_buff.addr) ;
	 lwusb_pma_control.lwusb_pma->btable[ep_num].tx_buff.count = entry->tx_buff.count ;
	 lwusb_pma_control.lwusb_pma->btable[ep_num].rx_buff.addr = LWUSB_PMA_GET_PMA_ADDR(entry->rx_buff.addr) ;
	 lwusb_pma_control.lwusb_pma->btable[ep_num].rx_buff.count = entry->rx_buff.count ;

	return ERR_OK ;

}



err_t lwUSB_pmaInit( void )
{
	USE(lwUSB_pmaWriteEntry);
	lwusb_pma_control.lwusb_pma =  (struct lwUSB_pma_s*) LWUSB_PMA_START_ADDR ;
	memset((void*)lwusb_pma_control.lwusb_pma  , LWUSB_PMA_FILL_VALUE ,  sizeof(struct lwUSB_pma_s) * sizeof(uint32_t));

	for ( int32_t i = 0 ; i < LWUSB_PMA_EP_NUM ; i++  ){

		lwusb_pma_control.lwusb_buffs[i].isalloced = 0u ;
		lwusb_pma_control.lwusb_buffs[i].ep_num = 0u ;
		lwusb_pma_control.lwusb_buffs[i].size = 0u ;
		lwusb_pma_control.lwusb_buffs[i].start_addr = NULL ;
	}

	lwusb_pma_control.next_addr = (uint32_t*)&lwusb_pma_control.lwusb_pma->buff_mem[0];
	lwusb_pma_control.isInitialized = 1u ;

	return ERR_OK ;
}



