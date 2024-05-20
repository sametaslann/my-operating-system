#ifndef __MYOS__MYSTDLIB__H
#define __MYOS__MYSTDLIB__H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>

using namespace myos::hardwarecommunication;
using namespace myos::drivers;
using namespace myos::common;

namespace myos
{
    namespace mystd
    {
        


        void printf(char* str);
        void printfHex(uint8_t key);
        void printfHex16(uint16_t key);
        void printfHex32(uint32_t key);
        void printfDigit(int digit);
        int atoi(char *str, int *out);
        void readline(char * res);
        void scanf(char* label, int *res);
        unsigned rand();
        void sleep(int n);

        
    }
}

#endif
