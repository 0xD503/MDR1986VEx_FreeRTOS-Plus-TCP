#include "app_includes.h"


uint8_t rxBuff[128] = "Initial Rx buffer filling";
uint8_t txBuff[128] = "Initial Tx buffer filling";

TaskHandle_t xEthernet_RxHandle = NULL;
TaskHandle_t xDataExchange_Handle = NULL;
TaskHandle_t xOE_Listener_Handle = NULL;

TimerHandle_t xTx_Tmr = NULL;


/// RTOS hooks
/**
 * will only be called if
 * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
 * function that will get called if a call to pvPortMalloc() fails.
 * pvPortMalloc() is called internally by the kernel whenever a task, queue,
 * timer or semaphore is created.  It is also called by various parts of the
 * demo application.  If heap_1.c or heap_2.c are used, then the size of the
 * heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
 * FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
 * to query the size of free heap space that remains (although it does not
 *  provide information on how the remaining heap might be fragmented).
*/
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();

    for (;;) {};
}

/**
 * <pre>void vApplicationStackOverflowHook( TaskHandle_t xTask char *pcTaskName); </pre>
 *
 * The application stack overflow hook is called when a stack overflow is detected for a task.
 *
 * Details on stack overflow detection can be found here: https://www.FreeRTOS.org/Stacks-and-stack-overflow-checking.html
 *
 * @param xTask the task that just exceeded its stack boundaries.
 * @param pcTaskName A character string containing the name of the offending task.
 */
/// USE IN DEBUG MODE ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/// USE IN DEBUG MODE ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/// USE IN DEBUG MODE ONLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void vApplicationStackOverflowHook (TaskHandle_t xTask, char * pcTaskName)
{
    taskDISABLE_INTERRUPTS();

    for (;;) {};
}

/// This hook function is called in the system tick handler after any OS work is completed.
void vApplicationTickHook (void)
{
    //
}


/// this function is called automatically by the idle task once per iteration
/// of the idle task loop. You can enter SLEEP mode or something like in this
/// task. Don't use blocking actions!!!
void vApplicationIdleHook (void)
{
    //do something
    //Power_EnterLowPowerMode();
}



/// hooks for static allocator
void vApplicationGetIdleTaskMemory (StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize)
{
    /// if the buffers to be provided to the Idle task are declared inside this
    /// function then they must be declared static - otherwise they will be
    /// allocated on the stack and so not exists after this function exits
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /// pass out the pointer to the StaticTask_t structure in which the Idle
    /// task's state will be stored
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /// pass out the array that will be used as the Idle task's stack
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /// pass out the size of the array pointed to by *ppxIdletaskStackBuffer.
    /// note that, as the array is neccessarily of type StackType_t,
    /// configMINIMAL_STACK_SIZE is specified IN WORDS, NOT BYTES
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize)
{
    /// if the buffers to be provided to the Timer task are declared inside this
    /// function then they must be declared static - otherwise they will be
    /// allocated on the stack and so not exists after this function exits
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /// pass out the pointer to the StaticTask_t structure in which the Timer
    /// task's state will be stored
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /// pass out the array that will be used as the Timer task's stack
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /// pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    /// note that, as the array is neccessarily of type StackType_t,
    /// configTIMER_TASK_STACK_DEPTH is specified IN WORDS, NOT BYTES
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
