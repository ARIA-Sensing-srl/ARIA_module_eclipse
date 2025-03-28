/*
 * processin.S
 *
 *  Created on: Jan 24, 2024
 *      Author: ARIA Sensing
 *      License: ARIA Sensing - Confidential. Not for Public Release
 *
 * Assembly optimized functions for general processing, this function use the suffix _asm to select between C
 * writtend and asm written
 * The asm level allows the usage of hardware loop instructions
 */


.section .text
.global proc_iir_biquad_asm

.align 4
.option norvc;

//void proc_iir_biquad(float* psrc. float * pdst, int numdata, float *pcoeff, int numsec, stride, reverse )
//							a0			a1			a2			a3				a4 			a5,   a6
proc_iir_biquad_asm:
	//save registers
	addi sp, sp, -8
	fsw fs0, 0(sp)
	fsw fs1, 4(sp)
	lp.starti x1, is_s
	lp.endi x1, is_e
	lp.count x1, a4
	lp.starti x0, id_s
	lp.endi x0, id_e
	slli a5, a5, 2 //*4 for stride

	//check reverse
	beq a6, x0, start_loop
	//reverse pointers
	mul t0, a2, a5 //stride * numdata*4
	//slli t0, t0, 2 //stride * numdata*4
	sub t0, t0, a5 //(stride * numdata*4)-stride
	add a0, a0, t0	//point to last
	add a1, a1, t0  //point to last
	sub a5, x0, a5 //-stride

start_loop:
	fmv.s.x ft0, x0	//zp
	fmv.s.x ft1, x0	//zpp
is_s:
	flw ft2, 0(a3)	//b0
	flw ft3, 4(a3)	//b1
	flw ft4, 8(a3)	//b2
	flw ft5, 12(a3)	//a1
	flw ft6, 16(a3)	//a2
	addi a3, a3, 20
	//ft7 acc
	//fs0 acc2
	mv t0, a0 		//t0 srcpt
	mv t1, a1		//t1 dstpt
	lp.count x0, a2
	flw fs1, 0(t0)
	add t0,t0,a5
id_s:
	//flw ft7, 0(t0)
	fnmsub.s ft7,ft0,ft5,fs1
	flw fs1, 0(t0) //next
	fnmsub.s ft7,ft1,ft6,ft7
	//fmul.s fs0,ft7,ft2
	//fmadd.s fs0,ft1,ft4,fs0
	fmul.s fs0,ft1,ft4
	fmv.s ft1, ft0
	fmadd.s fs0,ft0,ft3,fs0
	fmv.s ft0, ft7
	fmadd.s fs0,ft7,ft2,fs0
	add t0,t0,a5
	fsw fs0, 0(t1)
id_e:add t1,t1,a5
	//add t0,t0,a5
	//fmv.s ft1, ft0
//id_e:fmv.s ft0, ft7

	mv a0, a1
	fmv.s.x ft0, x0	//zp
is_e:fmv.s.x ft1, x0	//zpp

	flw fs0, 0(sp)
	flw fs1, 4(sp)
	addi sp,sp,8
	jr ra
