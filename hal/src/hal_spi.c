/*
 * hal_spi.c
 *
 *  Created on: Dec 4, 2023
 *      Author: ARIA Sensing
 *      Copyright: ARIA Sensing, 2023 - 
 */

#include "hal_spi.h"
#include "stddef.h"
#include "hal_evt_ctl.h"
#include "hal_udma.h"
#include "stdbool.h"
#include "eventctrl.h"
#include "hal_clkmgr.h"


#define forceu32(x) (*(uint32_t*)&(x))


static uDMA_SPI_t * priv_getPeripheral(int chID);
static int priv_SPIEnable(int chID);
static int priv_SPIDisable(int chID);
static void priv_SPIFDEOTCb (void *); //full duplex EOT
static int priv_enableSPIEOTEvt(int channel, void (*func)(void*), void * pargs);
static int priv_disableSPIEOTEvt(int channel);


hal_spi_status hal_spi_init(hal_spi_t* ph,hal_spi_config * pconfig){
	ph->channel = pconfig->channel;
	ph->handler = priv_getPeripheral(ph->channel);
	if (ph->handler == NULL)
		return HAL_SPI_INVALID_PARAM;
	//setup
	uint32_t perclk;


	perclk  = hal_clkmgh_get_peripheral_freq();

	priv_SPIEnable(ph->channel);
	int clkDiv = perclk/pconfig->baudrate;
	ph->cfg = uDMA_SPICMD_CFG(clkDiv, pconfig->cpol, pconfig->cpha);
	ph->handler->CMD_CFG |= (1 << UDMA_SPIM_CMD_CFG_CLR_BIT);
	ph->handler->RX_CFG |= (1 << UDMA_SPIM_RX_CFG_CLR_BIT);
	ph->handler->TX_CFG |= (1 << UDMA_SPIM_TX_CFG_CLR_BIT);
	asm volatile("nop");
	asm volatile("nop");
	asm volatile("nop");
	ph->handler->CMD_CFG = 0;
	ph->handler->RX_CFG = 0;
	ph->handler->TX_CFG = 0;

	return HAL_SPI_SUCCESS;
}

hal_spi_status hal_spi_deinit(hal_spi_t* ph){
	priv_SPIDisable(ph->channel);
	return HAL_SPI_SUCCESS;
}

//normal SPI modes, single channel fullduplex
hal_spi_status hal_spi_fullduplex(hal_spi_t*ph , uint8_t * pwrite, uint8_t * pread, int size, uint8_t chipselect){
	volatile uint32_t spicmd[4];
	volatile uDMA_SPI_t * pspi = ph->handler;
	volatile bool complete = false;

	forceu32(pspi->RX_SADDR) = (uint32_t)pread;
	forceu32(pspi->RX_SIZE) = size; //DEBUG
	forceu32(pspi->RX_CFG) = (1 << UDMA_SPIM_RX_CFG_EN_BIT);

	spicmd[0] = ph->cfg; //created on startup for configuration command
	spicmd[1] = uDMA_SPICMD_SOT(chipselect);
	spicmd[2] = uDMA_SPICMD_FULLDUPL(size,8,0,0);
	spicmd[3] = uDMA_SPICMD_EOT(1);

	priv_enableSPIEOTEvt(ph->channel, priv_SPIFDEOTCb, (void*)&complete);


	forceu32(pspi->CMD_SADDR) = (uint32_t)spicmd;
	forceu32(pspi->CMD_SIZE) = 4*4; //bytes
	forceu32(pspi->CMD_CFG) = ((1<<UDMA_SPIM_CMD_CFG_EN_BIT)|(2<<UDMA_SPIM_CMD_CFG_DATASIZE0_BIT));

	forceu32(pspi->TX_SADDR) = (uint32_t)pwrite;
	forceu32(pspi->TX_SIZE) = size;
	forceu32(pspi->TX_CFG) = ((1<<UDMA_SPIM_TX_CFG_EN_BIT)); //enable

	while(!complete){
		asm volatile ("nop");
	}
	priv_disableSPIEOTEvt(ph->channel);
	return 0;

}


//SPI qpi mode
hal_spi_status hal_spi_QSPITransaction(hal_spi_t* ph, hal_qspi_transaction_t* ptrans, uint8_t * pbuffer, int size, int chipselect){
	volatile uint32_t spicmd[6];
	volatile uDMA_SPI_t * pspi = ph->handler;
	volatile bool complete = false;



	//setup command
	int numCmds = 0;
	spicmd[numCmds++] = ph->cfg; //created on startup for configuration command
	spicmd[numCmds++] = uDMA_SPICMD_SOT(chipselect);
	if (ptrans->cmdSize){
		//MSB of the command must be at bit15 (doesn't matter if shorter)
		uint32_t cmdFormatted = ptrans->cmdSize == 1 ? ((uint32_t)ptrans->cmd)<<8 : ptrans->cmd;
		spicmd[numCmds++] = uDMA_SPICMD_SEND_CMD(cmdFormatted, 8*ptrans->cmdSize, 0, ptrans->cmdQPIMode);
	}
	if (ptrans->dummyCycles){
		spicmd[numCmds++] = uDMA_SPICMD_SEND_DUMMY(ptrans->dummyCycles);
	}
	if (ptrans->transactionType ==  HAL_QSPI_TRANSACTION_RD){
		//
		spicmd[numCmds++] = uDMA_SPICMD_RX(size, 8, 0, 0, ptrans->IOQPIMode);
		//setup
		forceu32(pspi->RX_SADDR) = (uint32_t)pbuffer;
		forceu32(pspi->RX_SIZE) = size; //DEBUG
		forceu32(pspi->RX_CFG) = (1 << UDMA_SPIM_RX_CFG_EN_BIT);
	}else{
		spicmd[numCmds++] = uDMA_SPICMD_TX(size, 8, 0, 0 , ptrans->IOQPIMode);
	}
	spicmd[numCmds++] = uDMA_SPICMD_EOT(1);
	//last command
	priv_enableSPIEOTEvt(ph->channel, priv_SPIFDEOTCb, (void*)&complete);


	forceu32(pspi->CMD_SADDR) = (uint32_t)spicmd;
	forceu32(pspi->CMD_SIZE) = 4*numCmds; //bytes
	forceu32(pspi->CMD_CFG) = ((1<<UDMA_SPIM_CMD_CFG_EN_BIT)|(2<<UDMA_SPIM_CMD_CFG_DATASIZE0_BIT));

	if (ptrans->transactionType == HAL_QSPI_TRANSACTION_WR){
		forceu32(pspi->TX_SADDR) = (uint32_t)pbuffer;
		forceu32(pspi->TX_SIZE) = size;
		forceu32(pspi->TX_CFG) = ((1<<UDMA_SPIM_TX_CFG_EN_BIT)); //enable
	}

	while(!complete){
		asm volatile ("nop");
	}
	priv_disableSPIEOTEvt(ph->channel);
	return 0;

}



static uDMA_SPI_t * priv_getPeripheral(int ID){
	uDMA_SPI_t * pret = NULL;
	switch (ID){
		case HAL_SPI_CH0:
			pret = UDMA_SPIM0;
			break;
		case HAL_SPI_CH1:
			pret = UDMA_SPIM1;
			break;
		default:
			break;
	}
	return pret;
}

static int priv_SPIEnable(int chID){
	int perID;
	switch (chID){
		case HAL_SPI_CH0:
			perID = UDMA_SPIM0_PERID;
			break;
		case HAL_SPI_CH1:
			perID = UDMA_SPIM1_PERID;
			break;
		default:
			return -1;
			break;
	}
	hal_udma_peripheral_reset(perID);
	hal_udma_peripheral_ck_enable(perID);
	return 0;
}


static int priv_SPIDisable(int chID){
	int perID;
	switch (chID){
		case HAL_SPI_CH0:
			perID = UDMA_SPIM0_PERID;
			break;
		case HAL_SPI_CH1:
			perID = UDMA_SPIM1_PERID;
			break;
		default:
			return -1;
			break;
	}

	hal_udma_peripheral_ck_disable(perID);
	return 0;
}

static int priv_enableSPIEOTEvt(int channel, void (*func)(void*), void * pargs){
	int evtID;
	switch (channel){
		case HAL_SPI_CH0:
			evtID = EVC_EVT_SPI0_EOT_ID;
			break;
		case HAL_SPI_CH1:
			evtID = EVC_EVT_SPI1_EOT_ID;
			break;
		default:
			return -1;
			break;
	}
	hal_evt_ctl_register(evtID, func, pargs);
	hal_evt_ctl_unmask(evtID);
	return 0;
}
static int priv_disableSPIEOTEvt(int channel){
	int evtID;
	switch (channel){
		case HAL_SPI_CH0:
			evtID = EVC_EVT_SPI0_EOT_ID;
			break;
		case HAL_SPI_CH1:
			evtID = EVC_EVT_SPI1_EOT_ID;
			break;
		default:
			return -1;
			break;
	}
	hal_evt_ctl_unregister(evtID); //remove event and mask generation
	return 0;
}

static void priv_SPIFDEOTCb  (void  * pargs){
	bool * prcomplete = (bool*)pargs;
	*prcomplete = true;
}

