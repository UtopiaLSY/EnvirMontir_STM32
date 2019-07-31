#include "malloc.h"

//�ڴ��
__align(32) u8 INMEMBase[INMEM_MAXSIZE];                  //�ڲ��ڴ��
__align(32) u8 EXMEMBase[EXMEM_MAXSIZE] __attribute__((at(0x68000000)));                  //���ڴ��
//�ڴ�����
u16 INMEMMapBase[INMEM_ALLOC_TABLESIZE];                         //�ڲ��ڴ�����
u16 EXMEMMapBase[EXMEM_ALLOC_TABLESIZE] __attribute__((at(0X68000000+EXMEM_MAXSIZE)));      //�ⲿ�ڴ�����
//�ڴ�������
const u32 memtabsize[SRAMBANK] = {INMEM_ALLOC_TABLESIZE, EXMEM_ALLOC_TABLESIZE};
const u32 memblocksize[SRAMBANK] = {INMEM_BLOCKSIZE, EXMEM_BLOCKSIZE};
const u32 memsize[SRAMBANK] = {INMEM_MAXSIZE, EXMEM_MAXSIZE};


//�ֲ�����
u32 my_mem_malloc(u8 memflag, u32 size);
u8 my_mem_free(u8 memflag, u32 offset);

u8 my_mem_preused(u8 memflag);

struct _malloc_dev malloc_dev = 
{
    my_mem_init,
    my_mem_preused,
    INMEMBase,EXMEMBase,
    INMEMMapBase,EXMEMMapBase,
    0,0,
};


/************************************
*����:�����ڴ�
*˵��:des:Ŀ�ĵ�ַ
        src:Դ��ַ
        len:���ݳ���
************************************/
void mymemcpy(void *des, void *src, u32 len)
{
    u8 *tempdes = des;
    u8 *tempsrc = src;
    while(len--)
    {
        *tempdes = *tempsrc;
        tempdes++;
        tempsrc ++;
    }
}

/************************************
*����:�����ڴ�
*˵��:des:Ŀ�ĵ�ַ
      data������
      len�����ݳ���
************************************/
void mymemset(void *des, u8 data, u32 len)
{
    u8 *tempdes = des;
    while(len--)
    {
        *tempdes = data;
        tempdes++;
    }
}

/************************************
*����:��ʼ���ڴ�غ��ڴ�����
*˵��:memflag:��/���ڴ�Ƭѡ��
************************************/
void my_mem_init(u8 memflag)
{
    mymemset(malloc_dev.membase[memflag], 0, memsize[memflag]);
    mymemset(malloc_dev.memmap[memflag], 0, memtabsize[memflag] * 2); //memtabsize����ΪΪ2�ֽڣ�mymemset�����е�data����Ϊ1�ֽڣ������Ҫ*2
    malloc_dev.memreadyflag[memflag] = 1;
}

/************************************
*����:��ȡ�ڴ�ʹ����
*˵��:memflag:��/���ڴ�Ƭѡ��
************************************/
u8 my_mem_preused(u8 memflag)
{
    u32 used = 0;
    u32 i;
    for(i = 0; i < memtabsize[memflag]; i++)
    {
        if(malloc_dev.memmap[memflag][i])
            used++;
    }
    return (used*100)/(memtabsize[memflag]);
}

u32 my_mem_malloc(u8 memflag, u32 size)
{
    signed long offset = 0;
    u32 memblock_neednum = 0;
    u32 memblock_serinum = 0;
    u32 i;
    if(!malloc_dev.memreadyflag[memflag]) 
        malloc_dev.init(memflag);
    if(size == 0) 
        return 0xFFFFFFFF;
    memblock_neednum = size / memblocksize[memflag];
    if(size % memblocksize[memflag])    
        memblock_neednum++;
    for(offset = memtabsize[memflag]-1; offset >= 0; offset--)  //���ڴ������������������п�
    {
        if(!malloc_dev.memmap[memflag][offset])         //Ѱ���������ڴ��
            memblock_serinum++;
        else 
            memblock_serinum = 0;
        if(memblock_neednum == memblock_serinum)
        {
            for(i = 0; i < memblock_neednum; i++)                   //���ռ�õ��ڴ��
                malloc_dev.memmap[memflag][offset+i] = memblock_neednum; 
            return (offset * memblocksize[memflag]);
        }
        
    }
    return 0xFFFFFFFF;
}

/************************************
*����:�����ڴ�ռ�
*˵��:memflag����/���ڴ�Ƭѡ��
      size�������С
     ����ֵ�����䵽���ڴ��׵�ַ
************************************/
void *mymalloc(u8 memflag, u32 size)
{
    u32 offset;
    offset = my_mem_malloc(memflag, size);
    if(offset == 0xFFFFFFFF)
        return NULL;
    else
        return (void*)((u32)malloc_dev.membase[memflag]+offset);
}

u8 my_mem_free(u8 memflag, u32 offset)
{
    int i;
    int index = 0;
    int memblock_num = 0;
    
    if(!malloc_dev.memreadyflag[memflag])
    {
        malloc_dev.init(memflag);
        return 1;       //�ڴ�Ƭδ��ʼ��
    }
    if(offset < memsize[memflag])
    {
        index = offset / memblocksize[memflag];             //�ҵ�ƫ�Ƶ�ַ��Ӧ���ڴ���
        memblock_num = malloc_dev.memmap[memflag][index];   //������ռ�õ��ڴ����
        for(i = 0; i < memblock_num; i++)
        {
            malloc_dev.memmap[memflag][index+i] = 0;          //�ڴ���������
        }
        return 0;
    }
    return 2;       //ƫ�Ƶ�ַ����
}

/************************************
*����:�ͷ��ڴ�ռ�
*˵��:memflag����/���ڴ�Ƭѡ��
      ptr�����ͷŵ�ַ
************************************/
void myfree(u8 memflag, void *ptr)
{
    u32 offset;
    if(ptr == NULL) return;
    offset = (u32)ptr - (u32)malloc_dev.membase[memflag];
    my_mem_free(memflag, offset);
}

/************************************
*����:���·����ڴ�ռ�
*˵��:memflag����/���ڴ�Ƭѡ��
      ptr�����ڴ�ռ�
      size��Ҫ����Ŀռ��С
      ����ֵ���·��䵽���ڴ��׵�ַ
************************************/
void *myrealloc(u8 memflag, void *ptr, u32 size)
{
    u32 offset;
    offset = my_mem_malloc(memflag, size);
    if(offset == 0xFFFFFFFF)
        return NULL;
    else
    {
        mymemcpy((void*)((u32)malloc_dev.membase[memflag]+offset), ptr, size);
        myfree(memflag, ptr);
        return (void*)((u32)malloc_dev.membase[memflag]+offset);
    }
}

