

/* not needed, but we need separate linker scripts anyway */
OUTPUT_ARCH(riscv)
SEARCH_DIR(.)

/* We have to align each sector to word boundaries as our current s19->slm
 * conversion scripts are not able to handle non-word aligned sections. */

SECTIONS
{

  __stack_size = DEFINED(__stack_size) ? __stack_size : 0x800;
  __heap_size = DEFINED(__heap_size) ? __heap_size :  LENGTH(L2_CV_HEAP);
  
  
   .text.coprraw :
   {
   		__copr_image_start = .;
   		KEEP(*(.text.coprraw))
   		KEEP(*(.text.coprraw.*))
   		__copr_image_end = .;
   } > PRI2
   
    .data.coprdata :
   {
   		__copr_data_start = .;
   		KEEP(*(.data.coprdata))
   		KEEP(*(.data.coprdata.*))
   		__copr_data_end = .;
   } > PRI3

  .text :
   {
     . = ALIGN(4);
     __vector_start = .;
     KEEP(*(.vectors))
     *(.text)
     *(.text.*)
     *(.boot)
     *(.eh_frame*)
     _endtext = .;
  }  > PRI0
  
  .rodata : ALIGN(4)
    {
     *(.rodata);
     *(.rodata.*)
    } > PRI0


  .data : ALIGN(4)
  {
    sdata  =  .;
    _sdata  =  .;
    __data_begin = .;
    *(.boot.data)
    *(.data_fc)
    *(.data_fc.*)
    *(.data);
    *(.data.*)
    __sdata_begin = .;
    *(.sdata);
    *(.sdata.*)
    *(.sdata2.*)
    . = ALIGN(4);
    edata  =  .;
    _edata  =  .; /* man 3 end: first addr after data */
  } > PRI1

  /* uninitialized variables to reduce startup time */
  .noinit : ALIGN(4)
    {
      *(.noinit)
    } > PRI1

  .bss : ALIGN(8)
    {
      _bss_start = .;
      __bss_start = .;
      *(.shbss) /* don't even know where this is from */
      *(.bss)
      *(.bss.*)
      *(.sbss)
      *(.sbss.*)
      *(COMMON)
      . = ALIGN(4);
      __bss_end = .;
      _bss_end = .;
      _end = .; /* man 3 end: first addr after bss */
    } > PRI1
    
  

  /* The compiler uses this to access data in the .sdata, .data, .sbss and .bss
     sections with fewer instructions (relaxation). This reduces code size. */
    PROVIDE(__global_pointer$ = MIN(__sdata_begin + 0x800,
			    MAX(__data_begin + 0x800, __bss_end - 0x800)));
	__copr_start = ORIGIN(PRI2);
  .stack :
    {
      . = ALIGN(16);
      __stack_bottom = .;
      . += __stack_size;
      __stack_top = .;
    } > PRI1
    
    .l2 : {
    	. = ALIGN(4);
    	__l2_start = .;
    	*(.l2);
     	*(.l2.*)
     	__l2_end = .;
    } > L2_CV_HEAP
    
    .heap (NOLOAD) : {
    	. = ALIGN(4);
    	__heap_start__ = .;
    	end = __heap_start__;
    	_end = end;
    	__end = end;
    	/* KEEP(*(.heap)) */
    	. = . + __heap_size;
    	__heap_end__ = .;
    	__HeapLimit = __heap_end__;
    } > L2_CV_HEAP
    
    .ipc (NOLOAD) : {
   		. = ALIGN(4);
   		*(.ipc.cv)
   		*(.ipc.copr)
   		*(.ipc)
      	*(.ipc.*)
   } > L2_IPC
   
   .ioshared (NOLOAD) : {
   		. = ALIGN(4);
   		*(.ioshared)
      	*(.ioshared.*)
   } > L2_SHARE
    
    .shared (NOLOAD) : {
   		. = ALIGN(4);
   		*(.shared)
      	*(.shared.*)
   } > L2_SHARE


  .stab  0 (NOLOAD) :
    {
      [ .stab ]
    }

  .stabstr  0 (NOLOAD) :
    {
      [ .stabstr ]
    }
}
