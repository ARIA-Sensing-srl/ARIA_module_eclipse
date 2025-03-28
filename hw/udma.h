/*
 * uDMA.h
*
 *  Created on: Oct 10, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#ifndef UDMA_H_
#define UDMA_H_

#include "soc_reg.h"
#include "stdint.h"


	#define UDMA_UART_OFFSET 	(0x00000080)
	#define UDMA_SPIM0_OFFSET 	(0x00000100)
	#define UDMA_SPIM1_OFFSET 	(0x00000180)
	#define UDMA_SPIM2_OFFSET 	(0x00000200)


	#define UDMA_UART_MAX_SIZE 65535

	//channels
	#define UDMA_UART_CHANNELS 1
	#define UDMA_SPIM_CHANNELS 3

	//
	#define UDMA_UART_CH0 0

	#define UDMA_SPI_CH0 0
	#define UDMA_SPI_CH1 1
	#define UDMA_SPI_CH2 2

	typedef struct _uDMA_t{
		uint32_t CTRL_CFG_CG;
		uint32_t CTRL_CFG_EVENT;
		uint32_t CTRL_CFG_RST;
	}uDMA_t;

	#define UDMA ((volatile uDMA_t*)uDMA_BASE)

	#define UDMA_UART0_PERID 0
	#define UDMA_SPIM0_PERID 1
	#define UDMA_SPIM1_PERID 2
	#define UDMA_SPIM2_PERID 3
	#define UDMA_MAX_PERID 3

	#define UDMA_CTR_CFG_CG_UART0_BIT (UDMA_UART0_PERID)
	#define UDMA_CTR_CFG_CG_SPIM0_BIT (UDMA_SPIM0_PERID)
	#define UDMA_CTR_CFG_CG_SPIM1_BIT (UDMA_SPIM1_PERID)
	#define UDMA_CTR_CFG_CG_SPIM2_BIT (UDMA_SPIM2_PERID)


	#define UDMA_CTR_CFG_RST_UART0_BIT (UDMA_UART0_PERID)
	#define UDMA_CTR_CFG_RST_SPIM0_BIT (UDMA_SPIM1_PERID)
	#define UDMA_CTR_CFG_RST_SPIM1_BIT (UDMA_SPIM2_PERID)
	#define UDMA_CTR_CFG_RST_SPIM2_BIT (UDMA_SPIM3_PERID)

	typedef struct _uDMA_UART_t{
		uint32_t RX_SADDR;
		uint32_t RX_SIZE;
		uint32_t RX_CFG;
		unsigned:32;
		uint32_t TX_SADDR;
		uint32_t TX_SIZE;
		uint32_t TX_CFG;
		unsigned:32;
		uint32_t STATUS;
		uint32_t SETUP;
		uint32_t ERROR;
		uint32_t IRQ_EN;
		uint32_t VALID;
		uint32_t DATA;
	}uDMA_UART_t;


	typedef struct _uDMASPI_t{
		uint32_t RX_SADDR;
		uint32_t RX_SIZE;
		uint32_t RX_CFG;
		unsigned :32;
		uint32_t TX_SADDR;
		uint32_t TX_SIZE;
		uint32_t TX_CFG;
		unsigned :32;
		uint32_t CMD_SADDR;
		uint32_t CMD_SIZE;
		uint32_t CMD_CFG;
		unsigned :32;
	}__attribute__((packed)) uDMA_SPI_t;


	#define UDMA_UART0 ((volatile uDMA_UART_t *)(UDMA_UART_OFFSET+uDMA_BASE))
	#define UDMA_SPIM0 ((volatile uDMA_SPI_t *)(UDMA_SPIM0_OFFSET+uDMA_BASE))
	#define UDMA_SPIM1 ((volatile uDMA_SPI_t *)(UDMA_SPIM1_OFFSET+uDMA_BASE))
	#define UDMA_SPIM2 ((volatile uDMA_SPI_t *)(UDMA_SPIM2_OFFSET+uDMA_BASE))


	//bits for UART
	#define UDMA_UART_RX_CFG_CLR_BIT 		6
	#define UDMA_UART_RX_CFG_PENDING_BIT 	5
	#define UDMA_UART_RX_CFG_EN_BIT			4
	#define UDMA_UART_RX_CFG_CONT_BIT		0

	#define UDMA_UART_TX_CFG_CLR_BIT 		6
	#define UDMA_UART_TX_CFG_PENDING_BIT 	5
	#define UDMA_UART_TX_CFG_EN_BIT			4
	#define UDMA_UART_TX_CFG_CONT_BIT		0

	#define UDMA_UART_STATUS_TXBUSY_BIT		0
	#define UDMA_UART_STATUS_RXBUSY_BIT		1

	#define UDMA_UART_SETUP_CLKDIV15_BIT	31
	#define UDMA_UART_SETUP_CLKDIV14_BIT	30
	#define UDMA_UART_SETUP_CLKDIV13_BIT	29
	#define UDMA_UART_SETUP_CLKDIV12_BIT	28
	#define UDMA_UART_SETUP_CLKDIV11_BIT	27
	#define UDMA_UART_SETUP_CLKDIV10_BIT	26
	#define UDMA_UART_SETUP_CLKDIV9_BIT		25
	#define UDMA_UART_SETUP_CLKDIV8_BIT		24
	#define UDMA_UART_SETUP_CLKDIV7_BIT		23
	#define UDMA_UART_SETUP_CLKDIV6_BIT		22
	#define UDMA_UART_SETUP_CLKDIV5_BIT		21
	#define UDMA_UART_SETUP_CLKDIV4_BIT		20
	#define UDMA_UART_SETUP_CLKDIV3_BIT		19
	#define UDMA_UART_SETUP_CLKDIV2_BIT		18
	#define UDMA_UART_SETUP_CLKDIV1_BIT		17
	#define UDMA_UART_SETUP_CLKDIV0_BIT		16
	#define UDMA_UART_SETUP_RXEN_BIT		9
	#define UDMA_UART_SETUP_TXEN_BIT		8
	#define UDMA_UART_SETUP_CLEANFIFO_BIT	5
	#define UDMA_UART_SETUP_POLLINGEN_BIT	4
	#define UDMA_UART_SETUP_STOPBITS_BIT	3
	#define UDMA_UART_SETUP_BITLGTH1_BIT	2
	#define UDMA_UART_SETUP_BITLGTH0_BIT	1
	#define UDMA_UART_SETUP_PARITY_BIT		0

	#define UDMA_UART_ERROR_RX_PARITY_BIT	1
	#define UDMA_UART_ERROR_RX_OVR_BIT		0

	#define UDMA_UART_IRQEN_ERROR_BIT		1
	#define UDMA_UART_IRQEN_RX_BIT			0

	#define UDMA_UART_VALID_READY_BIT		0


	//SPIM bits
	#define UDMA_SPIM_RX_CFG_CLR_BIT 		6
	#define UDMA_SPIM_RX_CFG_PENDING_BIT 	5
	#define UDMA_SPIM_RX_CFG_EN_BIT			4
	#define UDMA_SPIM_RX_CFG_DATASIZE1_BIT	2
	#define UDMA_SPIM_RX_CFG_DATASIZE0_BIT	1
	#define UDMA_SPIM_RX_CFG_CONT_BIT		0

	#define UDMA_SPIM_TX_CFG_CLR_BIT 		6
	#define UDMA_SPIM_TX_CFG_PENDING_BIT 	5
	#define UDMA_SPIM_TX_CFG_EN_BIT			4
	#define UDMA_SPIM_TX_CFG_DATASIZE1_BIT	2
	#define UDMA_SPIM_TX_CFG_DATASIZE0_BIT	1
	#define UDMA_SPIM_TX_CFG_CONT_BIT		0


	#define UDMA_SPIM_CMD_CFG_CLR_BIT 		6
	#define UDMA_SPIM_CMD_CFG_PENDING_BIT 	5
	#define UDMA_SPIM_CMD_CFG_EN_BIT		4
	#define UDMA_SPIM_CMD_CFG_DATASIZE1_BIT	2
	#define UDMA_SPIM_CMD_CFG_DATASIZE0_BIT	1
	#define UDMA_SPIM_CMD_CFG_CONT_BIT		0

	//CMD macro
	#define uDMA_SPICMD_CFG(divider, pol, phase) ((divider & 0xFF) | (pol << 8) | (phase << 9))
	#define uDMA_SPICMD_SOT(csnum) ((csnum & 0x3) | (1 << 28))
	#define uDMA_SPICMD_SEND_CMD(cmd, cmdbits, lsbfirst, qspi) ((cmd & 0xFFFF) | (((cmdbits-1) & 0xF) << 16) | (lsbfirst << 26) | (qspi << 27) | (2 << 28))
	#define uDMA_SPICMD_SEND_DUMMY(dummybits) (((dummybits & 0x1F) << 16) | (4 << 28))
	#define uDMA_SPICMD_WAIT(evtidorcycle, type) (((evtidorcycle & 0x7F)<<0) | ((type & 0x3) << 8) | (5 << 28))
	#define uDMA_SPICMD_TX(words, wordbits, wordsfortransfer, lsbfirst, qspi ) ((words-1) | ((wordbits-1)<<16) | ((wordsfortransfer & 0x3) << 21) | (lsbfirst << 26) | (qspi << 27) | (6 << 28))
	#define uDMA_SPICMD_RX(words, wordbits, wordsfortransfer, lsbfirst, qspi ) ((words-1) | ((wordbits-1)<<16) | ((wordsfortransfer & 0x3) << 21) | (lsbfirst << 26) | (qspi << 27) | (7 << 28))
	#define uDMA_SPICMD_RPT(numrepeat) ((numrepeat & 0xFFFF) | (8 << 28))
	#define uDMA_SPICMD_EOT(genevt) (genevt | (9 << 28))
	//#define SPICMD_FULLDUPL(numbits, lsbfirst) (numbits-1 | (lsbfirst << 26) | (0x0C<<28))
	#define uDMA_SPICMD_FULLDUPL(words, wordbits, wordsfortransfer, lsbfirst ) ((words-1) | ((wordbits-1)<<16) | ((wordsfortransfer & 0x3) << 21) | (lsbfirst << 26) | (0xC << 28))

#endif /* UDMA_H_ */
