
#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <mystdlib.h>

using namespace myos::hardwarecommunication;
using namespace myos::drivers;
using namespace myos::common;

namespace myos
{
    namespace mystd
    {
        
        

        void printf(char* str)
        {
            static uint16_t* VideoMemory = (uint16_t*)0xb8000;

            static uint8_t x=0,y=0;

            for(int i = 0; str[i] != '\0'; ++i)
            {
                switch(str[i])
                {
                    case '\n':
                        x = 0;
                        y++;
                        break;
                    default:
                        VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | str[i];
                        x++;
                        break;
                }

                if(x >= 80)
                {
                    x = 0;
                    y++;
                }

                if(y >= 25)
                {
                    for(y = 0; y < 25; y++)
                        for(x = 0; x < 80; x++)
                            VideoMemory[80*y+x] = (VideoMemory[80*y+x] & 0xFF00) | ' ';
                    x = 0;
                    y = 0;
                }
            }
        }


        void printfHex(uint8_t key)
        {
            char* foo = "00";
            char* hex = "0123456789ABCDEF";
            foo[0] = hex[(key >> 4) & 0xF];
            foo[1] = hex[key & 0xF];
            printf(foo);
        }

        void printfHex16(uint16_t key)
        {
            printfHex((key >> 8) & 0xFF);
            printfHex( key & 0xFF);
        }
        
        void printfHex32(uint32_t key)
        {
            printfHex((key >> 24) & 0xFF);
            printfHex((key >> 16) & 0xFF);
            printfHex((key >> 8) & 0xFF);
            printfHex( key & 0xFF);
        }

        void printfDigit(int digit) 
        {
            char buff[256];
            int n; 
            int i;
            
            if (digit < 0) {
                digit *= -1;
                buff[0] = '-';
                i = n = 1;
            }
            else {
                i = n = 0;
            }

            do {
                buff[n] = '0' + (digit % 10);
                digit /= 10;
                ++n;
            } while (digit > 0);

            buff[n] = '\0';
            
            while (i < n / 2) {
                int temp = buff[i];
                buff[i] = buff[n - i - 1];
                buff[n - i - 1] = temp;
                ++i;        
            }
            printf((char *) buff);
        }


        int atoi(char *str, int *out)
        {
            char *start = str;
            int sign;
            int num = 0;
            
            // sign check
            if ((sign = (str[0] == '-' || str[0] == '+')))
                ++str;

            while (*str != '\0') {
                if (*str < '0' || '9' < *str)
                    return -1;
                num =  num * 10 + (*str - '0');
                ++str; 
            }
            *out = sign ? (((start[0] == '-') ? -1 : 1) * num) : (num);
            return 0;
        }


        void readline(char * res)
        {
            InterruptManager::isWaitingForInput = true;

            char *str;
            while ((str = KeyboardEventHandler::readFromBuffer()) == nullptr);

            InterruptManager::isWaitingForInput = false;
            int i;
            for(i = 0; str[i] != '\0'; i++)
                res[i] = str[i];
            res[i] = '\0';
        }

        void scanf(char* label, int *res)
        {
            char str[256];
            printf(label);
            readline(str);
            if(atoi(str, res) == -1)
            {
                printf("Invalid input !");
                res = 0;
            }
        }



        uint16_t lfsr = 0xACE1u;
        unsigned bit;
        unsigned rand()
        {
            bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
            return lfsr =  (lfsr >> 1) | (bit << 15);
        }

        void sleep(int n){
            for(int i = 0; i < n; i++)
                printf("");

        }

    }
    
}

