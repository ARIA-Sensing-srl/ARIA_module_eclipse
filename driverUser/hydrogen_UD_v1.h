/**
 * @file hydrogen_UD_v1.h
 * @brief Hydrogen user level drivers
 *
 *  Created on: Jan 25, 2024\n
 *  Author: ARIA Sensing\n

 */


#include "ipc.h"
#include "ipc_types.h"

#ifndef HYDROGEN_UD_V1_H_
#define HYDROGEN_UD_V1_H_

/** I channel macro */
#define HYDRUD_DRV_CHI 0
/** Q channel macro */
#define HYDRUD_DRV_CHQ 1
/** Maximum word length */
#define HYDRUD_CODE_LGTH_MAX 64

/**
 * Acquisition mode
 */
typedef enum _HydrUDrv_acqMode_t{
	HUACQ_SINGLE = 0,/**< Single acquisition */
	HUACQ_CONT       /**< Continous acquisition */
}HydrUDrv_acqMode_t;

/**
 * Transmitter power setting
 */
typedef enum _HydrUDrv_pwrlvl_t {
	HUTXPWR_OFF = 0, /**< HUTXPWR_OFF */
	HUTXPWR_LOW = 1, /**< HUTXPWR_LOW */
	HUTXPWR_LVL2 = 2,/**< HUTXPWR_LVL2 */
	HUTXPWR_LVL3 = 3,/**< HUTXPWR_LVL3 */
	HUTXPWR_MID = 4, /**< HUTXPWR_MID */
	HUTXPWR_LVL5 = 5,/**< HUTXPWR_LVL5 */
	HUTXPWR_LVL6 = 6,/**< HUTXPWR_LVL6 */
	HUTXPWR_HIGH = 7,/**< HUTXPWR_HIGH */
}HydrUDrv_pwrlvl_t;


/**
 * Power-down modes
 */
typedef enum _HydrUDrv_pwrsave_t{
	HUPWR_OFF = 0,/**< None */
	HUPWR_0 = 1,  /**< ADC and controller clock gating */
	HUPWR_1 = 2,  /**< Transceiver off, LDOs on */
	HUPWR_2 = 3,  /**< Transceiver and LDO off */
}HydrUDrv_pwrsave_t;

/** @brief Scanning configuration  */
typedef ipc_scan_seq_t HydrUDrv_channels_t ;
/**  @brief Acquistion hanlder */
typedef ipc_cmd_acq_t HydrUDrv_acqHandler_t;

/**
 * User driver handler
 */
typedef struct _HydrUDriver_t{
	uint32_t ID;						//identifier, placeholder
	uint32_t freqCarrier_MHz;			//center frequency 	(MHz)
	uint32_t bandwidth_MHz;				//pulse bandwidth	(MHz)
	float xmin_range;					//range (meters)
	float xmax_range;					//range (meters)
	float xoffset;						//offset in meters
	uint16_t VGA_gain[2];				//IQ channels VGA
	uint16_t iterations;				//number of iterations
	uint16_t frame_rate;				//frame rate (Hz)
	uint16_t frame_bins;				//number of bins
	HydrUDrv_pwrlvl_t txpwr;			//pwrlvl
	HydrUDrv_acqMode_t acqMode;
	HydrUDrv_pwrsave_t pwrSaveMode;
	int8_t code[HYDRUD_CODE_LGTH_MAX];	//code
	uint8_t codeSize;
	void (*pDelay)(uint32_t);			//pointer to SW delay function
	volatile uint32_t (*pGetSysTick)();	//pointer for systick function
}HydrUDriver_t;


/**
 * Initialize the user driver handler
 * @param h
 * @return SUCCESS/FAIL
 */
int HydrUDrv_init(HydrUDriver_t * h);
/**
 * Configure transceiver
 * @param h
 * @param xtalfreq_Hz crystal driver frequency
 * @param txfbMask calibration feedback transmitter mask channel
 * @param rxfbMask calibration feedback receiver mask channel
 * @return SUCCESS/FAIL
 */
int HydrUDrv_configure(HydrUDriver_t * h, uint32_t xtalfreq_Hz, uint8_t txfbMask, uint8_t rxfbMask);
/**
 * Set the acquisition range
 * @param h
 * @param min minimum range (meters)
 * @param max maximum range (meters)
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_range(HydrUDriver_t *h, float min, float max);
/**
 * Set the carrier frequency
 * @param h
 * @param freqMHz carrier frequency in MHz
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_carrier(HydrUDriver_t *h, uint32_t freqMHz);
/**
 * Set the pulse bandwidth
 * @param h
 * @param freqMHz bandwidth in MHz
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_bandwidth(HydrUDriver_t *h, uint32_t freqMHz);
/**
 * Set the VGA gain
 * @param h
 * @param iCH I channel gain
 * @param qCH Q channel gain
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_gain(HydrUDriver_t *h, uint16_t iCH, uint16_t qCH);
/**
 * Set the frame rate (used in continuous acquisition mode
 * @param h
 * @param FPS
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_framerate(HydrUDriver_t *h, uint16_t FPS);
/**
 * Set the transmitted code word
 * @param h
 * @param pcode pointer to int8 array with ternary code (-1,0,1)
 * @param size size of the code word
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_code(HydrUDriver_t *h, int8_t * pcode, uint8_t size);
/**
 * Set the transmitter output power
 * @param h
 * @param pwr
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_pwr(HydrUDriver_t *h, HydrUDrv_pwrlvl_t pwr);
/**
 * Set the acquisition mode (continuous/single)
 * @param h
 * @param mode
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_acqmode(HydrUDriver_t *h, HydrUDrv_acqMode_t mode);
/**
 * Set the integration level (nmber of acquisition for each frame)
 * @param h
 * @param integrations
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_integrations(HydrUDriver_t *h, uint16_t integrations);
/**
 * Set the acquisition frame offset
 * @param h
 * @param offset frame offset in meter
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_offset(HydrUDriver_t *h, float offset);
/**
 * Set the power save mode
 * @param h
 * @param pwrmode
 * @return SUCCESS/FAIL
 */
int HydrUDrv_set_pwrSave(HydrUDriver_t *h, HydrUDrv_pwrsave_t pwrmode);
/**
 * Get the current range setting in meters
 * @param h
 * @param pmin
 * @param pmax
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_range(HydrUDriver_t *h, float *pmin, float *pmax);
/**
 * Get the current carrier frequency (MHz)
 * @param h
 * @param pfreqMHz
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_carrier(HydrUDriver_t *h, uint32_t *pfreqMHz);
/**
 * Get the current carrier frequency
 * @param h
 * @param pfreqMHz
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_bandwidth(HydrUDriver_t *h, uint32_t *pfreqMHz);
/**
 * Get the current VGAs gain
 * @param h
 * @param piCH
 * @param pqCH
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_gain(HydrUDriver_t *h, uint16_t *piCH, uint16_t *pqCH);
/**
 * Get the current frame rate
 * @param h
 * @param pFPS
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_framerate(HydrUDriver_t *h, uint16_t *pFPS);
/**
 * Get the number of bins of each stream
 * @param h
 * @param pframebins
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_framebins(HydrUDriver_t *h, uint32_t *pframebins);
/**
 * Get the current codeword
 * @param h
 * @param pcode
 * @param psize
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_code(HydrUDriver_t *h, int8_t *pcode, uint8_t * psize);
/**
 * Get the current transmitter power level
 * @param h
 * @param ppwr
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_pwr(HydrUDriver_t *h, HydrUDrv_pwrlvl_t *ppwr);
/**
 * Get the current acquisition mode
 * @param h
 * @param pmode
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_acqmode(HydrUDriver_t *h, HydrUDrv_acqMode_t *pmode);
/**
 * Get the current integrations level
 * @param h
 * @param pintegrations
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_integrations(HydrUDriver_t *h, uint16_t *pintegrations);
/**
 * Get the current frame offset (meters)
 * @param h
 * @param poffset
 * @return SUCCESS/FAIL
 */
int HydrUDrv_get_offset(HydrUDriver_t *h, float *poffset);

/**
 * Start a single frame acquisition\n
 * The acquisition mode must be set accordingly before calling this function
 * @param h
 * @param pacqHandler
 * @return SUCCESS/FAIL
 */
int HydrUDrv_start_single(HydrUDriver_t * h, HydrUDrv_acqHandler_t* pacqHandler);

/**
 * Start continuous mode acquisition\n
 * The acquisition mode must be set accordingly before calling this function\n
 * If two buffers are provided, ping-pong buffer mode is automatically inferred\n
 * @param h
 * @param pbuf0
 * @param pbuf1
 * @return SUCCESS/FAIL
 */
int HydrUDrv_start_cont(HydrUDriver_t * h, HydrUDrv_acqHandler_t* pbuf0, HydrUDrv_acqHandler_t* pbuf1);

/**
 * Stop acquisition
 * @param h
 * @return SUCCESS/FAIL
 */
int HydrUDrv_stop_cont(HydrUDriver_t *h);

/**
 * Inform the subsystem that a PLL update occurred
 * @param h
 * @return SUCCESS/FAIL
 */
int HydrUDrv_pll_update(HydrUDriver_t *h);

#define HYDRUDRV_SUCCESS 0
#define HYDRUDRV_FAIL -1
#define HYDRUDRV_TIMEOUT -2
#define HYDRUDRV_INVALID_PARAMS -3
#define HYDRUDRV_INVALID_STATUS -4


#endif /* HYDROGEN_UD_V1_H_ */
