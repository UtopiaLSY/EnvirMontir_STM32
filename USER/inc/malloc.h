#include "stm32f10x.h"
#ifndef __MALLOC_H
#define __MALLOC_H



#ifndef NULL
#define NULL 0
#endif


#define     SRAMBANK                2                           //������ڴ�Ƭ��
#define     INSRAM                  0                           //�ڲ��ڴ��
#define     EXSRAM                  1                           //�ⲿ�ڴ��

//�ڴ��ڴ��
#define     INMEM_BLOCKSIZE           32                          //�ڴ����ʱ����С��Ԫ
#define     INMEM_MAXSIZE             30*1024                     //�������ڴ��40K
#define     INMEM_ALLOC_TABLESIZE     INMEM_MAXSIZE/INMEM_BLOCKSIZE //�ڴ���С
//�ⲿ�ڴ��
#define     EXMEM_BLOCKSIZE           32                          //�ڴ����ʱ����С��Ԫ
#define     EXMEM_MAXSIZE             960*1024                     //�������ڴ��960K
#define     EXMEM_ALLOC_TABLESIZE     EXMEM_MAXSIZE/EXMEM_BLOCKSIZE //�ڴ���С




struct _malloc_dev
{
    void (*init)(u8);           //��ʼ��
    u8 (*preused)(u8);          //�ڴ�ʹ����
    u8 *membase[SRAMBANK];      //�ڴ�أ�����SRAMBANK��������ڴ�
    u16 *memmap[SRAMBANK];      //�ڴ�����
    u8 memreadyflag[SRAMBANK];  //�ڴ�����Ƿ����
};

extern struct _malloc_dev malloc_dev;

void mymemcpy(void *des, void *src, u32 len);
void mymemset(void *des, u8 data, u32 len);
void *mymalloc(u8 memflag, u32 size);
void myfree(u8 memflag, void *ptr);
void *myrealloc(u8 memflag, void *ptr, u32 size);
void my_mem_init(u8 memflag);

#endif
