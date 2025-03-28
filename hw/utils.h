/*
 * utils.h
 *
 *  Created on: Dec 14, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */


#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

#define mSetRegisterPart(reg, val, mask, pos) {reg = (reg & ~mask) | ((val << pos) & mask);}
#define mGetRegisterPart(reg, mask, pos)	((reg & mask) >> pos)

#endif /* COMMON_UTILS_H_ */
