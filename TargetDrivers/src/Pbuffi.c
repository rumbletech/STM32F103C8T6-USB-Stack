


#include <src/Pbuffi.h>

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
	/* Next Free Address */
	uint32_t * next_addr ;

};



volatile static struct lwUSB_pmacontrol_s lwusb_pma_control ;



/* Retval : Pointer to the Allocated Memory
 * Param : ep_num , EndPoint Number
 * Param : size   , Size to Allocate in bytes
 */
void* lwUSB_pmaAllocate( uint32_t ep_num , size_t size  ){

	LW_ASSERT( size <= LWUSB_PMA_BUFF_SPACE_SIZE*LWUSB_PMA_BUFF_ALIGNMENT );
	LW_ASSERT( lwusb_pma_control.isInitialized != 0 );

	/* Align */
	if ( size%LWUSB_PMA_BUFF_ALIGNMENT != 0  ){
		size+= LWUSB_PMA_BUFF_ALIGNMENT ;
		size &= ~ (LWUSB_PMA_BUFF_ALIGNMENT-1u );
	}
	uint32_t free_mem = UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1] ) - UINT32T_CAST( lwusb_pma_control.next_addr ) ;
	uint32_t* ptr = lwusb_pma_control.next_addr  ;

	if ( size > free_mem ){
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
	return lwusb_pma_control.lwusb_pma->btable[EPNum].rx_buff.count & 0x3FF ;
}

int32_t lwUSB_pmaWriteNumBytes( uint8_t EPNum , uint32_t Length ){

	if ( lwusb_pma_control.isInitialized == 0 ){
		return ERR_FAULT ;
	}
	lwusb_pma_control.lwusb_pma->btable[EPNum].tx_buff.count = Length ;
	return ERR_OK ;
}

err_t lwUSB_pmaWriteTXEntry( uint32_t ep_num , void *addr ){
	 /* Assert PMA Initialization */
	 LW_ASSERT(lwusb_pma_control.isInitialized != 0u );
	 /* Assert Addresses Lie in the buffer memory */
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(addr) ) >= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[0]) ) ;
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(addr) ) <= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1]) ) ;


	 lwusb_pma_control.lwusb_pma->btable[ep_num].tx_buff.addr = LWUSB_PMA_GET_PMA_ADDR(addr) ;

	return ERR_OK ;

}

err_t lwUSB_pmaWriteRXEntry( uint32_t ep_num , void *addr , size_t size  ){
	 /* Assert PMA Initialization */
	 LW_ASSERT(lwusb_pma_control.isInitialized != 0u );
	 /* Assert Addresses Lie in the buffer memory */
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(addr) ) >= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[0]) ) ;
	 LW_ASSERT( UINT32T_CAST(UINT32T_CAST(addr) ) <= UINT32T_CAST(&lwusb_pma_control.lwusb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1]) ) ;


	 lwusb_pma_control.lwusb_pma->btable[ep_num].rx_buff.addr  = LWUSB_PMA_GET_PMA_ADDR(addr) ;
	 lwusb_pma_control.lwusb_pma->btable[ep_num].rx_buff.count =  ((size/2) <<  USB_COUNT0_RX_NUM_BLOCK_Pos) ;

	return ERR_OK ;

}


err_t lwUSB_pmaInit( void ){

	/* Point to PMA Base Address */
	lwusb_pma_control.lwusb_pma =  (struct lwUSB_pma_s*) LWUSB_PMA_START_ADDR ;
	/* Set to the Predefined Fill Value */
	memset((void*)lwusb_pma_control.lwusb_pma  , LWUSB_PMA_FILL_VALUE ,  sizeof(struct lwUSB_pma_s) * sizeof(uint32_t));
	/* Setup the Next Valid Pma Address*/
	lwusb_pma_control.next_addr = (uint32_t*)&lwusb_pma_control.lwusb_pma->buff_mem[0];
	/* Initialized */
	lwusb_pma_control.isInitialized = 1u ;

	return ERR_OK ;
}



