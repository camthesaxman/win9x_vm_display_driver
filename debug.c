#include "debug.h"

#pragma code_seg("_INIT")

#ifdef DEBUG

// outputs a string to the QEMU debug port 0xE9
void debug_print(const char* string)
{
    __asm {
        lds si, string
        mov dx, 0xE9
      again:
        cmp BYTE PTR [si], 0
        jz done
        outsb
        jmp again
        done:
    }
}

// converts a number to a hex string
char* debug_hex(unsigned long int n)
{
    static const char hexdigits[] = "0123456789ABCDEF";
    static char buf[9];
    int i;

    for (i = 0; i < 8; i++)
    {
        buf[7-i] = hexdigits[n & 0xF];
        n >>= 4;
    }
    buf[8] = 0;
    return buf;
}

#endif
