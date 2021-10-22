#ifndef __MAIN_H__
#define __MAIN_H__


//#define CPU_CLK_MUL 9

/// priorities
#define IDLE_TASK_PRIORITY  tskIDLE_PRIORITY
#define LOW_PRIORITY        1
#define MEDIUM_PRIORITY     2
#define HIGH_PRIORITY       3
#define RT_PRIORITY         (configMAX_PRIORITIES - 2)
#define IRQ_PRIORITY        (configMAX_PRIORITIES - 1)


#endif  //  __MAIN_H__
