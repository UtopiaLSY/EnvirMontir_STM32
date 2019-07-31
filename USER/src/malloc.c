#include "malloc.h"

//内存池
__align(32) u8 INMEMBase[INMEM_MAXSIZE];                  //内部内存池
__align(32) u8 EXMEMBase[EXMEM_MAXSIZE] __attribute__((at(0x68000000)));                  //外内存池
//内存管理表
u16 INMEMMapBase[INMEM_ALLOC_TABLESIZE];                         //内部内存管理表
u16 EXMEMMapBase[EXMEM_ALLOC_TABLESIZE] __attribute__((at(0X68000000+EXMEM_MAXSIZE)));      //外部内存管理表
//内存管理参数
const u32 memtabsize[SRAMBANK] = {INMEM_ALLOC_TABLESIZE, EXMEM_ALLOC_TABLESIZE};
const u32 memblocksize[SRAMBANK] = {INMEM_BLOCKSIZE, EXMEM_BLOCKSIZE};
const u32 memsize[SRAMBANK] = {INMEM_MAXSIZE, EXMEM_MAXSIZE};


//局部函数
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
*操作:复制内存
*说明:des:目的地址
        src:源地址
        len:数据长度
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
*操作:设置内存
*说明:des:目的地址
      data：数据
      len：数据长度
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
*操作:初始化内存池和内存管理表
*说明:memflag:内/外内存片选择
************************************/
void my_mem_init(u8 memflag)
{
    mymemset(malloc_dev.membase[memflag], 0, memsize[memflag]);
    mymemset(malloc_dev.memmap[memflag], 0, memtabsize[memflag] * 2); //memtabsize声明为为2字节，mymemset（）中的data声明为1字节，因此需要*2
    malloc_dev.memreadyflag[memflag] = 1;
}

/************************************
*操作:获取内存使用率
*说明:memflag:内/外内存片选择
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
    for(offset = memtabsize[memflag]-1; offset >= 0; offset--)  //在内存管理表中搜索连续空闲块
    {
        if(!malloc_dev.memmap[memflag][offset])         //寻找连续的内存块
            memblock_serinum++;
        else 
            memblock_serinum = 0;
        if(memblock_neednum == memblock_serinum)
        {
            for(i = 0; i < memblock_neednum; i++)                   //标记占用的内存块
                malloc_dev.memmap[memflag][offset+i] = memblock_neednum; 
            return (offset * memblocksize[memflag]);
        }
        
    }
    return 0xFFFFFFFF;
}

/************************************
*操作:分配内存空间
*说明:memflag：内/外内存片选择
      size：申请大小
     返回值：分配到的内存首地址
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
        return 1;       //内存片未初始化
    }
    if(offset < memsize[memflag])
    {
        index = offset / memblocksize[memflag];             //找到偏移地址对应的内存块号
        memblock_num = malloc_dev.memmap[memflag][index];   //被连续占用的内存块数
        for(i = 0; i < memblock_num; i++)
        {
            malloc_dev.memmap[memflag][index+i] = 0;          //内存管理表清零
        }
        return 0;
    }
    return 2;       //偏移地址超区
}

/************************************
*操作:释放内存空间
*说明:memflag：内/外内存片选择
      ptr：需释放地址
************************************/
void myfree(u8 memflag, void *ptr)
{
    u32 offset;
    if(ptr == NULL) return;
    offset = (u32)ptr - (u32)malloc_dev.membase[memflag];
    my_mem_free(memflag, offset);
}

/************************************
*操作:重新分配内存空间
*说明:memflag：内/外内存片选择
      ptr：就内存空间
      size：要分配的空间大小
      返回值：新分配到的内存首地址
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

