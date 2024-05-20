
#include <syscalls.h>
#include <mystdlib.h>
using namespace myos;
using namespace myos::common;
using namespace myos::syscalls;
using namespace myos::mystd;
using namespace myos::hardwarecommunication;

/*syscall interrupts*/

/*
eax for syscall number
ecx for parameter
ebx for return
*/ 

void syscalls::exit() {
    asm("int $0x80" : : "a" (SYSCALLS::EXIT));
}

uint32_t syscalls::execve(void (*entrypoint)()) {
    uint32_t ret = -1;

    asm("int $0x80" :: "a" (SYSCALLS::EXECV), "c" ((uint32_t)entrypoint));
    __asm__ volatile("" : "=b"(ret));
    return ret;
    
}
void syscalls::nice(Priority newPriority)
{
    asm("int $0x80" :: "a" (SYSCALLS::NICE), "c" ((int)newPriority));
}

uint32_t syscalls::fork()
{
    uint32_t ret = -1;

    asm("int $0x80" :: "a" (SYSCALLS::FORK));
    __asm__ volatile("" : "=b"(ret));

    return ret;
}

void syscalls::waitpid(uint16_t pid) {
    asm("int $0x80" : : "a" (SYSCALLS::WAITPID), "c" (pid));
}


void syscalls::sysprintf(char* str)
{
    asm("int $0x80" : : "a" (SYSCALLS::PRINTF), "c" (str));

}




 
SyscallHandler::SyscallHandler(InterruptManager* interruptManager, uint8_t InterruptNumber)
:    InterruptHandler(interruptManager, InterruptNumber  + interruptManager->HardwareInterruptOffset())
{
}

SyscallHandler::~SyscallHandler()
{
}




uint32_t SyscallHandler::Schedule(uint32_t esp)
{
    return (uint32_t)(interruptManager->taskManager->Schedule((CPUState*)esp));
}




uint32_t SyscallHandler::HandleInterrupt(uint32_t esp)
{
    CPUState* cpu = (CPUState*)esp;
    
    
    switch(cpu->eax)
    {
        case SYSCALLS::EXIT:
            interruptManager->taskManager->sys_exit();
            return SyscallHandler::Schedule(esp);
            break;

        case SYSCALLS::FORK:
            cpu->ebx = interruptManager->taskManager->sys_fork(cpu);
            return SyscallHandler::Schedule(esp);
        break;
        
        case SYSCALLS::WAITPID:
            if(interruptManager->taskManager->sys_waitpid(cpu->ecx)){
                return SyscallHandler::Schedule(esp);
            }
            break;

        case SYSCALLS::EXECV:
            cpu->ebx = interruptManager->taskManager->sys_execve((void (*)())(cpu->ecx));
            return SyscallHandler::Schedule(esp);
            break;

        case SYSCALLS::NICE:
            
            interruptManager->taskManager->sys_nice((Priority)cpu->ecx);
            return SyscallHandler::Schedule(esp);
            break;
                    
        default:
            break;
    }

    
    return esp;
}

