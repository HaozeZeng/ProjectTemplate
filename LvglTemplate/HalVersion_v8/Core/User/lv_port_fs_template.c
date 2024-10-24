/**
 * @file lv_port_fs_templ.c
 *
 */

/*Copy this file as "lv_port_fs.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_fs_template.h"
#include "lvgl.h"
#include "fatfs_app.h"
#include "ff.h"
#include "malloc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode);

static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p);

static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br);

static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw);

static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence);

static lv_fs_res_t fs_size(lv_fs_drv_t *drv, void *file_p, uint32_t *size_p);

static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p);

static void *fs_dir_open(lv_fs_drv_t *drv, const char *path);

static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn);

static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *rddir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_fs_init(void) {
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    /*Add a simple drive to open images*/
    static lv_fs_drv_t fs_drv;
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = '0';
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your Storage device and File system.*/
static void fs_init(void) {
    /*E.g. for FatFS initialize the SD card and FatFS itself*/
    FATFS_Init();
    f_mount(fs[0], "0:", 1);        //挂载SD卡
    printf("f_mount OK\r\n");
    /*You code here*/
}

/**
 * Open a file
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode      read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return          a file descriptor or NULL on error
 */
static void *fs_open(lv_fs_drv_t *drv, const char *path, lv_fs_mode_t mode) {
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    FIL *file = (FIL *) mymalloc(SRAMIN, sizeof(FIL));  // 分配内存以存储文件对象
    if (file == NULL) {
        return NULL;  // 内存分配失败
    }
    void *f = NULL;

    if (mode == LV_FS_MODE_WR) {
        /*Open a file for write*/
        res = f_open(file, path, FA_WRITE | FA_CREATE_ALWAYS);       /*Add your code here*/
    } else if (mode == LV_FS_MODE_RD) {
        /*Open a file for read*/
        res = f_open(file, path, FA_READ);        /*Add your code here*/
    } else if (mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) {
        /*Open a file for read and write*/
        res = f_open(file, path, FA_READ | FA_WRITE | FA_OPEN_ALWAYS);        /*Add your code here*/
    } else {
        myfree(SRAMIN, file);  // 释放内存
        return NULL; // 不支持的模式
    }

    if (res != FR_OK) {
        myfree(SRAMIN, file);  // 释放内存
        return NULL; // 打开文件失败
    }

    return file;
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with lv_ufs_open)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t *drv, void *file_p) {
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    FIL *file = (FIL *) file_p;

    // 调用 FatFs 的 f_close 函数关闭文件
    res = f_close(file);
    // 释放文件对象的内存
    myfree(SRAMIN, file);

    // 根据 f_close 的返回值设置 LVGL 的返回值
    if (res == FR_OK) {
        return LV_FS_RES_OK;  // 成功关闭文件
    } else {
        return LV_FS_RES_UNKNOWN;  // 关闭文件时发生错误
    }
    /*Add your code here*/

    return res;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t *drv, void *file_p, void *buf, uint32_t btr, uint32_t *br) {
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Add your code here*/
    FIL *file = (FIL *)file_p;

    // 调用 FatFs 的 f_read 函数读取数据
    UINT bytesRead;  // 用于存储实际读取的字节数
    res = f_read(file, buf, btr, &bytesRead);

    // 设置实际读取的字节数
    *br = bytesRead;

    // 根据 f_read 的返回值设置 LVGL 的返回值
    if (res == FR_OK) {
        return LV_FS_RES_OK;  // 成功读取数据
    } else {
        return LV_FS_RES_UNKNOWN;  // 读取文件时发生错误
    }
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btr       Bytes To Write
 * @param br        the number of real written bytes (Bytes Written). NULL if unused.
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t *drv, void *file_p, const void *buf, uint32_t btw, uint32_t *bw) {
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Add your code here*/
    FIL *file = (FIL *)file_p;
    // 调用 FatFs 的 f_write 函数写入数据
    UINT bytesWritten;  // 用于存储实际写入的字节数
    res = f_write(file, buf, btw, &bytesWritten);

    // 设置实际写入的字节数
    *bw = bytesWritten;

    // 根据 f_write 的返回值设置 LVGL 的返回值
    if (res == FR_OK) {
        return LV_FS_RES_OK;  // 成功写入数据
    } else {
        return LV_FS_RES_UNKNOWN;  // 写入文件时发生错误
    }
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable. (opened with lv_ufs_open )
 * @param pos       the new position of read write pointer
 * @param whence    tells from where to interpret the `pos`. See @lv_fs_whence_t
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t *drv, void *file_p, uint32_t pos, lv_fs_whence_t whence) {
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;

    /*Add your code here*/
    FIL *file = (FIL *)file_p;

    // 根据 whence 的值设置偏移量
    if (whence == LV_FS_SEEK_SET) {
        // 从文件开头设置位置
        res = f_lseek(file, pos);
    } else if (whence == LV_FS_SEEK_CUR) {
        // 从当前指针位置设置位置
        res = f_lseek(file, f_tell(file) + pos);
    } else if (whence == LV_FS_SEEK_END) {
        // 从文件末尾设置位置
        res = f_lseek(file, f_size(file) + pos);
    } else {
        return LV_FS_RES_NOT_IMP;  // 不支持的 whence 值
    }

    // 根据 f_lseek 的返回值设置 LVGL 的返回值
    if (res == FR_OK) {
        return LV_FS_RES_OK;  // 成功设置位置
    } else {
        return LV_FS_RES_UNKNOWN;  // 设置位置时发生错误
    }
}

/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a file_t variable.
 * @param pos_p     pointer to to store the result
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t *drv, void *file_p, uint32_t *pos_p) {
    /*Add your code here*/
    FIL *file = (FIL *)file_p;

    // 获取当前文件指针的位置
    DWORD pos = f_tell(file);

    // 检查 f_tell 的返回值
    if (pos == (DWORD)-1) {
        return LV_FS_RES_UNKNOWN;  // 获取位置时发生错误
    }

    // 将位置值存储在 pos_p 中
    *pos_p = (uint32_t)pos;

    return LV_FS_RES_OK;  // 成功获取位置
}

/**
 * Initialize a 'lv_fs_dir_t' variable for directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param path      path to a directory
 * @return          pointer to the directory read descriptor or NULL on error
 */
static void *fs_dir_open(lv_fs_drv_t *drv, const char *path) {
    DIR *dir = mymalloc(SRAMIN,sizeof(DIR));  // 分配内存以存储目录对象
    if (dir == NULL) {
        return NULL;  // 内存分配失败
    }

    FRESULT res = f_opendir(dir, path);  // 打开目录
    if (res != FR_OK) {
        myfree(SRAMIN,dir);  // 释放内存
        return NULL;  // 打开目录失败
    }

    return dir;  // 返回目录对象
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @param fn        pointer to a buffer to store the filename
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t *drv, void *rddir_p, char *fn) {
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    /*Add your code here*/
    DIR *dir = (DIR *)rddir_p;  // 将 void* 转换为 DIR*
    FILINFO fno;  // 文件信息结构体
    res = f_readdir(dir, &fno);  // 读取下一个目录项
    if (res == FR_OK) {
        if (fno.fname[0] != '\0') {  // 如果读取到有效的文件名
            sprintf(fn, "/%s", fno.fname);  // 将文件名格式化为 LVGL 所需格式
            return LV_FS_RES_OK;  // 成功读取文件名
        } else {
            return LV_FS_RES_UNKNOWN;  // 没有更多文件
        }
    } else {
        return LV_FS_RES_UNKNOWN;  // 读取目录时发生错误
    }
}

/**
 * Close the directory reading
 * @param drv       pointer to a driver where this function belongs
 * @param rddir_p   pointer to an initialized 'lv_fs_dir_t' variable
 * @return          LV_FS_RES_OK: no error or  any error from @lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t *drv, void *rddir_p) {
    lv_fs_res_t res = LV_FS_RES_NOT_IMP;
    DIR *dir = (DIR *)rddir_p;  // 将 void* 转换为 DIR*

    res = f_closedir(dir);  // 关闭目录
    myfree(SRAMIN,dir);  // 释放目录对象的内存

    if (res == FR_OK) {
        return LV_FS_RES_OK;  // 成功关闭目录
    } else {
        return LV_FS_RES_UNKNOWN;  // 关闭目录时发生错误
    }
    /*Add your code here*/

}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
