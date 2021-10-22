#include "utils.h"

#include <string.h>


/* Ensure stdint is only used by the compiler, and not the assembler. */
#ifdef __ICCARM__
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
#endif


static size_t __ticks_in_ms;
static size_t __ticks_in_us;

static const uint16_t crc16_NibbleTable[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xE1CE, 0xF1EF
};



void Utils_Init (void)
{
    __ticks_in_ms = SystemCoreClock / 1000 / 10;    /// 10????
    __ticks_in_us = SystemCoreClock / 1000000 / 10; /// 10????
}


void delay (size_t ticks)
{
    volatile size_t c;

    for (c = 0; c < ticks; c++) { /*__ASM("nop");*/ };
}

void delay_ms (size_t msec)
{
    volatile size_t c;

    const size_t ticks = __ticks_in_ms * msec;
    for (c = 0; c < ticks; c++) { /*__ASM("nop");*/ };
}

void delay_us (size_t usec)
{
    volatile size_t c;

    const size_t ticks = __ticks_in_us * usec;
    for (c = 0; c < ticks; c++) { /*__ASM("nop");*/ };
}

void custom_itoa(char *result, size_t bufsize, int number)
{
    char *res = result;
    char *tmp = result + bufsize - 1;
    int n = (number >= 0) ? number : -number;

    /* handle invalid bufsize */
    if (bufsize < 2) {
        if (bufsize == 1) {
            *result = 0;
        }
        return;
    }

    /* First, add sign */
    if (number < 0) {
        *res++ = '-';
    }
    /* Then create the string from the end and stop if buffer full,
     and ensure output string is zero terminated */
    *tmp = 0;
    while ((n != 0) && (tmp > res)) {
        char val = (char)('0' + (n % 10));
        tmp--;
        *tmp = val;
        n = n / 10;
    }
    if (n) {
        /* buffer is too small */
        *result = 0;
        return;
    }
    if (*tmp == 0) {
        /* Nothing added? */
        *res++ = '0';
        *res++ = 0;
        return;
    }
    /* move from temporary buffer to output buffer (sign is not moved) */
    memmove(res, tmp, (size_t)((result + bufsize) - tmp));
}

/*
 *  itox()
 *
 *  Converts integer <number> to the string of maximum size <bufsize> in HEX
 *  representation
 */
void itox(char *result, size_t bufsize, int number)
{
    static const char referenceLUT[16] = "0123456789ABCDEF";
    char *res = result;
    char *tmp = result + bufsize - 1;
    int n = number;

    /* handle invalid bufsize */
    if (bufsize < 2) {
        if (bufsize == 1) {
            *result = 0;
        }
        return;
    }

    /* First,  */
    if (number == 0)
    {
        result[0] = '0';
        result[1] = '\0';
        return;
    }

    /* Then create the string from the end and stop if buffer full,
     and ensure output string is zero terminated */
    *tmp = 0;
    while ((n != 0) && (tmp > res)) {
        char val = (char)(referenceLUT[n % 16]);
        *(--tmp) = val;
        n = n / 16;
    }
    if (n) {
        /* buffer is too small */
        *result = 0;
        return;
    }
    if (*tmp == 0) {
        /* Nothing added? */
        *res++ = '0';
        *res++ = 0;
        return;
    }
    /* move from temporary buffer to output buffer */
    memmove(res, tmp, (size_t)((result + bufsize) - tmp));
}


/*
 *  uint16_t crc16(uint16_t checksum, uint8_t *startAddr, size_t len)
 *
 *  Computes crc16 sum of <len> bytes starting from <startAddr>, using <checksum>
 *  as an initial value
 */
uint16_t crc16 (uint16_t initVal, uint8_t *startAddr, size_t len)
{

    while (len--)
    {
        initVal = crc16_NibbleTable[(initVal >> 12) ^ (*startAddr >> 4)] ^ (initVal << 4);   /// high nibble
        initVal = crc16_NibbleTable[(initVal >> 12) ^ (*startAddr++ & 0x0F)] ^ (initVal << 4);   /// low nibble
    }

    return (initVal);
}


/*
 * Indicates a device error using the device LEDs, UART, or something other.
 */
void indicateError (const char *file, const int line, const char *msg)
{
    /// provide error indication
    //
}


//int fputc (
