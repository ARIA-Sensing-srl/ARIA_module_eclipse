/*
 * tbl_trig.h
 *
 *  Created on: Jan 29, 2024
 *      Author: ARIA Sensing
 */

#include "tbl_trig.h"
#include "math.h"

//** DEFINES START **//
#define TABLE_MASK 1023
#define TABLE_SIZE (TABLE_MASK+1)
//** DEFINES END **//

//** MACRO START **//

//** MACRO END **//

//** TYPES START **//

//** TYPES END **//

//** PRIVATE VARIABLES START **//
static uint8_t isinit = 0;
static float table_sin[TABLE_SIZE];
static float table_cos[TABLE_SIZE];

//** PRIVATE VARIABLES END **//

//** PUBLIC VARIABLES START **//

//** PUBLIC VARIABLES END **//

//** PRIVATE FUNCTIONS START **//

//** PRIVATE FUNCTIONS END **//

//** PUBLIC FUNCTIONS START **//

//** PUBLIC FUNCTIONS END **//


float tbl_sin(float norm_phase){
	uint32_t index = (uint32_t)(norm_phase * TABLE_SIZE) & TABLE_MASK;
	return table_sin[index];
}
float tbl_cos(float norm_phase){
	uint32_t index = (uint32_t)(norm_phase * TABLE_SIZE + TABLE_SIZE/4) & TABLE_MASK;
	return table_sin[index];
}
complex_float tbl_exp(float norm_phase){
	uint32_t index = (uint32_t)(norm_phase * TABLE_SIZE) & TABLE_MASK;
	complex_float ret;
	ret.imag = table_sin[index];
	/*index = (index + TABLE_SIZE/4) & TABLE_MASK;
	ret.real = table_sin[index];*/
	ret.real = table_cos[index];
	return ret;
}
void tbl_init(){
	if (isinit)
		return;
	for (int i = 0 ; i< TABLE_SIZE; i++){
		float angle = M_PI*2.0*((float)i/(float)TABLE_SIZE);
		table_sin[i]=sinf(angle);
		table_cos[i]=cosf(angle);
	}
	isinit = 1;
}
