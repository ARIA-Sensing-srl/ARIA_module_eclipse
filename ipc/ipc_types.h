/*
 * ipc_types.h
 *
 *  Created on: Feb 1, 2024
 *      Author: ARIA Sensing
 */

#ifndef IPC_TYPES_H_
#define IPC_TYPES_H_


//ipc data structure for exchange informations
typedef struct __attribute__((packed)) _ipc_scan_seq_t{
	uint8_t txmask;
	uint8_t rxmask;
	unsigned: 16; //placeholder
}ipc_scan_seq_t;

typedef struct _ipc_cmd_acq_t{
	uint32_t samplePerScan;
	uint32_t * pIbuffer;	//must be numScan*samplePerScan (type unsigned or signed depends on PREPROC_OPT)
	uint32_t * pQbuffer;
	uint32_t * pstatus;		//set 1 when buffer is ready (in PP mode, is set by the copr and reset by the CV
	ipc_scan_seq_t * pScanSeq;
	uint32_t numScan;
	uint32_t totBinsAllocated; //samplePerScan*numScan
}ipc_cmd_acq_t;

typedef struct _ipc_cmd_codeword_t {
	int8_t * pCodeWord;
	uint32_t size; //this must be filled with the size of the buffer
}ipc_cmd_codeword_t;

typedef struct _ipc_cmd_corr_reference_t {
	int32_t * pref;	//poiner to correlation reference array
	uint32_t size;	//number of samples (must be a multiple of 4, implemented a fast correlator
	uint32_t shmnt;	//right shift amount
}ipc_cmd_corr_reference_t;

//this handler must be kept until transferComplete is asserted
typedef struct __attribute__((packed)) _ipc_cmd_datamover_t{
	void * pdest;
	const void * psrc;
	int size;
	uint8_t transferComplete;
	uint8_t busy;
}ipc_cmd_datamover_t;


#endif /* IPC_TYPES_H_ */
