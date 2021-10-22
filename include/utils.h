#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <stddef.h>


#define LOG_MSG(MSG)  indicateError(__FILE__, __LINE__, MSG)


void Utils_Init (void);

inline void delay (size_t ticks);
inline void delay_ms (size_t msec);
inline void delay_us (size_t usec);

void custom_itoa(char *result, size_t bufsize, int number);
void itox(char *result, size_t bufsize, int number);

uint16_t crc16 (uint16_t initVal, uint8_t *startAddr, size_t len);

//inline void indicateError (void);
inline void indicateError (const char *file, const int line, const char *msg);


#endif  //  __UTILS_H__
