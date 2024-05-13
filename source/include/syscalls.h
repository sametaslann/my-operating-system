 
#ifndef __MYOS__SYSCALLS_H
#define __MYOS__SYSCALLS_H

#include <common/types.h>
#include <hardwarecommunication/interrupts.h>
#include <multitasking.h>

namespace myos
{
    
    class SyscallHandler : public hardwarecommunication::InterruptHandler
    {
        common::uint32_t Schedule(common::uint32_t esp);

        
    public:
        SyscallHandler(hardwarecommunication::InterruptManager* interruptManager, myos::common::uint8_t InterruptNumber);
        ~SyscallHandler();
        
        virtual myos::common::uint32_t HandleInterrupt(myos::common::uint32_t esp);

    };

    namespace syscalls
    {
        enum SYSCALLS{ EXIT, FORK, WAITPID, GETPID, EXECV, PRINTF, ADDTASK};
        void exit();
        common::uint32_t execve(void (*entrypoint)());
        common::uint32_t fork();
        void waitpid(common::uint16_t pid);
        void sysprintf(char* str);
    }
    
    
}


#endif