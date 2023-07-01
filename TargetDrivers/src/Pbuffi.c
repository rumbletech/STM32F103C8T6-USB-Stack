


#include <src/Pbuffi.h>

/* Packet Buffer Interface for USB Communication USB_PMAADDR */

struct USB_btable_buff_entry_s {

	uint32_t addr ;    /* Address of Buffer */
	uint32_t count ;   /* Size */

};

/* Defines an Entry in the BTable */
struct USB_btable_ep_entry_s {

	struct USB_btable_buff_entry_s tx_buff ;	  /* Single Buffered TX */
	struct USB_btable_buff_entry_s rx_buff ;	  /* Single Buffered RX */

};


struct USB_pma_s {

	struct USB_btable_ep_entry_s btable[LWUSB_PMA_EP_NUM] ;
	uint32_t buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE];



};

struct USB_pmabuffer_s {


	uint32_t * start_addr ;
	uint32_t size ;
	uint32_t isalloced ;
	uint32_t ep_num ;

};


struct USB_pmacontrol_s {

	/* Init Check */
	uint8_t isInitialized ;
	/* PMA Memory */
	struct USB_pma_s * usb_pma ;
	/* Next Free Address */
	uint32_t * next_addr ;

};

volatile static struct USB_pmacontrol_s usb_pma_control ;

/* Retval : Pointer to the Allocated Memory
 * Param : ep_num , EndPoint Number
 * Param : size   , Size to Allocate in bytes
 */
void* USB_pmaAllocate( uint32_t ep_num , size_t size  ){

	TARGET_ASSERT( size <= LWUSB_PMA_BUFF_SPACE_SIZE*LWUSB_PMA_BUFF_ALIGNMENT );
	TARGET_ASSERT( usb_pma_control.isInitialized != 0 );

	/* Align */
	if ( size%LWUSB_PMA_BUFF_ALIGNMENT != 0  ){
		size+= LWUSB_PMA_BUFF_ALIGNMENT ;
		size &= ~ (LWUSB_PMA_BUFF_ALIGNMENT-1u );
	}
	uint32_t free_mem = UINT32T_CAST(&usb_pma_control.usb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1] ) - UINT32T_CAST( usb_pma_control.next_addr ) ;
	uint32_t* ptr = usb_pma_control.next_addr  ;

	if ( size > free_mem ){
		return NULL ;
	}

	usb_pma_control.next_addr +=  size/LWUSB_PMA_BUFF_ALIGNMENT ;
	return ptr ;
}


 err_t USB_pmaWrite( uint8_t EPNum , uint8_t * DataPtr , uint32_t len  ){

	uint32_t* dest = LWUSB_PMA_GET_SYS_ADDR(usb_pma_control.usb_pma->btable[EPNum].tx_buff.addr);
	uint16_t* src = (uint16_t*) DataPtr ;
	for ( uint32_t i = 0 ; i < len/2 ; len-= 2 ){
		*(dest++) = *(src++);
	}
	if ( len ){
		*(dest) = *(DataPtr+len-1) ;
	}
	return ERR_OK ;

}

err_t USB_pmaRead( uint8_t EPNum , uint8_t * DataPtr , uint32_t len  ){

	uint32_t* src  = LWUSB_PMA_GET_SYS_ADDR(usb_pma_control.usb_pma->btable[EPNum].rx_buff.addr);
	uint16_t* dest = (uint16_t*)DataPtr ;

	for ( uint32_t i = 0 ; i < len/2 ; len-= 2 ){
		*(dest++) = *(src++);
	}
	if ( len ){
		*(DataPtr+len-1) = *(src) ;
	}
	return ERR_OK ;
}

int32_t USB_pmaGetNumBytes( uint8_t EPNum  ) {

	if ( usb_pma_control.isInitialized == 0 ){
		return ERR_FAULT ;
	}
	return usb_pma_control.usb_pma->btable[EPNum].rx_buff.count & 0x3FF ;
}

int32_t USB_pmaWriteNumBytes( uint8_t EPNum , uint32_t Length ){

	if ( usb_pma_control.isInitialized == 0 ){
		return ERR_FAULT ;
	}
	usb_pma_control.usb_pma->btable[EPNum].tx_buff.count = Length ;
	return ERR_OK ;
}

err_t USB_pmaWriteTXEntry( uint32_t ep_num , void *addr ){
	 /* Assert PMA Initialization */
	TARGET_ASSERT(usb_pma_control.isInitialized != 0u );
	 /* Assert Addresses Lie in the buffer memory */
	TARGET_ASSERT( UINT32T_CAST(UINT32T_CAST(addr) ) >= UINT32T_CAST(&usb_pma_control.usb_pma->buff_mem[0]) ) ;
	TARGET_ASSERT( UINT32T_CAST(UINT32T_CAST(addr) ) <= UINT32T_CAST(&usb_pma_control.usb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1]) ) ;


	 usb_pma_control.usb_pma->btable[ep_num].tx_buff.addr = LWUSB_PMA_GET_PMA_ADDR(addr) ;

	return ERR_OK ;

}

err_t USB_pmaWriteRXEntry( uint32_t ep_num , void *addr , size_t size  ){
	 /* Assert PMA Initialization */
	TARGET_ASSERT(usb_pma_control.isInitialized != 0u );
	 /* Assert Addresses Lie in the buffer memory */
	TARGET_ASSERT( UINT32T_CAST(UINT32T_CAST(addr) ) >= UINT32T_CAST(&usb_pma_control.usb_pma->buff_mem[0]) ) ;
	TARGET_ASSERT( UINT32T_CAST(UINT32T_CAST(addr) ) <= UINT32T_CAST(&usb_pma_control.usb_pma->buff_mem[LWUSB_PMA_BUFF_SPACE_SIZE-1]) ) ;


	 usb_pma_control.usb_pma->btable[ep_num].rx_buff.addr  = LWUSB_PMA_GET_PMA_ADDR(addr) ;
	 usb_pma_control.usb_pma->btable[ep_num].rx_buff.count =  ((size/2) <<  USB_COUNT0_RX_NUM_BLOCK_Pos) ;

	return ERR_OK ;

}


err_t USB_pmaInit( void ){

	/* Point to PMA Base Address */
	usb_pma_control.usb_pma =  (struct USB_pma_s*) LWUSB_PMA_START_ADDR ;
	/* Set to the Predefined Fill Value */
	memset((void*)usb_pma_control.usb_pma  , LWUSB_PMA_FILL_VALUE ,  sizeof(struct USB_pma_s) * sizeof(uint32_t));
	/* Setup the Next Valid Pma Address*/
	usb_pma_control.next_addr = (uint32_t*)&usb_pma_control.usb_pma->buff_mem[0];
	/* Initialized */
	usb_pma_control.isInitialized = 1u ;

	return ERR_OK ;
}
