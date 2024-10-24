/*
*---------------------------------------------------------------
*                        Lvgl Font Tool                         
*                                                               
* 注:使用unicode编码                                              
* 注:本字体文件由Lvgl Font Tool V0.4 生成                          
* 作者:阿里(qq:617622104)                                         
*---------------------------------------------------------------
*/


#include "lvgl.h"
#include "string.h"
#include "flash.h"
#include "ff.h"
#include "font_update.h"


typedef struct{
    uint16_t min;
    uint16_t max;
    uint8_t  bpp;
    uint8_t  reserved[3];
}x_header_t;
typedef struct{
    uint32_t pos;
}x_table_t;
typedef struct{
    uint8_t adv_w;
    uint8_t box_w;
    uint8_t box_h;
    int8_t  ofs_x;
    int8_t  ofs_y;
    uint8_t r;
}glyph_dsc_t;


static x_header_t __g_xbf_hd = {
    .min = 0x0020,
    .max = 0x9fa0,
    .bpp = 4,
};


static uint8_t __g_font_buf[275];//如bin文件存在SPI FLASH可使用此buff


static uint8_t *__user_font_getdata(int offset, int size){
    //如字模保存在SPI FLASH, SPIFLASH_Read(__g_font_buf,offset,size);
    //如字模已加载到SDRAM,直接返回偏移地址即可如:return (uint8_t*)(sdram_fontddr+offset);
    EN25QXX_Read(__g_font_buf,offset+ftinfo.f24addr,size);

    return __g_font_buf;
}
static const uint8_t opa4_table[16] = {0,  17, 34,  51,
                                       68, 85, 102, 119,
                                       136, 153, 170, 187,
                                       204, 221, 238, 255
};

static const uint8_t opa2_table[4] = {0, 85, 170, 255};


static const void * __user_font_get_bitmap(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf)
{
    uint32_t unicode_letter = g_dsc->gid.index;
    uint8_t * bitmap_out = draw_buf->data;
    const lv_font_t *font = g_dsc->resolved_font;
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *) font->dsc;

    if(unicode_letter >__g_xbf_hd.max || unicode_letter<__g_xbf_hd.min ) {
        return NULL;
    }

    uint32_t unicode_offset = sizeof(x_header_t)+(unicode_letter-__g_xbf_hd.min)*4;
    uint32_t *p_pos = (uint32_t *)__user_font_getdata(unicode_offset, 4);
    if( p_pos[0] != 0 ) {
        uint32_t pos = p_pos[0];
        glyph_dsc_t * gdsc = (glyph_dsc_t*)__user_font_getdata(pos, sizeof(glyph_dsc_t));
        glyph_dsc_t gdscPoint = {0};
        memcpy(&gdscPoint, gdsc, sizeof(glyph_dsc_t));
        gdsc = &gdscPoint;

        int32_t gsize = (int32_t) gdsc->box_w * gdsc->box_h;
        if(gsize == 0) return NULL;
        if(fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
            const uint8_t * bitmap_in = __user_font_getdata(pos+sizeof(glyph_dsc_t), gdsc->box_w*gdsc->box_h*__g_xbf_hd.bpp/8);
            uint8_t * bitmap_out_tmp = bitmap_out;
            int32_t i = 0;
            int32_t x, y;
            uint32_t stride = lv_draw_buf_width_to_stride(gdsc->box_w, LV_COLOR_FORMAT_A8);
            if(fdsc->bpp == 1) {
                for(y = 0; y < gdsc->box_h; y ++) {
                    for(x = 0; x < gdsc->box_w; x++, i++) {
                        i = i & 0x7;
                        if(i == 0) bitmap_out_tmp[x] = (*bitmap_in) & 0x80 ? 0xff : 0x00;
                        else if(i == 1) bitmap_out_tmp[x] = (*bitmap_in) & 0x40 ? 0xff : 0x00;
                        else if(i == 2) bitmap_out_tmp[x] = (*bitmap_in) & 0x20 ? 0xff : 0x00;
                        else if(i == 3) bitmap_out_tmp[x] = (*bitmap_in) & 0x10 ? 0xff : 0x00;
                        else if(i == 4) bitmap_out_tmp[x] = (*bitmap_in) & 0x08 ? 0xff : 0x00;
                        else if(i == 5) bitmap_out_tmp[x] = (*bitmap_in) & 0x04 ? 0xff : 0x00;
                        else if(i == 6) bitmap_out_tmp[x] = (*bitmap_in) & 0x02 ? 0xff : 0x00;
                        else if(i == 7) {
                            bitmap_out_tmp[x] = (*bitmap_in) & 0x01 ? 0xff : 0x00;
                            bitmap_in++;
                        }
                    }
                    bitmap_out_tmp += stride;
                }
            }
            else if(fdsc->bpp == 2) {
                for(y = 0; y < gdsc->box_h; y ++) {
                    for(x = 0; x < gdsc->box_w; x++, i++) {
                        i = i & 0x3;
                        if(i == 0) bitmap_out_tmp[x] = opa2_table[(*bitmap_in) >> 6];
                        else if(i == 1) bitmap_out_tmp[x] = opa2_table[((*bitmap_in) >> 4) & 0x3];
                        else if(i == 2) bitmap_out_tmp[x] = opa2_table[((*bitmap_in) >> 2) & 0x3];
                        else if(i == 3) {
                            bitmap_out_tmp[x] = opa2_table[((*bitmap_in) >> 0) & 0x3];
                            bitmap_in++;
                        }
                    }
                    bitmap_out_tmp += stride;
                }

            }
            else if(fdsc->bpp == 4) {
                for(y = 0; y < gdsc->box_h; y ++) {
                    for(x = 0; x < gdsc->box_w; x++, i++) {
                        i = i & 0x1;
                        if(i == 0) {
                            bitmap_out_tmp[x] = opa4_table[(*bitmap_in) >> 4];
                        }
                        else if(i == 1) {
                            bitmap_out_tmp[x] = opa4_table[(*bitmap_in) & 0xF];
                            bitmap_in++;
                        }
                    }
                    bitmap_out_tmp += stride;
                }
            }
            return draw_buf;
        }
    }
    return NULL;
}


static bool __user_font_get_glyph_dsc(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next) {
    if( unicode_letter>__g_xbf_hd.max || unicode_letter<__g_xbf_hd.min ) {
        return NULL;
    }
    uint32_t unicode_offset = sizeof(x_header_t)+(unicode_letter-__g_xbf_hd.min)*4;  // 每个字符的编码都是4个字节
    uint32_t *p_pos = (uint32_t *)__user_font_getdata(unicode_offset, 4);  // 这里应该是取该编码字符具体位图信息地址（基础信息+字模位置）
    if( p_pos[0] != 0 ) {
        glyph_dsc_t * gdsc = (glyph_dsc_t*)__user_font_getdata(p_pos[0], sizeof(glyph_dsc_t));
        dsc_out->adv_w = (uint8_t)gdsc->adv_w;
        dsc_out->box_h = (uint8_t)gdsc->box_h;
        dsc_out->box_w = (uint8_t)gdsc->box_w;
        dsc_out->ofs_x = (int8_t)gdsc->ofs_x;
        dsc_out->ofs_y = (int8_t)gdsc->ofs_y;
        //dsc_out->bpp   = __g_xbf_hd.bpp; //改成下面
        dsc_out->format = (uint8_t)__g_xbf_hd.bpp;
        dsc_out->gid.index = unicode_letter; //官方工具生成的字库赋的值就是uicode的id
        dsc_out->is_placeholder = false;
        return true;
    }
    return false;
}


#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
        static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
        .glyph_bitmap = NULL,
        .glyph_dsc = NULL,
        .cmaps = NULL,
        .kern_dsc = NULL,
        .kern_scale = 0,
        .cmap_num = 0,
        .bpp = 4,
        .kern_classes = 0,
        .bitmap_format = LV_FONT_FMT_TXT_PLAIN,
#if LVGL_VERSION_MAJOR == 8
        .cache = &cache
#endif
};
//SJbangkaijianti,,-1
//字模高度：25
//XBF字体,外部bin文件
lv_font_t SanJiBang20 = {
    .get_glyph_bitmap = __user_font_get_bitmap,
    .get_glyph_dsc = __user_font_get_glyph_dsc,
    .line_height = 25,
    .base_line = 0,
};

