

.section .text

_start:
/* initialize global pointer */
.option push
.option norelax
	add s1, s1, s2
	add s1, s1, s2
1:	auipc gp, %pcrel_hi(__global_pointer$)
	addi  gp, gp, %pcrel_lo(1b)
	//pv.max.h t0, t1, t2 //pv. add simd support
.option pop
	/* init stack pointer */
	la sp, __stack_top
/* clear the bss segment */
	la t0, __bss_start
    la t1, __bss_end
1:
	sw zero,0(t0)
	addi t0, t0, 4
	bltu t0, t1, 1b

	/*clear l2 section*/
	la t0, __l2_start
    la t1, __l2_end
1:
	sw zero,0(t0)
	addi t0, t0, 4
	bltu t0, t1, 1b


	//setup interrupt vector start address
	la t0, __vector_start
	csrw 0x305, t0
	//enable interrupts line (global interrupt not enabled for now)
	addi t0, x0, -1
	csrw 0x304, t0
	call main
	tail exit

.global change_stack
change_stack:
	mv   sp, a2
	jr   a1


.section .vectors, "ax"
.option norvc;
.org 0x0
	jal x0, exception_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, isr_timer_lo
	jal x0, isr_timer_hi
	jal x0, default_handler
	jal x0, default_handler
	jal x0, isr_rise_fall
	jal x0, isr_gpio
	jal x0, default_handler
	jal x0, isr_adv_tmr0
	jal x0, isr_adv_tmr1
	jal x0, isr_adv_tmr2
	jal x0, isr_adv_tmr3
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, isr_evt_fifo_valid
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler
	jal x0, default_handler

	/* reset vector */
	.org 0x80
	jal x0, _start

	.org 0x84
loop:
	j loop

default_handler:
	mret


#ifdef INCLUDE_COPRBIN
.section .text.coprraw
.incbin "../coprbin/coprimg-aa.bin"

.section .data.coprdata
.incbin "../coprbin/coprimg-ab.bin"
#endif
