/**
  * PRNG - pseudo-random number generator
  */

#include "prng.h"


#if (PRNG_TYPE == 1)

#error "No internal PRNG in this MCU"
#elif (PRNG_TYPE == 2)

#include "MDR32F9Qx_timer.h"

#include "clock.h"
#include "hw_timers.h"
#include "utils.h"
#elif (PRNG_TYPE == 3)

#include "FreeRTOS.h"
#include "task.h"

#include "utils.h"
#elif (PRNG_TYPE == 4)

#error "Not implemented yet"
#endif  //  PRNG_TYPE


void PRNG_Init (bool enable)
{
#if (PRNG_TYPE == 1)

#error "No internal PRNG in this MCU"
#elif (PRNG_TYPE == 2)

    /// Use the hardware Timer as a number generator
    MDR_TIMER_TypeDef *timer = PRNG_GENERATOR;
    uint32_t evntSrc = PRNG_EVNT_SRC;
 //   IRQn_Type PRNG_IRQ_NUM;

    Timer_ClockInit(TIMER_2);

    TIMER_CntInitTypeDef countConfig;
    TIMER_CntStructInit(&countConfig);
    countConfig.TIMER_Prescaler = TIMER_PRESCALER_8;    /// HCLK / 32 / TIMER_PRESCALER =
    countConfig.TIMER_Period = 0xFFFFFFFF;

    TIMER_CntInit(timer, &countConfig);
    TIMER_CntEventSourceConfig(timer, evntSrc);

    //TIMER_ITConfig(timer, TIMER_IT_CNT_ARR_EVENT, ENABLE);
    //NVIC_EnableIRQ(irqNum);

    if (enable) TIMER_Cmd(timer, ENABLE);
    sdf
#elif (PRNG_TYPE == 3)

    /// use RTOS API, no init is neccessary
#elif (PRNG_TYPE == 4)

#error "Not implemented yet"
#endif  //  PRNG_TYPE
}


uint32_t __readNumber (void)
{
#if (PRNG_TYPE == 1)

#error "No internal PRNG in this MCU"
#elif (PRNG_TYPE == 2)

    asd
    /// read hardware timer cnt value
    return (PRNG_GENERATOR->CNT);

#elif (PRNG_TYPE == 3)

    return ((uint32_t) xTaskGetTickCount());
#elif (PRNG_TYPE == 4)

#error "Not implemented yet"
#endif  //  PRNG_TYPE
}

void PRNG_GetRandomNumber (uint32_t *seed)
{
    delay(*seed & 0x1F);            /// small delay
    uint32_t num = __readNumber();
    uint32_t seedVal = *seed;

    //*seed = ((num << (seedVal % 7)) + (seedVal % 773) - num + seedVal);
    //*seed = num;
    *seed ^= ((num << (num & 0x01) | (num & 0x01)) + (seedVal % 773) + (num >> 16) + seedVal);
}
