/*
 * sysmem.c
 *
 *  Created on: Jan 16, 2024
 *      Author: ARIA Sensing
 */

/*
 * Memory management function, increase heap
 */

#include "errno.h"
#include "stdint.h"

static uint8_t* __sbrk_heap_end = NULL;


void * _sbrk(ptrdiff_t incr){
	extern uint8_t _end;
	extern uint8_t __HeapLimit;
	uint8_t *prev_heap_end;

	if (__sbrk_heap_end == NULL)
		__sbrk_heap_end = &_end;

	if (__sbrk_heap_end+incr > &__HeapLimit){
		errno = ENOMEM;
		return (void *)-1;
	}
	prev_heap_end = __sbrk_heap_end;
	__sbrk_heap_end += incr;
	return (void*)prev_heap_end;
}


