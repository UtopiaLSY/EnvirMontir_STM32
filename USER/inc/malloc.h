#include "stm32f10x.h"
#ifndef __MALLOC_H
#define __MALLOC_H



#ifndef NULL
#define NULL 0
#endif


#define     SRAMBANK                2                           //管理的内存片数
#define     INSRAM                  0                           //内部内存池
#define     EXSRAM                  1                           //外部内存池

//内存内存池
#define     INMEM_BLOCKSIZE           32                          //内存分配时的最小单元
#define     INMEM_MAXSIZE             30*1024                     //最大管理内存池40K
#define     INMEM_ALLOC_TABLESIZE     INMEM_MAXSIZE/INMEM_BLOCKSIZE //内存表大小
//外部内存池
#define     EXMEM_BLOCKSIZE           32                          //内存分配时的最小单元
#define     EXMEM_MAXSIZE             960*1024                     //最大管理内存池960K
#define     EXMEM_ALLOC_TABLESIZE     EXMEM_MAXSIZE/EXMEM_BLOCKSIZE //内存表大小




struct _malloc_dev
{
    void (*init)(u8);           //初始化
    u8 (*preused)(u8);          //内存使用率
    u8 *membase[SRAMBANK];      //内存池，管理SRAMBANK个区域的内存
    u16 *memmap[SRAMBANK];      //内存管理表
    u8 memreadyflag[SRAMBANK];  //内存管理是否就绪
};

extern struct _malloc_dev malloc_dev;

void mymemcpy(void *des, void *src, u32 len);
void mymemset(void *des, u8 data, u32 len);
void *mymalloc(u8 memflag, u32 size);
void myfree(u8 memflag, void *ptr);
void *myrealloc(u8 memflag, void *ptr, u32 size);
void my_mem_init(u8 memflag);

#endif
