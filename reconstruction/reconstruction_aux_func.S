/*
 * reconstruction_aux_func.S
 *
 *  Created on: Apr 23, 2024
 *      Author: andream
 */



.section .text
.global rec_partCFcompute_asm

.align 4
.option norvc;


//void rec_partCFcompute_asm(float16_t* pPart, int canvasPoints, int numStream, complex_float * pcfCplxCumSum, float * pcfCumAbsSum);
/*
a0 pPart
a1 canvasPoints
a2 numStream
a3 pcfCplxCumSum
a4 pcfCumAbsSum

computation is made inside HW loop and without float to half float transfer
*/
rec_partCFcompute_asm:
	lp.starti x1, 1f
	lp.endi x1, 4f
	addi a2, a2, -1 //numStreams-1
	lp.count x1, a2
	lp.starti x0, 2f
	lp.endi x0, 3f
	fmv.s.x fa0, x0	//complex pixel out (start with 0)
	fmv.s.x fa1, x0
	flw fa2, 0(a3) //pcfCplxCumSum fa2, fa3 == a
	flw fa3, 4(a3)
	flw fa4, 0(a4) //pcfCumAbsSum fa2
	//convert to half precision
	//fcvt.h.s fa2, fa2
	//fcvt.h.s fa3, fa3
	//fcvt.h.s fa4, fa4
	fmv.s ft0, fa2	//t0,t1 are a
	fmv.s ft1, fa3

	sll a1, a1, 2 //canvasPoints<<2 (increment for canvas point)
1:	lp.count x0, a2
	mv t1, a0 //set pointer (first run is equal to s)
2:	/*lw t2, 0(t1) //load complex encoded in f16
	add t1, t1, a1 //update index
	pv.extractu.h t3, t2, 0
	fmv.h.x ft2, t3 //re(b)
	fcvt.s.h ft2, ft2
	pv.extractu.h t3, t2, 1
	fmv.h.x ft3, t3 //im(b)
	fcvt.s.h ft3, ft3*/
	lh t2, 0(t1)
	lh t3, 2(t1)
	add t1, t1, a1
	fmv.h.x ft2, t2 //re(b)
	fcvt.s.h ft2, ft2
	fmv.h.x ft3, t3 //im(b)
	fcvt.s.h ft3, ft3
	/*//cplx mult a*b
	fmul.s ft4, ft0, ft2
	fmsub.s ft4, ft1, ft3, ft4
	fmul.s ft5, ft0, ft3
	fmadd.s ft5, ft1, ft2, ft5
	fadd.s fa0, fa0, ft4
3:	fadd.s fa1, fa1, ft5*/
	//cplxmult a* conj(b)
	fmul.s ft4, ft0, ft2
	fmadd.s ft4, ft1, ft3, ft4
	fmul.s ft5, ft1, ft2
	fmsub.s ft5, ft0, ft3, ft5
	fadd.s fa0, fa0, ft4
3:	fadd.s fa1, fa1, ft5


	/*lw t2, 0(a0)
	add a0, a0, a1
	addi a2, a2, -1 //decrement numStream (count number of iterations of inner loop
	pv.extractu.h t3, t2, 0
	fmv.h.x ft0, t3 //re(a)
	fcvt.s.h ft0, ft0
	pv.extractu.h t3, t2, 1
	fmv.h.x ft1, t3 //im(a)
	fcvt.s.h ft1, ft1*/
	lh t2, 0(a0)
	addi a2, a2, -1 //decrement numStream (count number of iterations of inner loop
	lh t3, 2(a0)
	add a0, a0, a1
	fmv.h.x ft0, t2 //re(a)
	fcvt.s.h ft0, ft0
	fmv.h.x ft1, t3 //im(a)
	fcvt.s.h ft1, ft1

	fadd.s fa2, fa2, ft0
	fadd.s fa3, fa3, ft1
	fmadd.s fa4, ft0, ft0, fa4
4:	fmadd.s fa4, ft1, ft1, fa4
	//output is passed thru a0 and a1
	fmv.x.w a0, fa0
	fmv.x.w a1, fa1
	//convert pixel out
	/*fcvt.s.h fa0, fa0
	fcvt.s.h fa1, fa1
	//convert sumcum and save
	fcvt.s.h fa2, fa2
	fcvt.s.h fa3, fa3
	fcvt.s.h fa4, fa4*/
	fsw fa2, 0(a3) //pcfCplxCumSum fa2, fa3 == a
	fsw fa3, 4(a3)
	fsw fa4, 0(a4) //pcfCumAbsSum fa2
	jr ra

