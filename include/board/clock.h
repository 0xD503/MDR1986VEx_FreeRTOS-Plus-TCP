#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <stddef.h>


void System_ClockInit (void);

/// peripheral clk
inline void Ports_ClockInit (void);
inline void ADC_ClockInit (void);
inline void UART_ClockInit (size_t uartNum);
inline void Ethernet_ClockInit (void);
inline void Timer_ClockInit (size_t timerNum);
inline void SPI_ClockInit (void);
inline void LED_ClockInit (void);


#endif  //  __CLOCK_H__
