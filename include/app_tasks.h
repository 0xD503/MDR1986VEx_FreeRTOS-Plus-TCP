#ifndef __APP_TASKS_H__
#define __APP_TASKS_H__

#include "FreeRTOS.h"
#include "timers.h"



/// IRQ tasks
void vETHERNET_IRQ_RxPacketHandler_Task (void *pvParams);

/// application tasks
void vTalker_Task (void *pvParams);
void vListener_Task (void *pvParams);

/// FreeRTOS timers callbacks
void prvStandbyTxTimer_Callback (TimerHandle_t xTimer);


#endif  //  __APP_TASKS_H__
