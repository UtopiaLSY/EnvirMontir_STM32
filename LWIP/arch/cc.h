/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __CC_H__
#define __CC_H__

#include "stdio.h"

/*-------------data type------------------------------------------------------*/
typedef unsigned   char     u8_t;    /* Unsigned 8 bit quantity         */
typedef signed     char     s8_t;    /* Signed    8 bit quantity        */
typedef unsigned   short    u16_t;   /* Unsigned 16 bit quantity        */
typedef signed     short    s16_t;   /* Signed   16 bit quantity        */
typedef unsigned   int      u32_t;   /* Unsigned 32 bit quantity        */
typedef signed     int      s32_t;   /* Signed   32 bit quantity        */
typedef unsigned   int      mem_ptr_t;              //内存地址型数据
typedef unsigned   int      sys_prot_t;             //临界保护型数据
/*----------------------------------------------------------------------------*/


//定义结构体相关的宏
#define PACK_STRUCT_FIELD(x)    x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN       __packed    //__packed是编译器中限
#define PACK_STRUCT_END                     //定字节对齐的关键字

//定义调试信息输出宏
#define LWIP_DEBUG
#define LWIP_PLATFORM_DIAG(x)       {printf x;}
#define LWIP_PLATFORM_ASSERT(x) \
    do \
    {   printf("Assertion \"%s\" failed at line %d in %s\r\n", x, __LINE__, __FILE__); \
    } while(0)
#define LWIP_ERROR(message, expression, handler) \
        do { if(!(expression)) { \
                printf(message);handler;} \
        }while(0)
        
        
/*---define (sn)printf formatters for these lwip types, for lwip DEBUG/STATS--*/
#define U16_F   "u"
#define S16_F   "d"
#define X16_F   "x"
#define U32_F   "u"
#define S32_F   "d"
#define X32_F   "x"

        
#define     LWIP_PROVIDE_ERRNO              //使用LWIP内部自带的错误码
#define     BYTE_ORDER  LITTLE_ENDIAN       //处理器定义为小端模式


        
#endif /* __CC_H__ */
