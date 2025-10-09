/*
 * memoryOperation.h
 *
 *  Created on: Nov 25, 2024
 *      Author: cezary.macik
 */

#ifndef INC_MEMORYOPERATION_H_
#define INC_MEMORYOPERATION_H_

#include "nor.h"
#include "lfs.h"

void __init_nor();
void __init_littlefs();
void __init_storage();
int copyFileSdToFlash(const char *sd_path, const char *flash_path);
uint8_t loadAssetFromSD(void);
int32_t loadAssetFromFlash(uint8_t fileNo, char *path, uint32_t offset);

extern FRESULT 	FatFsResult;
extern FATFS 	SdFatFs;
extern FIL 		SdCardFile;
extern UINT bytesRead;

extern volatile uint8_t 	dataCopy;
extern volatile uint8_t dma_tx_complete;
extern volatile uint8_t dma_rx_complete;
extern lfs_t Lfs;
extern struct lfs_config LfsConfig;

#endif /* INC_MEMORYOPERATION_H_ */
