/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sdio_sdcard.h"
#include "flash.h"
#include "malloc.h"
/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#define SD_CARD	 0  //SD卡,卷标为0
#define EX_FLASH 1	//外部flash,卷标为1


#define FLASH_SECTOR_SIZE 	512
//对于W25Q128
//前12M字节给fatfs用,12M字节后,用于存放字库,字库占用3.09M.	剩余部分,给客户自己用
u16	    FLASH_SECTOR_COUNT=2048*12;	//W25Q1218,前12M字节给FATFS占用每个扇区512Byte
#define FLASH_BLOCK_SIZE   	8     	//每个BLOCK有8个扇区8*512 = 4096

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    u8 res=0;
    switch(pdrv)
    {
        case SD_CARD://SD卡
            res=SD_Init();//SD卡初始化
            break;
        case EX_FLASH://外部flash
            EN25QXX_Init();
            FLASH_SECTOR_COUNT=2048*12;//W25Q1218,前12M字节给FATFS占用
            break;
        default:
            res=1;
    }
    if(res)return  STA_NOINIT;
    else return 0; //初始化成功
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    u8 res=0;
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误
    switch(pdrv)
    {
        case SD_CARD://SD卡
            res=SD_ReadDisk(buff,sector,count);
            while(res)//读出错
            {
                SD_Init();	//重新初始化SD卡
                res=SD_ReadDisk(buff,sector,count);
                //printf("sd rd error:%d\r\n",res);
            }
            break;
        case EX_FLASH://外部flash
            for(;count>0;count--)
            {
                EN25QXX_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
                sector++;
                buff+=FLASH_SECTOR_SIZE;
            }
            res=0;
            break;
        default:
            res=1;
    }
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res==0x00)return RES_OK;
    else return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
    u8 res=0;
    if (!count)return RES_PARERR;//count不能等于0，否则返回参数错误
    switch(pdrv)
    {
        case SD_CARD://SD卡
            res=SD_WriteDisk((u8*)buff,sector,count);
            while(res)//写出错
            {
                SD_Init();	//重新初始化SD卡
                res=SD_WriteDisk((u8*)buff,sector,count);
                //printf("sd wr error:%d\r\n",res);
            }
            break;
        case EX_FLASH://外部flash
            for(;count>0;count--)
            {
                EN25QXX_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
                sector++;
                buff+=FLASH_SECTOR_SIZE;
            }
            res=0;
            break;
        default:
            res=1;
    }
    //处理返回值，将SPI_SD_driver.c的返回值转成ff.c的返回值
    if(res == 0x00)return RES_OK;
    else return RES_ERROR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res;
    if(pdrv==SD_CARD)//SD卡
    {
        switch(cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;
            case GET_SECTOR_SIZE:
                *(DWORD*)buff = 512;
                res = RES_OK;
                break;
            case GET_BLOCK_SIZE:
                *(WORD*)buff = SDCardInfo.BlockSize;
                res = RES_OK;
                break;
            case GET_SECTOR_COUNT:
                *(DWORD*)buff = SDCardInfo.BlockNbr/512;
                res = RES_OK;
                break;
            default:
                res = RES_PARERR;
                break;
        }
    }else if(pdrv==EX_FLASH)	//外部FLASH
    {
        switch(cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;
            case GET_SECTOR_SIZE:
                *(WORD*)buff = FLASH_SECTOR_SIZE;
                res = RES_OK;
                break;
            case GET_BLOCK_SIZE:
                *(WORD*)buff = FLASH_BLOCK_SIZE;
                res = RES_OK;
                break;
            case GET_SECTOR_COUNT:
                *(DWORD*)buff = FLASH_SECTOR_COUNT;
                res = RES_OK;
                break;
            default:
                res = RES_PARERR;
                break;
        }
    }else res=RES_ERROR;//其他的不支持
    return res;
}

//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
