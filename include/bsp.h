/*
 *  Board Support Package
 */

#ifndef __BSP_H__
#define __BSP_H__

#include <stdbool.h>


struct DeviceInitStruct {
    volatile struct DeviceInfo *info;
    bool startAfterSetup;
};


void prvSetupHardware (void *pvParameters);
void prvStartHardware (void *pvParams);

#endif  //  __BSP_H__
