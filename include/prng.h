/**
  * PRNG - pseudo-random number generator
  */

#ifndef __PRNG_H__
#define __PRNG_H__

#include <stdbool.h>
#include <stdint.h>


#define PRNG_TYPE       3 /* 1 - internal prng, 2- hw timer, 3 - using RTOS api, 4 - read IN port val in Hi-Z */

/// defs for PRNG_TYPE == HW_TIMER
#define PRNG_GENERATOR  MDR_TIMER2
#define PRNG_EVNT_SRC   TIMER_EvSrc_TM2
#define PRNG_IRQ_NUM    TIMER2_IRQn


enum PRNG_TYPES {
    INTERNAL = 1, HW_TIMER, RTOS_API = 3, HI_Z
};


void PRNG_Init (bool enable);

void PRNG_GetRandomNumber (uint32_t *seed);


#endif  //  __PRNG_H__
