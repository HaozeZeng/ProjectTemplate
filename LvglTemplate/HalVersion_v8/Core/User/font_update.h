#ifndef _font_update_H
#define _font_update_H

#include "system.h"
// 带pack和不带pack的不同点
//首先结构体变大,不够位就填充,但编译器不会读这些位
//带pack按单字节填充该怎样就怎样
//不带pack编译器会对齐不够就填充无意义的数但是编译器实际不会读这些无意义的数,
//但是如果你非要在这些无意义的地方写 "((u8*)dest)[i]""结果这些地方不会放到正确的位置,结构体出现错位,填充的数据湮灭"
//人工写((u8*)dest)[i]可以访问到这些无意义的数"结果你的有效数据就湮灭了"
//Aligned: fontok=1, ugbkaddr=134678021, ugbksize=202050057
//Packed: fontok=1, ugbkaddr=84148994, ugbksize=151521030
//fontok Aligned编译器自动填充3个无意义数 0x2, 0x3, 0x4添加了这个空穴里湮灭了读不到
//ugbkaddr读到 0x5 0x6 0x7 0x8为 0x08070605 为134678021 结果134678021成立
//size读到 0x09,0x0A,0x0B,0X0C 为0x0C0B0A09 为202050057 print结果202050057成立
//fontok Packed添加了这个空穴里湮灭了读不到
//ugbkaddr读到 0x2, 0x3, 0x4, 0x5 0x05040302 为84,148,994 结84148994成立
//size读到 	   0x6, 0x7, 0x8,0x09 为0x09080706 为151521030 print结果151521030成立


extern u32 FONTINFOADDR;

typedef struct __attribute__((packed)) {
    u8 fontok;                //??????ì0X55ì???????ì?????
    u32 ugbkaddr;            //unigbk???
    u32 ugbksize;            //unigbk??С
    u32 f12addr;            //gbk12??
    u32 gbk12size;            //gbk12??С
    u32 f16addr;            //gbk16??
    u32 gbk16size;            //gbk16??С
    u32 f24addr;            //gbk24??
    u32 gbk24size;            //gbk24??С
    u32 f32addr;            //gbk32??
    u32 gbk32size;            //gbk32??С
} _font_info;



extern _font_info ftinfo;    //??х????

u32 fupd_prog(u16 x, u16 y, u8 size, u32 fsize, u32 pos);    //Д??т??
u8 updata_fontx(u16 x, u16 y, u8 size, u8 *fxpath, u8 fx);    //?т????
u8 update_font(u16 x, u16 y, u8 size, u8 *src);            //?т????
u8 font_init(void);                                        //?????


#endif
