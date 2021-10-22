#ifndef __APP_INCLUDES__
#define __APP_INCLUDES__

/// RTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"


#define NET_MASK_ADDR_0 255
#define NET_MASK_ADDR_1 255
#define NET_MASK_ADDR_2 255
#define NET_MASK_ADDR_3 0

#define IP_ADDR_0       192
#define IP_ADDR_1       168
#define IP_ADDR_2       1
#define IP_ADDR_3       55

#define GW_ADDR_0       192
#define GW_ADDR_1       168
#define GW_ADDR_2       1
#define GW_ADDR_3       1

#define DNS_ADDR0       196
#define DNS_ADDR1       46
#define DNS_ADDR2       1
#define DNS_ADDR3       215

#define RH_IP_ADDR_0    192
#define RH_IP_ADDR_1    168
#define RH_IP_ADDR_2    1
#define RH_IP_ADDR_3    7

#define LISTENER_PORT   47776
#define TALK_TO_PORT    47777



extern TaskHandle_t xDataExchange_Handle;
extern TaskHandle_t xOE_Listener_Handle;

extern TimerHandle_t xTx_Tmr;


#endif  //  __APP_INCLUDES__
