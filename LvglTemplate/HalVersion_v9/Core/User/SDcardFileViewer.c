#include "app.h"
#include "string.h"
#include "malloc.h"
#include "lvgl.h"
#include "ff.h"

LV_FONT_DECLARE(SanJiBang12)
LV_FONT_DECLARE(SanJiBang16)
LV_FONT_DECLARE(SanJiBang20)
#define scr_get_width() lv_obj_get_width(lv_scr_act())
#define scr_get_height() lv_obj_get_height(lv_scr_act())


FRESULT MenuInit(const char *FileFullPath, lv_obj_t *PreviousLevelFolder);

static void BtnDirEvent_Cb(lv_event_t *e);

static void BtnExitEvent_Cb(lv_event_t *e);

static void BtnFileEvent_Cb(lv_event_t *e);


static lv_style_t font_style;
static lv_obj_t *MainMenu;
struct FileDsc {
    char *FileFullPath;//Full FileFullPath
    char *FileName;//Just FileName
};


/**
 *
 * @param path:Must be similar to "0:"
 * @param font:SanJiBang_12, SanJiBang_16,SanJiBang_20
 */
void SDcardFileViewerStart(const char *path, enum SupportFont font) {
    //FileFullPath need be "0:"
    lv_style_init(&font_style);
    switch (font){
        case SanJiBang_12:
            lv_style_set_text_font(&font_style, &SanJiBang12);
            break;
        case SanJiBang_16:
            lv_style_set_text_font(&font_style, &SanJiBang16);
            break;
        case SanJiBang_20:
            lv_style_set_text_font(&font_style, &SanJiBang20);
            break;
        default:
            lv_style_set_text_font(&font_style, &SanJiBang16);
    }
    MainMenu = lv_list_create(lv_scr_act());
    lv_obj_center(MainMenu);
    lv_obj_set_size(MainMenu, scr_get_width(), scr_get_height());
    MenuInit(path, MainMenu);
}


//EventGroups
static void BtnDirEvent_Cb(lv_event_t *e) {
    lv_event_code_t Event = lv_event_get_code(e);
    lv_obj_t *Trigger = lv_event_get_target(e);
    lv_obj_t *PreviousLevelFolder = (lv_obj_t *) (lv_event_get_user_data(e));
    if (Event == LV_EVENT_CLICKED) {
        lv_obj_set_style_bg_color(Trigger, lv_color_hex(0x47a7f1), LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(Trigger, 150, LV_STATE_FOCUSED);
        lv_obj_clear_flag(PreviousLevelFolder, LV_OBJ_FLAG_HIDDEN);
    }
}

static void BtnExitEvent_Cb(lv_event_t *e) {
    lv_event_code_t Event = lv_event_get_code(e);
    lv_obj_t *Trigger = lv_event_get_target(e);
    lv_obj_t *PreviousLevelFolder = (lv_obj_t *) (lv_event_get_user_data(e));// userdata is ChildPage
    if (Event == LV_EVENT_CLICKED) {
        lv_obj_set_style_bg_color(Trigger, lv_color_hex(0x47a7f1), LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(Trigger, 150, LV_STATE_FOCUSED);
        lv_obj_add_flag(PreviousLevelFolder, LV_OBJ_FLAG_HIDDEN);
    }
}

#define FileMaxBufSize 2048

static void BtnFileEvent_Cb(lv_event_t *e) {
    FIL file;
    FRESULT result;
    UINT bytes_read = 0;
    char readbuffer[100];
    char FileMaxContent[FileMaxBufSize] = "";
    struct FileDsc *FileDscInfo = (struct FileDsc *) (lv_event_get_user_data(e));
    lv_event_code_t Event = lv_event_get_code(e);
    lv_obj_t *Trigger = lv_event_get_target(e);

    if (Event == LV_EVENT_CLICKED) {
        lv_obj_set_style_bg_color(Trigger, lv_color_hex(0x47a7f1), LV_STATE_FOCUSED);
        lv_obj_set_style_bg_opa(Trigger, 150, LV_STATE_FOCUSED);
        result = f_open(&file, FileDscInfo->FileFullPath, FA_READ);
        if (result != FR_OK) {
            LV_LOG_WARN("open error: %d", result);
            return;
        }
        lv_obj_t *DispWindow = lv_win_create(lv_scr_act());
        lv_obj_center(DispWindow);
        lv_obj_set_size(DispWindow, scr_get_width(), scr_get_height());
        lv_obj_t *title = lv_win_add_title(DispWindow, FileDscInfo->FileName);
        lv_obj_add_style(title, &font_style, 0);

        lv_obj_t *WindowExitBtn = lv_win_add_button(DispWindow, LV_SYMBOL_CLOSE, 60);
        lv_obj_add_event_cb(WindowExitBtn, BtnExitEvent_Cb, LV_EVENT_CLICKED, DispWindow);

        lv_obj_t *WindowDispContent = lv_win_get_content(DispWindow);
        lv_obj_t *WindowContent = lv_label_create(WindowDispContent);
        lv_label_set_long_mode(WindowContent, LV_LABEL_LONG_WRAP);
        do {
            result = f_read(&file, readbuffer, sizeof(readbuffer) - 1, &bytes_read);  // ÿ������ȡ 99 ���ֽ�
            if (result != FR_OK) {
                LV_LOG_ERROR("FILE Read Error: %d", result);
                break;
            }
            readbuffer[bytes_read] = '\0';//avoid err
            if ((strlen(FileMaxContent) + bytes_read) < FileMaxBufSize) {//limit
                strncat(FileMaxContent, readbuffer, bytes_read);
            } else {
                LV_LOG_WARN("Buffer overflow: file content too large.");
                break;
            }
        } while (bytes_read > 0);
        lv_obj_add_style(WindowContent, &font_style, 0);
        lv_label_set_text(WindowContent, FileMaxContent);
    }
}


#define DirPathMaxBufSize 128
#define FilepathMaxBufSize 128

FRESULT MenuInit(const char *FolderFullPath, lv_obj_t *PreviousLevelFolder) {
    FRESULT res;
    DIR dir;
    static FILINFO fno;
    struct FileDsc *FileDsc;
    char Filepath[FilepathMaxBufSize];
    char FolderPath[DirPathMaxBufSize];
    res = f_opendir(&dir, FolderFullPath);
    if (res == FR_OK) {
        snprintf(FolderPath, sizeof(FolderPath), "%s/..", FolderFullPath);
        lv_obj_t *FolderBackBtn = lv_list_add_btn(PreviousLevelFolder, LV_SYMBOL_DIRECTORY, NULL);
        lv_obj_t *FolderBackTitle = lv_label_create(FolderBackBtn);
        lv_label_set_long_mode(FolderBackTitle, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_label_set_text(FolderBackTitle, FolderPath);
        lv_obj_add_style(FolderBackTitle, &font_style, 0);
        lv_obj_add_event_cb(FolderBackBtn, BtnExitEvent_Cb, LV_EVENT_CLICKED, PreviousLevelFolder);

        while (1) {
            res = f_readdir(&dir, &fno);
            if (res != FR_OK || fno.fname[0] == 0) {
                LV_LOG_INFO("Folder %s Reach End!", FileFullPath);
                break;
            }
            if ((fno.fattrib & AM_DIR) && !(fno.fattrib & AM_SYS)) {
                snprintf(Filepath, sizeof(Filepath), "%s/%s", FolderFullPath, fno.fname);
                lv_obj_t *FolderBtn = lv_list_add_btn(PreviousLevelFolder, LV_SYMBOL_DIRECTORY, NULL);
                lv_obj_t *label = lv_label_create(FolderBtn);
                lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
                lv_label_set_text(label, fno.fname);
                lv_obj_add_style(label, &font_style, 0);

                lv_obj_t *NextLevelFolder = lv_list_create(lv_scr_act());
                lv_obj_center(NextLevelFolder);
                lv_obj_set_size(NextLevelFolder, scr_get_width(), scr_get_height());
                lv_obj_add_flag(NextLevelFolder, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_event_cb(FolderBtn, BtnDirEvent_Cb, LV_EVENT_CLICKED, NextLevelFolder);
                //to go through all Dir
                res = MenuInit(Filepath, NextLevelFolder);
                if (res != FR_OK) {
                    LV_LOG_ERROR("NextLevelFolder Init fail! res:%d", res);
                    break;
                }
            } else if (!(fno.fattrib & AM_SYS)) {
                snprintf(Filepath, sizeof(Filepath), "%s/%s", FolderFullPath, fno.fname);
                lv_obj_t *FileBtn = lv_list_add_btn(PreviousLevelFolder, LV_SYMBOL_FILE, NULL);
                lv_obj_t *FileTitle = lv_label_create(FileBtn);
                lv_label_set_long_mode(FileTitle, LV_LABEL_LONG_SCROLL_CIRCULAR);
                lv_label_set_text(FileTitle, fno.fname);
                lv_obj_add_style(FileTitle, &font_style, 0);

                FileDsc = (struct FileDsc *) mymalloc(SRAMIN, sizeof(struct FileDsc));
                if (FileDsc != NULL) {
                    FileDsc->FileFullPath = (char *) mymalloc(SRAMIN, strlen(Filepath) + 1);
                    FileDsc->FileName = (char *) mymalloc(SRAMIN, strlen(fno.fname) + 1);
                    if (FileDsc->FileFullPath != NULL && FileDsc->FileName != NULL) {
                        strcpy((char *) FileDsc->FileFullPath, Filepath);
                        strcpy((char *) FileDsc->FileName, fno.fname);
                        lv_obj_add_event_cb(FileBtn, BtnFileEvent_Cb, LV_EVENT_CLICKED, FileDsc);
                    } else {
                        myfree(SRAMIN, FileDsc->FileFullPath);
                        myfree(SRAMIN, FileDsc->FileName);
                        myfree(SRAMIN, FileDsc);
                    }
                } else {
                    LV_LOG_ERROR("FileDsc Malloc Error");
                }
            }
        }
    } else {
        LV_LOG_ERROR("DIR %s OPEN Fail", FolderFullPath);
    }
    f_closedir(&dir);
    LV_LOG_INFO("Folder Close %s!", FolderFullPath);
    return res;
}

