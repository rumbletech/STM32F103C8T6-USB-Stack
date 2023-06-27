

#include <Common.h>
#include <src/USART.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>





#define MEM_BK_SIZE (MEM_HEAP_SIZE/MEM_ALIGNNMENT/8)



//todo
//better implementation

static uint32_t sys_clk ;
static uint32_t apb1_clk ;
static uint32_t apb2_clk ;
static uint32_t usb_clk ;
static uint8_t gheap[MEM_HEAP_SIZE];
static uint8_t gheap_bk[MEM_BK_SIZE];
struct heap_s gheap_s ;

uint32_t lw_getSYSClk( void ){
	return sys_clk ;
}

uint32_t lw_getAPB1Clk( void ){
	return apb1_clk ;
}
uint32_t lw_getAPB2Clk( void ){
	return apb2_clk ;
}
uint32_t lw_getUSBClk( void ){
	return usb_clk ;
}


void lw_Init( uint32_t SYS_clk , uint32_t APB1_clk , uint32_t APB2_clk , uint32_t USB_clk )
{
	sys_clk  = SYS_clk  ;
	apb1_clk = APB1_clk ;
	apb2_clk = APB2_clk ;
	usb_clk  = USB_clk ;

	init_heap(&gheap_s, gheap, gheap_bk, MEM_HEAP_SIZE , MEM_ALIGNNMENT);


}
void __attribute__((naked)) lw_waitfor( uint32_t time_ns){

	asm volatile
	(
		" udiv r1 , %[input_time] , %[loop_time] \n\t"
		" t_loop_ns : \n\t"
		" sub r1 , #1 \n\t"
		" cmp r1 , #0 \n\t"
		" bne t_loop_ns \n\t"
		" mov pc , lr"
		:  : [input_time] "r" (time_ns) , [loop_time] "r" (LOOP_TIME_NS) : "r1" , "cc"
	);
}

void __attribute__((naked)) lw_waitfor_cycles ( uint32_t cycles )
{

	asm volatile
	(	" t_loop_cycles : \n\t"
		"  sub %[loop_v] , #1 \n\t"
		"  cmp %[loop_v] , #0 \n\t"
		"  bne t_loop_cycles \n\t"
		"  mov pc , lr"
		:  : [loop_v] "r" (cycles) :  "cc"
	);

}










err_t init_heap ( struct heap_s* mem_heap  , uint8_t * heap  , uint8_t* bk , size_t hp_sz , uint8_t align )
{
    /* Mandatory Param Check */
    if ( !mem_heap || !heap || !bk ||
         hp_sz%8  ) {
             return ERR_PARAM ;
    }

    /* Initialize Meta Data*/
    mem_heap->heap = heap ;
    mem_heap->bk = bk ;
    mem_heap->free_mem_sz = hp_sz ;
    mem_heap->mem_sz = hp_sz ;
    mem_heap->heap_align = align ;

    /* Initialize Memory Keeping */
    for ( int i = 0 ; i < hp_sz/align/8 ; i++ ) {
        bk[i] = 0 ;
    }
    /* Initialize heap to 0u */
    for ( int i = 0 ; i < hp_sz ; i++ ) {
        heap[i] = 0 ;
    }

    return ERR_OK ;
}


void* heap_malloc ( struct heap_s * mem_heap , size_t sz ) {

    if ( mem_heap->free_mem_sz < sz  || sz == 0 ) {
        return NULL ;
    }

    if ( sz < mem_heap->heap_align  ){
        sz = mem_heap->heap_align ;
    }

    sz+= mem_heap->heap_align ;

    int32_t begin_i = -1 ;
    int32_t end_i = -1 ;
    uint8_t * bk = mem_heap->bk ;
    uint32_t free_chunk_sz =  0 ;
    uint32_t bk_range =  mem_heap->mem_sz/(8u*mem_heap->heap_align) ;
    uint8_t blockisf = 1 ;

    HEAP_LOCK();
    for ( int i = 0 ; i < bk_range && free_chunk_sz < sz; i++ ) {
        uint8_t bk_val = bk[i] ;
        for ( int j = 0 ; j < 8 ; j++ , bk_val>>=1 ) {
            if ( (bk_val&1) == 0 && blockisf ){
                if ( begin_i == -1 ){
                    begin_i = i*8u + j ;
                    free_chunk_sz = 0 ;
                }
                free_chunk_sz+= mem_heap->heap_align ;

                if ( free_chunk_sz >= sz ){
                    end_i = i*8u + j ;
                    /* Lock the Region */
                    bk[begin_i/8] |= (1u << begin_i%8) ;
                    bk[end_i/8] |= (1u << end_i%8) ;
                    mem_heap->free_mem_sz -= free_chunk_sz ;
                    break ;

                }
            }
            else if ( (bk_val&1) == 1 ){
                blockisf = !blockisf ;
                free_chunk_sz = 0 ;
                begin_i = -1 ;
                end_i = -1  ;
            }
        }
    }
    HEAP_UNLOCK();
    if ( free_chunk_sz < sz ) {
        return NULL ;
    }
    /* Debug*/
    //todo remove
//    LW_DEBUG("\r\n   --- Alloc with Begin Index %d  ---" , begin_i ) ;
//    LW_DEBUG("\r\n   --- Alloc with Begin Index %d  ---" , end_i ) ;
    uint32_t p = (uint32_t)(&mem_heap->heap[(begin_i+1)*mem_heap->heap_align]) ;
    p &= ~( mem_heap->heap_align -1 );
    return (void*)p ;

}

err_t  heap_free ( struct heap_s * mem_heap , void* ptr ) {

    if ( (uint32_t)ptr < (uint32_t)&mem_heap->heap[0] ||
    	 (uint32_t)ptr > (uint32_t)&mem_heap->heap[mem_heap->free_mem_sz-1] ){
        return ERR_FAULT ;
    }
    /* Get Index by ptr */
    uint32_t ptr_i = (((uint32_t)ptr - (uint32_t)mem_heap->heap)/mem_heap->heap_align)-1 ;
    uint32_t bk_range =  mem_heap->mem_sz/(8u*mem_heap->heap_align) ;

    HEAP_LOCK();
    uint32_t free_chunk_sz = 0u ;
    uint8_t * bk = mem_heap->bk ;

    /* Ptr is not allocated */
    if ( !(bk[ptr_i/8]&( 1 << ptr_i%8 )) ){
    	return ERR_FAULT ;
    }

    /* Clear Begining of the region */
    bk[ptr_i/8] &= ~( 1 << ptr_i%8 ) ;
    free_chunk_sz+= mem_heap->heap_align;

    /* Find end of the region and unlock it */
    for ( int i = ptr_i+1 ; i < bk_range ; i++ ) {
         if ( bk[i/8]&( 1 << i%8 ) ){
             bk[i/8]&= ~( 1 << i%8 ) ;
             //break out of the loop
             i = bk_range ;
         }
         free_chunk_sz+= mem_heap->heap_align;
    }



    mem_heap->free_mem_sz += free_chunk_sz ;
    HEAP_UNLOCK();

    return  ERR_OK ;


}


int32_t heap_dump ( struct heap_s * mem_heap  ) {

    uint8_t * bk = mem_heap->bk ;
    uint8_t * heap = mem_heap->heap ;
    uint32_t bk_range =  mem_heap->mem_sz/(8u*mem_heap->heap_align) ;

    LW_DEBUG("\r\n------------ Heap Start Address= 0x%x ---------" , (uint32_t)mem_heap->heap ) ;
    LW_DEBUG("\r\n------------ Heap End Address= 0x%x   ---------" , (uint32_t)(mem_heap->heap)+mem_heap->mem_sz) ;
    LW_DEBUG("\r\n------------ Total Memory in bytes = %d -----" , mem_heap->mem_sz ) ;
    LW_DEBUG("\r\n------------ Memory Alignment = %d ----------" , mem_heap->heap_align ) ;
    LW_DEBUG("\r\n------------ Free Memory = %d ---------------" , mem_heap->free_mem_sz ) ;


#if defined(MEM_DUMP_FULL_MEMORY) && (MEM_DUMP_FULL_MEMORY != 0 )
    LW_DEBUG("\r\n------------Memory Dump --------------\r\n");
    for ( int i =  0 ; i < mem_heap->mem_sz ; i++ ) {
        LW_DEBUG(" 0x%x " , heap[i] ) ;
        if ( i%10 == 0 ) {
            LW_DEBUG("\r\n");
        }
    }
#endif
#if defined(MEM_DUMP_FULL_BK) && (MEM_DUMP_FULL_BK != 0 )
    LW_DEBUG("\r\n------------Book Keeping Dump--------------\r\n");
    for ( int i = 0 ; i < bk_range ; i++ ) {
        LW_DEBUG(""BYTE_TO_BINARY_PATTERN ,  BYTE_TO_BINARY(bk[i]) ) ;
    }

#endif

    uint32_t chunk_i = 0 ;
    uint32_t chunk_size = 0 ;
    uint32_t chunk_loc = 0 ;

    for ( int i = 0  , cf = 0; i < bk_range  ; i++ ) {
        uint8_t bk_val = bk[i] ;
      for ( int j = 0 ; j < 8  ; j++ , bk_val>>= 1 ) {
        if (  cf ||  (((bk_val&1) == 1) && !cf )  ) {
            chunk_size+= mem_heap->heap_align ;
        }
        if ( (bk_val&1) == 1 && cf == 0 ) {
            chunk_loc = 8*i + j;
            cf = !cf ;
        }
        else if ( (bk_val&1) == 1 && cf  ){
            LW_DEBUG("\r\n--Allocated Chunk[%d] -- Found at Memory Location = 0x%x -- Size = %d " , chunk_i ,(uint32_t)&mem_heap->heap[chunk_loc*mem_heap->heap_align]  , chunk_size ) ;
            cf = !cf ;
            chunk_i++;
            chunk_size = 0 ;
            chunk_loc = 0 ;


        }
      }
    }

    USE(chunk_loc);
    USE(heap);
    USE(chunk_size);
    USE(chunk_i);
    USE(bk);
    return  0 ;
}



int lw_printf (char * str, ...)
{
	va_list vl;
	int i = 0, j=0;
		char buff[LW_PRINTF_BUFFER_LEN]={0}, tmp[20];
		va_start( vl, str );
		while (str && str[i])
		{
		  	if(str[i] == '%')
		  	{
 		    i++;
 		    switch (str[i])
 		    {
	 		    case 'c':
	 		    {
	 		        buff[j] = (char)va_arg( vl, int );
	 		        j++;
	 		        break;
	 		    }
	 		    case 'd':
	 		    {
	 		        itoa(va_arg( vl, int ), tmp, 10);
	 		        strcpy(&buff[j], tmp);
	 		        j += strlen(tmp);
		           break;
		        }
		        case 'x':
		        {
		           itoa(va_arg( vl, int ), tmp, 16);
		           strcpy(&buff[j], tmp);
		           j += strlen(tmp);
		           break;
		        }
		        case 's':
		        {
		           strcpy(&buff[j], va_arg( vl, char* ));
		           j += strlen(tmp);
		           break;
		        }
        	}
     	}
     	else
	    {
	       	buff[j] =str[i];
	       	j++;
	    }
	    i++;
	}

	/* Uart Debug Here*/

	lwUSART_AS_Send( LW_DEBUG_PORT ,(uint8_t*)&buff[0] ,  j ) ;
    va_end(vl);
    return j;
 }






