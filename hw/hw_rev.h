/*
 * hw_rev.h
*
 *  Created on: Jan 14, 2025
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */
 
 
 
 
#ifndef HWREV
  #error "HW revision not defined"
#endif


#if !(HWREV==0x00010001)
  #error "HW revision not valid"
#endif
