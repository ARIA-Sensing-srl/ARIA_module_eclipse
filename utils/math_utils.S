/*
 * utils.S
 *
 *  Created on: Jan 24, 2024
 *      Author: ARIA Sensing
 *      License: ARIA Sensing - Confidential. Not for Public Release
 */



.section .text
.global utils_conv_float2half
.global utils_conv_half2float
.global utils_conv_float2int
.global utils_conv_int2float

.align 4
.option norvc;
//void utils_conv_float2half(float *, float16_t * , size);
utils_conv_float2half:
	lp.starti x1, f2hstart
	lp.endi x1, f2hend
	lp.count x1, a2
f2hstart:
	flw ft1, 0(a0)
	addi a0, a0, 4
	fcvt.h.s ft2, ft1
	fmv.x.h t1, ft2
f2hend:p.sh t1, 2(a1!)
	nop
	jr ra


//void utils_conv_half2float(float16_t * ,float *,  size);
utils_conv_half2float:
	lp.starti x1, h2fstart
	lp.endi x1, h2fend
	lp.count x1, a2
h2fstart:
	p.lh t0, 2(a0!)
	fmv.h.x ft0, t0
	fcvt.s.h ft1, ft0
	fsw ft1, 0(a1)
h2fend:addi a1,a1,4
	nop
	jr ra


//void utils_conv_float2int(float *, int32_t * , size);
utils_conv_float2int:
lp.starti x1, f2i_s
	lp.endi x1, f2i_e
	lp.count x1, a2
	flw ft1, 0(a0)
f2i_s:
	addi a0, a0, 4
	fcvt.w.s t1, ft1
	flw ft1, 0(a0)
f2i_e:p.sw t1, 4(a1!)
	nop
	jr ra


//void utils_conv_int2float(float *, int32_t * , size);
utils_conv_int2float:
	lp.starti x1, i2f_s
	lp.endi x1, i2f_e
	lp.count x1, a2
	p.lw t0, 4(a0!)
i2f_s:
	addi a1,a1,4
	fcvt.s.w ft1, t0
	p.lw t0, 4(a0!)
i2f_e:fsw ft1, -4(a1)
	nop
	jr ra
