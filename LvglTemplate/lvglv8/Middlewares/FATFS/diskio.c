/**
 ****************************************************************************************************
 * @file        diskio.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       FATFS底层(diskio) 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 F429开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20220420
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./MALLOC/malloc.h"
#include "./BSP/NAND/nand.h"
#include "./BSP/NAND/ftl.h"
#include "./SYSTEM/usart/usart.h"
#include "./FATFS/source/diskio.h"
#include "./BSP/SDIO/sdio_sdcard.h"
#include "./BSP/NORFLASH/norflash.h"


#define SD_CARD     0       /* SD卡,卷标为0 */
#define EX_FLASH    1       /* 外部spi flash,卷标为1 */
#define EX_NAND     2       /* 外部NAND FLASH卷标为2 */

/**
 * 对于25Q256 FLASH芯片, 我们规定前 25M 给FATFS使用, 25M以后
 * 用于存放字库，字库占用6.01M.剩余部分给客户自己用 
 */

#define SPI_FLASH_SECTOR_SIZE   512
#define SPI_FLASH_SECTOR_COUNT  25 * 1024 * 2   /* 25Q256, 前25M字节给FATFS占用 */
#define SPI_FLASH_BLOCK_SIZE    8               /* 每个BLOCK有8个扇区 */
#define SPI_FLASH_FATFS_BASE    0               /* FATFS 在外部FLASH的起始地址 从0开始 */

/**
 * NAND FLASH全部归FATFS管理
 */
uint32_t NAND_FLASH_SECTOR_COUNT;
uint8_t NAND_FLASH_BLOCK_SIZE;
 

/**
 * @brief       获得磁盘状态
 * @param       pdrv : 磁盘编号0~9
 * @retval      无
 */
DSTATUS disk_status (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    return RES_OK;
}

/**
 * @brief       初始化磁盘
 * @param       pdrv : 磁盘编号0~9
 * @retval      无
 */
DSTATUS disk_initialize (
    BYTE pdrv       /* Physical drive number to identify the drive */
)
{
    uint8_t res = 0;

    switch (pdrv)
    {
        case SD_CARD:           /* SD卡 */
            res = sd_init();    /* SD卡初始化 */
            break;

        case EX_FLASH:          /* 外部flash */
            norflash_init(); 
            break;

        case EX_NAND:           /* NAND FLASH */
            res = nand_init();
            break;

        default:
            res = 1;
            break;
    }

    if (res)
    {
        return  STA_NOINIT;
    }
    else
    {
        return 0; /* 初始化成功*/
    }
}

/**
 * @brief       读扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 数据接收缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要读取的扇区数
 * @retval      无
 */
DRESULT disk_read (
    BYTE pdrv,      /* Physical drive number to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Sector address in LBA */
    UINT count      /* Number of sectors to read */
)
{
    uint8_t res = 0;

    if (!count)return RES_PARERR;   /* count不能等于0，否则返回参数错误 */

    switch (pdrv)
    {
        case SD_CARD:       /* SD卡 */
            res = sd_read_disk(buff, sector, count);

            while (res)     /* 读出错 */
            {
                //printf("sd rd error:%d\r\n", res);
                sd_init();  /* 重新初始化SD卡 */
                res = sd_read_disk(buff, sector, count);
            }
            break;

        case EX_FLASH:      /* 外部flash */
            for (; count > 0; count--)
            {
                norflash_read(buff, SPI_FLASH_FATFS_BASE + sector * SPI_FLASH_SECTOR_SIZE, SPI_FLASH_SECTOR_SIZE);
                sector++;
                buff += SPI_FLASH_SECTOR_SIZE;
            }
            res = 0;
            break;

        case EX_NAND:      /* 外部NAND */
            res = ftl_read_sectors(buff, sector, 512, count);
            break;

        default:
            res = 1;
    }

    /* 处理返回值，将返回值转成ff.c的返回值 */
    if (res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR; 
    }
}

/**
 * @brief       写扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 发送数据缓存区首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要写入的扇区数
 * @retval      无
 */
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive number to identify the drive */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Sector address in LBA */
    UINT count          /* Number of sectors to write */
)
{
    uint8_t res = 0;

    if (!count)return RES_PARERR;   /* count不能等于0，否则返回参数错误 */

    switch (pdrv)
    {
        case SD_CARD:       /* SD卡 */
            res = sd_write_disk((uint8_t *)buff, sector, count);

            while (res)     /* 写出错 */
            {
                //printf("sd wr error:%d\r\n", res);
                sd_init();  /* 重新初始化SD卡 */
                res = sd_write_disk((uint8_t *)buff, sector, count);
            }
            break;

        case EX_FLASH:      /* 外部flash */
            for (; count > 0; count--)
            {
                norflash_write((uint8_t *)buff, SPI_FLASH_FATFS_BASE + sector * SPI_FLASH_SECTOR_SIZE, SPI_FLASH_SECTOR_SIZE);
                sector++;
                buff += SPI_FLASH_SECTOR_SIZE;
            }
            res = 0;
            break;
            
        case EX_NAND:      /* 外部NAND */
            res = ftl_write_sectors((uint8_t *)buff, sector, 512, count);
            break;

        default:
            res = 1;
    }

    /* 处理返回值，将返回值转成ff.c的返回值 */
    if (res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR; 
    }
}

/**
 * @brief       获取其他控制参数
 * @param       pdrv   : 磁盘编号0~9
 * @param       ctrl   : 控制代码
 * @param       buff   : 发送/接收缓冲区指针
 * @retval      无
 */
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive number (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res;

    if (pdrv == SD_CARD)    /* SD卡 */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(DWORD *)buff = 512;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff = g_sd_card_info_handle.LogBlockSize;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = g_sd_card_info_handle.LogBlockNbr;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else if (pdrv == EX_FLASH)  /* 外部FLASH */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(WORD *)buff = SPI_FLASH_SECTOR_SIZE;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff = SPI_FLASH_BLOCK_SIZE;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = SPI_FLASH_SECTOR_COUNT;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else if (pdrv == EX_NAND)  /* 外部NAND FLASH */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(WORD *)buff = 512;       /* NAND FLASH扇区强制为512字节大小 */
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff = nand_dev.page_mainsize / 512;    /* block大小，定义成一个page的大小 */
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = nand_dev.valid_blocknum * nand_dev.block_pagenum * nand_dev.page_mainsize / 512;    /* NAND FLASH的总扇区大小 */
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else
    {
        res = RES_ERROR;    /* 其他的不支持 */
    }
    
    return res;
}




















