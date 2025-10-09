/*
 * memoryOperation.c
 *
 *  Created on: Nov 25, 2024
 *      Author: cezary.macik
 */

#include "main.h"
#include "spi.h"
#include "EVE.h"
#include "HAL.h"
#include "fatfs.h"
#include "iwdg.h"

#include "memoryOperation.h"

uint16_t norStatus;
nor_t Nor;
lfs_t Lfs;
lfs_file_t file;

struct lfs_config LfsConfig = {0};

FRESULT FatFsResult;
FATFS 	SdFatFs;
FIL 	SdCardFile;

volatile uint8_t dataCopy;

//const uint32_t font0_offset1 = 1000;

void nor_cs_assert()
{
	HAL_GPIO_WritePin(SPI2_FLASH_CS_GPIO_Port, SPI2_FLASH_CS_Pin, GPIO_PIN_RESET);
}

void nor_cs_deassert()
{
	HAL_GPIO_WritePin(SPI2_FLASH_CS_GPIO_Port, SPI2_FLASH_CS_Pin, GPIO_PIN_SET);
}

void nor_spi_tx(uint8_t *pData, uint32_t Size)
{
	if(dataCopy)
	{
		HAL_SPI_Transmit(&hspi2, pData, Size, 10);
	}
	else
	{
		dma_tx_complete = 0; // Resetuj flagę
		HAL_SPI_Transmit_DMA(&hspi2, pData, Size);

		while (!dma_tx_complete); // Czekaj na zakończenie transmisji
	}
}

void nor_spi_rx(uint8_t *pData, uint32_t Size)
{
	if(dataCopy)
	{
		HAL_SPI_Receive(&hspi2, pData, Size, 10);
	}
	else
	{
		dma_rx_complete = 0; // Resetuj flagę
		HAL_SPI_Receive_DMA(&hspi2, pData, Size);

		while (!dma_rx_complete); // Czekaj na zakończenie odbioru
	}
}

void nor_delay_us(uint32_t us)
{
	int i = us;

//	if(us > 0)
//		asm("nop");

	while(i > 0)
	{
		asm("nop");
		i--;
	}
}

void __init_nor()
{
	uint8_t retStat;

	Nor.config.CsAssert 	=	nor_cs_assert;
	Nor.config.CsDeassert 	= 	nor_cs_deassert;
	Nor.config.DelayUs 		= 	nor_delay_us;
	Nor.config.SpiRxFxn	 	=	nor_spi_rx;
	Nor.config.SpiTxFxn 	= 	nor_spi_tx;

	retStat = NOR_Init(&Nor);

	char buf[30] = {0};
	snprintf(buf, sizeof(buf), "ErrFlashMemStat: %d", retStat);
	eveMsgMultiLine(10, 10, 18, 20, 2, buf, ">>Remove a SD Card!<<");

	if(retStat != NOR_OK)
			Error_Handler();

	/*if(NOR_Init(&Nor) != NOR_OK)
		Error_Handler();*/
}

int _fs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
	if(NOR_ReadSector(&Nor, buffer, block, off, size) == NOR_OK)
		return 0;

	return LFS_ERR_IO;
}

int _fs_write(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
	if(NOR_WriteSector(&Nor, (uint8_t *)buffer, block, off, size) == NOR_OK)
		return 0;

	return LFS_ERR_IO;
}

int _fs_erase(const struct lfs_config *c, lfs_block_t block)
{
	if(NOR_EraseSector(&Nor, block) == NOR_OK)
		return 0;

	return LFS_ERR_IO;
}

int _fs_sync(const struct lfs_config *c)
{
	return 0;
}

void __init_littlefs()
{
	int error;

	LfsConfig.read_size = 256;
	LfsConfig.prog_size = 256;
	LfsConfig.block_size = Nor.info.u16SectorSize;
	LfsConfig.block_count = Nor.info.u32SectorCount;
	LfsConfig.cache_size = Nor.info.u16PageSize;
	//LfsConfig.lookahead_size = 16;
	LfsConfig.lookahead_size = LfsConfig.block_count / 8;
	LfsConfig.block_cycles = 500;

	LfsConfig.read = 	_fs_read;
	LfsConfig.prog = 	_fs_write;
	LfsConfig.erase = 	_fs_erase;
	LfsConfig.sync = 	_fs_sync;

	error = lfs_mount(&Lfs, &LfsConfig);

	if(error != LFS_ERR_OK)
	{
		lfs_format(&Lfs, &LfsConfig);
		error = lfs_mount(&Lfs, &LfsConfig);

		if(error != LFS_ERR_OK)
			Error_Handler();
	}
}

void __init_storage()
{
	__init_nor();
	__init_littlefs();
}

int copyFileSdToFlash(const char *sd_path, const char *flash_path)
{
	#define KB_256 262144

    FIL file;
    FRESULT res;
    UINT bytesRead;
    uint8_t buffer[1024]; // Większy bufor dla lepszej wydajności
    lfs_file_t lfs_file;
    int err;
    uint32_t bytesWrittenTotal = 0;
    uint32_t fileSize = 0;
    int16_t prevPercent = -1;

    // Otwieranie pliku na SD
    res = f_open(&file, sd_path, FA_READ);

    if (res != FR_OK)
    {
    	//sendUartString("Nie mozna otworzyc pliku na SD. Kod bledu: %d\n", res);
        return -1;
    }

    fileSize = f_size(&file);

    if(fileSize > KB_256) /*wincy niz 256 kb*/
    {
       	eveMsgMultiLine(10, 70, 18, 20, 3,
      					  "FILE on SD TO LARGE!",
      					  "Check file size!",
      					  "Please unplug&plug power supply!");
       	f_close(&file);
        while(1);
    }

    // Otwieranie pliku na LittleFS
    err = lfs_file_open(&Lfs, &lfs_file, flash_path, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
    if (err < 0)
    {
        //sendUartString("Nie można otworzyć pliku na LittleFS. Kod błędu: %d\n", err);
        f_close(&file);
        return -1;
    }

    // Kopiowanie danych
    while (1)
    {
        res = f_read(&file, buffer, sizeof(buffer), &bytesRead);
        if (res != FR_OK || bytesRead == 0) // Błąd lub koniec pliku
        {
            if (res != FR_OK)
                asm("nop");
            	//sendUartString("Błąd odczytu z SD. Kod błędu: %d\n", res);
            break;
        }

        err = lfs_file_write(&Lfs, &lfs_file, buffer, bytesRead);
        if (err < 0)
        {
            //sendUartString("Błąd zapisu do LittleFS. Kod błędu: %d\n", err);
            break;
        }

        bytesWrittenTotal += bytesRead;
        int16_t percent = (bytesWrittenTotal * 100) / fileSize;

        if (percent % 5 == 0 && percent != prevPercent) // Aktualizuj pasek co 5%
        {
            prevPercent = percent;

            // Aktualizacja paska postępu
            EVE_LIB_BeginCoProList();
            EVE_CMD_DLSTART();
            EVE_CLEAR_COLOR_RGB(0, 0, 0);
            EVE_CLEAR(1, 1, 1);
            EVE_CMD_TEXT(60, 440, 18, 0, "SD to FLASH:");
            EVE_CMD_PROGRESS(65, 460, 120, 12, 0, percent, 100);
            EVE_DISPLAY();
            EVE_CMD_SWAP();
            EVE_LIB_EndCoProList();
            EVE_LIB_AwaitCoProEmpty();
        }
    }

    // Zamykanie plików
    f_close(&file);
    lfs_file_close(&Lfs, &lfs_file);

    // Sprawdzenie, czy operacja zakończyła się sukcesem
    if (res == FR_OK && err >= 0)
    {
        //sendUartString("Kopiowanie zakończone pomyślnie!\n");
        return 0;
    }
    else
    {
        //sendUartString("Kopiowanie zakończone niepowodzeniem. E:%d\n", err);
        return -1;
    }
}


//uint8_t loadAssetFromSD(void)
//{
//	uint8_t buf[128];
//	DWORD file_size;
//	UINT bytes_read;
//	int16_t prev_percent = -1;
//	uint32_t bytes_written_total = 0;
//	uint32_t start_addr = 0;
//
//	//if(!init)
//	{
//		FatFsResult = f_open(&SdCardFile, "0:font.raw", FA_READ);
//
//		if (FatFsResult != FR_OK)
//		{
//			sendUartString("font.raw! err:%d\n\r", FatFsResult);
//			eveMsg(50, "SD N/D!");
//			return 0;
//		}
//		else
//		{
//			sendUartString("font.rawh reading!\n\r");
//			eveMsg(50, "SD OK!");
//			HAL_Delay(500);
//		}
//
//		file_size = f_size(&SdCardFile);
//
//		while (bytes_written_total < file_size)
//		{
//			FatFsResult = f_read(&SdCardFile, buf, sizeof(buf), &bytes_read);
//
//			if (FatFsResult != FR_OK || bytes_read == 0)
//			{
//				eveMsg(40, "READ ERR!");
//		   		break;
//			}
//
//			EVE_LIB_WriteDataToRAMG(buf, bytes_read, start_addr + bytes_written_total);
//
//			bytes_written_total += bytes_read;
//
//			int16_t percent = (bytes_written_total * 100) / file_size;
//
//			 if (percent != prev_percent)
//			 {
//				 prev_percent = percent;
//				 //sendUartString("Loading: %d%%!\n\r", percent);
//
//				 EVE_LIB_BeginCoProList();
//				 EVE_CMD_DLSTART();
//				 EVE_CLEAR_COLOR_RGB(0,0,0);
//				 EVE_CLEAR(1,1,1);
//				 EVE_CMD_SETROTATE(2);
//				 EVE_CMD_ROMFONT(1, 31);
//				 EVE_CMD_TEXT(40, 10, 18, 0,  "LWDO LIFT SERVICE S.A.");
//
//				 /*--komunikaty diagnostyczne*/
//				 EVE_CMD_TEXT(40, 30, 18, 0,  "CRC ACT:"); EVE_CMD_NUMBER(120, 30, 18, 0,  crcRes);
//				 EVE_CMD_TEXT(40, 50, 18, 0,  "CRC MEM:"); EVE_CMD_NUMBER(120, 50, 18, 0,  crcResFromEEPROM);
//
//				 if(crcRes == crcResFromEEPROM) EVE_CMD_TEXT(40, 70, 18, 0,  "CRC OK!");
//				 else EVE_CMD_TEXT(40, 70, 18, 0,  "CRC ERROR!");
//				 /*--komunikaty diagnostyczne end*/
//
//				 EVE_CMD_TEXT(60, 440, 18, 0, "SD LOAD:");
//				 EVE_CMD_PROGRESS(65, 460, 120, 12, 0, percent, 100);
//				 EVE_DISPLAY();
//				 EVE_DISPLAY();
//				 EVE_CMD_SWAP();
//				 EVE_LIB_EndCoProList();
//				 EVE_LIB_AwaitCoProEmpty();
//
//				#ifdef IWDG_ON
//				//HAL_IWDG_Refresh(&hiwdg);
//				#endif
//			 }
//		}
//
//		f_close(&SdCardFile);
//
//		sendUartString("logo.rawh read!\n\r");
//
//		return 1;
//	}
//
//	return 0;
//}

int32_t __attribute__((optimize("-Ofast"))) loadAssetFromFlash(uint8_t fileNo, char *path, uint32_t offset)
{
    uint8_t buf[1024];
    int16_t prev_percent = -1;
    uint32_t bytes_written_total = 0;
    uint32_t start_addr = 0;

    start_addr = start_addr + offset;

    lfs_file_t file; // Struktura dla otwartego pliku LittleFS

    //if (!init)
    {
        // Otwórz plik z LittleFS
        int err = lfs_file_open(&Lfs, &file, path, LFS_O_RDONLY);

        //sendUartString("font_ff.raw! Stat:%d\n\r", err);

        if (err < 0)
        {
            //sendUartString("Load: %s! err:%d \n\r", path, err);
            //eveMsg(50, "File N/D!");

            return 0;
        }
        else
        {
            //sendUartString("font_ff.raw reading!\n\r");
            //eveMsg(50, "FLASH OK!");
            HAL_Delay(500);
        }

        // Pobierz rozmiar pliku
        lfs_soff_t file_size = lfs_file_size(&Lfs, &file);

        //sendUartString("Ld: %lu \n\r", file_size);

        if (file_size < 0)
        {
            //sendUartString("file_size err:%lld\n\r", file_size);
            lfs_file_close(&Lfs, &file);

            return 0;
        }

        uint32_t start_time = HAL_GetTick();

        while (bytes_written_total < file_size)
        {
            lfs_ssize_t bytes_read = lfs_file_read(&Lfs, &file, buf, sizeof(buf));

            //uint32_t bytes_read = sizeof(buf);

            if (bytes_read <= 0)
            {
                //eveMsg(40, "READ ERR!");
                break;
            }

            EVE_LIB_WriteDataToRAMG(buf, bytes_read, start_addr + bytes_written_total);

            bytes_written_total += bytes_read;

            int16_t percent = (bytes_written_total * 100) / file_size;

            if (percent != prev_percent /*percent % 5 == 0 && percent != prev_percent*/)
            {
                prev_percent = percent;

                // Wyświetl pasek postępu
                EVE_LIB_BeginCoProList();
                EVE_CMD_DLSTART();
                EVE_CLEAR_COLOR_RGB(0, 0, 0);
                EVE_CLEAR(1, 1, 1);
                EVE_CMD_SETROTATE(2);
                EVE_CMD_ROMFONT(1, 31);
                EVE_CMD_TEXT(EVE_OPT_CENTERX, 10, 18, EVE_OPT_CENTERX, "LWDO LIFT SERVICE S.A.");
                //EVE_CMD_TEXT(EVE_DISP_HEIGHT/2 - 20, 440, 18, EVE_OPT_CENTERX, "ODCZYT PLIKOW");
                EVE_CMD_NUMBER(EVE_DISP_HEIGHT/2 - 10, 440, 18, EVE_OPT_CENTERX, fileNo);
                EVE_CMD_TEXT(EVE_DISP_HEIGHT/2 + 10, 440, 18, EVE_OPT_CENTERX, "/3:");
                EVE_CMD_PROGRESS(75, 460, 120, 12, 0, percent, 100);
                EVE_DISPLAY();
                EVE_DISPLAY();
                EVE_CMD_SWAP();
                EVE_LIB_EndCoProList();
                EVE_LIB_AwaitCoProEmpty();

            }
        }

       //uint32_t elapsed_time = HAL_GetTick() - start_time;
       //sendUartString("Ldt: %lu \n\r", elapsed_time);

        lfs_file_close(&Lfs, &file);
        //sendUartString("font_ff.raw read!\n\r");

        return 1;
    }

    return 0;
}


