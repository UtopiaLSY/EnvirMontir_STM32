#ifndef __DEBUG_CONF_H
#define __DEBUG_CONF_H
#include "LED.h"
#include <stdio.h>

#define     DEBUG_PRINT   1
#define     ASSERT_ERROR(x)   {LED0 = LED_ON;printf("Assertion \"%s\" failed at line %d in %s\r\n", x, __LINE__, __FILE__);while(1);}

#define     TRUE    1
#define     FALSE   0
#endif
